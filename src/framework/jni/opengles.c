/*
 * opengles.c
 *
 *  Created on: 2014-3-12
 *      Author: kly
 */

#include "opengles.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <sr_library.h>
#include <video_resample.h>

const char g_indices[] =
{ 0, 3, 2, 0, 2, 1 };

const char g_vertextShader[] =
{ "attribute vec4 aPosition;\n"
		"attribute vec2 aTextureCoord;\n"
		"varying vec2 vTextureCoord;\n"
		"void main() {\n"
		"  gl_Position = aPosition;\n"
		"  vTextureCoord = aTextureCoord;\n"
		"}\n" };

// The fragment shader.
// Do YUV to RGB565 conversion.
const char g_fragmentShader[] =
{ "precision mediump float;\n"
		"uniform sampler2D Ytex;\n"
		"uniform sampler2D Utex,Vtex;\n"
		"varying vec2 vTextureCoord;\n"
		"void main(void) {\n"
		"  float nx,ny,r,g,b,y,u,v;\n"
		"  mediump vec4 txl,ux,vx;"
		"  nx=vTextureCoord[0];\n"
		"  ny=vTextureCoord[1];\n"
		"  y=texture2D(Ytex,vec2(nx,ny)).r;\n"
		"  u=texture2D(Utex,vec2(nx,ny)).r;\n"
		"  v=texture2D(Vtex,vec2(nx,ny)).r;\n"

		//"  y = v;\n"+
		"  y=1.1643*(y-0.0625);\n"
		"  u=u-0.5;\n"
		"  v=v-0.5;\n"

		"  r=y+1.5958*v;\n"
		"  g=y-0.39173*u-0.81290*v;\n"
		"  b=y+2.017*u;\n"
		"  gl_FragColor=vec4(r,g,b,1.0);\n"
		"}\n" };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct opengles_t
{
	int32_t _id;
	GLuint _textureIds[3]; // Texture id of Y,U and V texture.
	GLuint _program;
	GLsizei _textureWidth;
	GLsizei _textureHeight;

	GLfloat _vertices[20];

} opengles_t;

static const unsigned int kErrorSize = 4096;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void checkGlError(const char* op)
{
	GLint error;
	for (error = glGetError(); error; error = glGetError())
	{
		LOGD( "after %s() glError (0x%x)\n", op, error);
	}
}

static GLuint loadShader(GLenum shaderType, const char* pSource)
{
	GLuint shader = glCreateShader(shaderType);
	if (shader)
	{
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
			if (infoLen)
			{
				char* buf = (char*) malloc(infoLen);
				if (buf)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGD("Could not compile shader %d: %s", shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

static GLuint createProgram(const char* pVertexSource,
		const char* pFragmentSource)
{
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
	if (!vertexShader)
	{
		return 0;
	}

	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
	if (!pixelShader)
	{
		return 0;
	}

	GLuint program = glCreateProgram();
	if (program)
	{
		glAttachShader(program, vertexShader);
		checkGlError("glAttachShader");
		glAttachShader(program, pixelShader);
		checkGlError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength)
			{
				char* buf = (char*) malloc(bufLength);
				if (buf)
				{
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOGD( "Could not link program: %s", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
}

static void printGLString(const char *name, GLenum s)
{
	const char *v = (const char *) glGetString(s);
	LOGD("GL %s = %s\n", name, v);
}

static void InitializeTexture(int name, int id, int width, int height)
{
	glActiveTexture(name);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
			GL_UNSIGNED_BYTE, NULL);
}

static void SetupTextures(opengles_t *gles, const VideoPacket *frameToRender)
{
	LOGD("width %d, height %d", frameToRender->width, frameToRender->height);

	const GLsizei width = frameToRender->width;
	const GLsizei height = frameToRender->height;

	glGenTextures(3, gles->_textureIds); //Generate  the Y, U and V texture
	InitializeTexture(GL_TEXTURE0, gles->_textureIds[0], width, height);
	InitializeTexture(GL_TEXTURE1, gles->_textureIds[1], width / 2, height / 2);
	InitializeTexture(GL_TEXTURE2, gles->_textureIds[2], width / 2, height / 2);

	checkGlError("SetupTextures");

	gles->_textureWidth = width;
	gles->_textureHeight = height;
}

// Uploads a plane of pixel data, accounting for stride != width*bpp.
static void GlTexSubImage2D(GLsizei width, GLsizei height, int stride,
		const uint8_t* plane)
{
	if (stride == width)
	{
		// Yay!  We can upload the entire plane in a single GL call.
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE,
				GL_UNSIGNED_BYTE, (const GLvoid*) (plane));
	}
	else
	{
		// Boo!  Since GLES2 doesn't have GL_UNPACK_ROW_LENGTH and Android doesn't
		// have GL_EXT_unpack_subimage we have to upload a row at a time.  Ick.
		int row;
		for (row = 0; row < height; ++row)
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, row, width, 1, GL_LUMINANCE,
					GL_UNSIGNED_BYTE, (const GLvoid*) (plane + (row * stride)));
		}
	}
}

static void UpdateTextures(opengles_t *gles, const VideoPacket *frameToRender)
{
	const GLsizei width = frameToRender->width;
	const GLsizei height = frameToRender->height;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gles->_textureIds[0]);
	GlTexSubImage2D(width, height, frameToRender->plane[0].stride,
			frameToRender->plane[0].data);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gles->_textureIds[1]);
	GlTexSubImage2D(width / 2, height / 2, frameToRender->plane[1].stride,
			frameToRender->plane[1].data);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gles->_textureIds[2]);
	GlTexSubImage2D(width / 2, height / 2, frameToRender->plane[2].stride,
			frameToRender->plane[2].data);

	checkGlError("UpdateTextures");
}

int32_t Setup(opengles_t *gles)
{
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	int maxTextureImageUnits[2];
	int maxTextureSize[2];
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, maxTextureImageUnits);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, maxTextureSize);

	LOGD("number of textures %d, size %d", (int) maxTextureImageUnits[0], (int) maxTextureSize[0]);

	gles->_program = createProgram(g_vertextShader, g_fragmentShader);
	if (!gles->_program)
	{
		LOGD( "Could not create program");
		return -1;
	}

	int positionHandle = glGetAttribLocation(gles->_program, "aPosition");
	checkGlError("glGetAttribLocation aPosition");
	if (positionHandle == -1)
	{
		LOGD( "Could not get aPosition handle");
		return -1;
	}

	int textureHandle = glGetAttribLocation(gles->_program, "aTextureCoord");
	checkGlError("glGetAttribLocation aTextureCoord");
	if (textureHandle == -1)
	{
		LOGD( "Could not get aTextureCoord handle");
		return -1;
	}

	// set the vertices array in the shader
	// _vertices contains 4 vertices with 5 coordinates.
	// 3 for (xyz) for the vertices and 2 for the texture
	glVertexAttribPointer(positionHandle, 3, GL_FLOAT, GL_FALSE,
			5 * sizeof(GLfloat), gles->_vertices);
	checkGlError("glVertexAttribPointer aPosition");

	glEnableVertexAttribArray(positionHandle);
	checkGlError("glEnableVertexAttribArray positionHandle");

	// set the texture coordinate array in the shader
	// _vertices contains 4 vertices with 5 coordinates.
	// 3 for (xyz) for the vertices and 2 for the texture
	glVertexAttribPointer(textureHandle, 2, GL_FLOAT, GL_FALSE,
			5 * sizeof(GLfloat), &gles->_vertices[3]);
	checkGlError("glVertexAttribPointer maTextureHandle");
	glEnableVertexAttribArray(textureHandle);
	checkGlError("glEnableVertexAttribArray textureHandle");

	glUseProgram(gles->_program);
	int i = glGetUniformLocation(gles->_program, "Ytex");
	checkGlError("glGetUniformLocation");
	glUniform1i(i, 0); /* Bind Ytex to texture unit 0 */
	checkGlError("glUniform1i Ytex");

	i = glGetUniformLocation(gles->_program, "Utex");
	checkGlError("glGetUniformLocation Utex");
	glUniform1i(i, 1); /* Bind Utex to texture unit 1 */
	checkGlError("glUniform1i Utex");

	i = glGetUniformLocation(gles->_program, "Vtex");
	checkGlError("glGetUniformLocation");
	glUniform1i(i, 2); /* Bind Vtex to texture unit 2 */
	checkGlError("glUniform1i");

	return 0;
}

int opengles_render(opengles_t *gles, const VideoPacket *frameToRender)
{
	if (frameToRender == NULL)
	{
		return -1;
	}

//	log_debug( "id %d", (int) gles->_id);

//	glViewport(0, 0, frameToRender->width, frameToRender->height);

	glUseProgram(gles->_program);
	checkGlError("glUseProgram");

	if (gles->_textureWidth != (GLsizei) frameToRender->width
			|| gles->_textureHeight != (GLsizei) frameToRender->height)
	{
		SetupTextures(gles, frameToRender);
	}
	UpdateTextures(gles, frameToRender);

	glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, g_indices);
	checkGlError("glDrawArrays");

	return 0;
}

int opengles_open(opengles_t **pp_gles)
{
	opengles_t *gles = (opengles_t*) malloc(sizeof(opengles_t));

	const GLfloat vertices[20] =
	{
	// X, Y, Z, U, V
			-1, -1, 0, 0, 1, // Bottom Left
			1, -1, 0, 1, 1, //Bottom Right
			1, 1, 0, 1, 0, //Top Right
			-1, 1, 0, 0, 0 }; //Top Left

	memcpy(gles->_vertices, vertices, sizeof(gles->_vertices));

	Setup(gles);

	*pp_gles = gles;

	return 0;
}

void opengles_close(opengles_t **pp_gles)
{
	if (pp_gles && *pp_gles){
		opengles_t *gles = *pp_gles;
		*pp_gles = NULL;
		free(gles);
	}
}
