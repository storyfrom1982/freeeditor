//
// Created by yongge on 19-7-9.
//

#include "GLVideoObject.h"
#include "GLPlaneShape.h"

using namespace freee;


#define  MAX_UPDATE_FPS (1000000/60)
#define  EYE_Z_POS      0.0f
#define  NEAR_Z_POS     1.0f
#define  VIDEO_Z_POS    100.0f
#define  FAR_Z_POS      (100.0f*2)


#define RTC_PIXEL_FORMAT GL_LUMINANCE
#define RTC_PIXEL_FORMAT_AL GL_LUMINANCE_ALPHA
#define SHADER_VERSION
#define RTC_UVV                 "a"
#define VERTEX_SHADER_IN        "attribute"
#define VERTEX_SHADER_OUT       "varying"
#define FRAGMENT_SHADER_IN      "varying"
#define FRAGMENT_SHADER_OUT
#define FRAGMENT_SHADER_COLOR   "gl_FragColor"
#define FRAGMENT_SHADER_TEXTURE "texture2D"


static const char * g_RC_mVertexShader =
        SHADER_VERSION
        VERTEX_SHADER_IN " vec4 aPosition;\n"
        VERTEX_SHADER_IN " vec3 aColor;\n"
        VERTEX_SHADER_IN " vec2 aTextureCoord;\n"
        "uniform mat4 uMVPMatrixP;\n"
        "uniform mat4 uMVPMatrixW;\n"
        "uniform mat4 uMVPMatrixM;\n"
        "uniform vec3  fHRange;\n"
        VERTEX_SHADER_OUT " vec2 yTextureCoord;\n"
        VERTEX_SHADER_OUT " vec2 uvTextureCoord;\n"
        VERTEX_SHADER_OUT " vec3 vColor;\n"
        "void main() {\n"
        "vec4  pos = uMVPMatrixP * uMVPMatrixW * uMVPMatrixM * aPosition ; \n"
        "gl_Position = pos; \n"
        "yTextureCoord = vec2(aTextureCoord.x*fHRange.x, aTextureCoord.y);\n"
        "uvTextureCoord = vec2(aTextureCoord.x*fHRange.y, aTextureCoord.y);\n"
        "vColor = aColor;\n"
        "}\n";


// RGB convert by http://www.fourcc.org/fccyvrgb.php
//B = 1.164(Y - 16)                  + 2.018(U - 128)
//G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
//R = 1.164(Y - 16) + 1.596(V - 128)
//
//

static const char *      g_RC_mFragmentShaderNV12 =
        "precision mediump float;\n"
        SHADER_VERSION
        FRAGMENT_SHADER_IN " vec2 yTextureCoord;\n"
        FRAGMENT_SHADER_IN " vec2 uvTextureCoord;\n"
        FRAGMENT_SHADER_IN " vec3 vColor;\n"
        "uniform sampler2D  tyy;\n"
        "uniform sampler2D  tuv;\n"
        "uniform sampler2D  tvu;\n"
        "uniform float  alpha;\n"
        FRAGMENT_SHADER_OUT
        "void main() {\n"
        "float y, u , v, b, g, r;\n"
        "y = " FRAGMENT_SHADER_TEXTURE "(tyy, yTextureCoord.xy, 0.0).r;\n"
        "v = " FRAGMENT_SHADER_TEXTURE "(tuv, uvTextureCoord.xy, 0.0)." RTC_UVV ";\n"
        "u = " FRAGMENT_SHADER_TEXTURE "(tuv, uvTextureCoord.xy, 0.0).r;\n"
        "y = 1.164*y - 0.073;\n"
        "u = u - 0.5;\n"
        "v = v - 0.5;\n"
        "b = y + 2.018*u;\n"
        "r = y + 1.596*v;\n"
        "g = y - 0.813*v - 0.391*u;\n"
        //FRAGMENT_SHADER_COLOR " =  vec4(1.0, 0.0, 1.0, 1.0);\n"
        FRAGMENT_SHADER_COLOR " = vec4(r, g, b, alpha); \n"
        "}\n";

static const char *      g_RC_mFragmentShaderNV21 =
        "precision mediump float;\n"
        SHADER_VERSION
        FRAGMENT_SHADER_IN " vec2 yTextureCoord;\n"
        FRAGMENT_SHADER_IN " vec2 uvTextureCoord;\n"
        FRAGMENT_SHADER_IN " vec3 vColor;\n"
        "uniform sampler2D  tyy;\n"
        "uniform sampler2D  tuv;\n"
        "uniform sampler2D  tvu;\n"
        "uniform float  alpha;\n"
        FRAGMENT_SHADER_OUT
        "void main() {\n"
        "float y, u , v, b, g, r;\n"
        "y = " FRAGMENT_SHADER_TEXTURE "(tyy, yTextureCoord.xy, 0.0).r;\n"
        "u = " FRAGMENT_SHADER_TEXTURE "(tuv, uvTextureCoord.xy, 0.0)." RTC_UVV ";\n"
        "v = " FRAGMENT_SHADER_TEXTURE "(tuv, uvTextureCoord.xy, 0.0).r;\n"
        "y = 1.164*y - 0.073;\n"
        "u = u - 0.5;\n"
        "v = v - 0.5;\n"
        "b = y + 2.018*u;\n"
        "r = y + 1.596*v;\n"
        "g = y - 0.813*v - 0.391*u;\n"
        //FRAGMENT_SHADER_COLOR " =  vec4(1.0, 0.0, 1.0, 1.0);\n"
        FRAGMENT_SHADER_COLOR " = vec4(r, g, b, alpha); \n"
        "}\n";


GLVideoObject::GLVideoObject() {

}

GLVideoObject::~GLVideoObject() {

}

void GLVideoObject::build() {

    glShape = new GLPlaneShape;

    mProgram = createProgram(g_RC_mVertexShader, g_RC_mFragmentShaderNV21);

    attrib_pos = glGetAttribLocation(mProgram, "aPosition");
    attrib_col = glGetAttribLocation(mProgram, "aColor");
    attrib_uv = glGetAttribLocation(mProgram, "aTextureCoord");

    uniform_project = glGetUniformLocation(mProgram, "uMVPMatrixP");
    uniform_view = glGetUniformLocation(mProgram, "uMVPMatrixW");
    uniform_model = glGetUniformLocation(mProgram, "uMVPMatrixM");

    uniform_tyy = glGetUniformLocation(mProgram, "tyy");
    uniform_tuv = glGetUniformLocation(mProgram, "tuv");
    uniform_tvu = glGetUniformLocation(mProgram,"tvu");

    uniform_fhRange = glGetUniformLocation(mProgram, "fHRange");
    uniform_alpha  = glGetUniformLocation(mProgram, "alpha");

    glUseProgram(mProgram);

    glUniform1i(uniform_tyy, 0);
    glUniform1i(uniform_tuv, 1);
    glUniform1i(uniform_tvu, 2);

    glUniform1f(uniform_alpha, 1.0f);

//    mViewMat = {
//            {1,    0,    0.0,     0.0},
//            {0,    1,    0.0,     0.0},
//            {0.0,  0.0,  -1,     0.000},
//            {0.0,  0.0,  -VIDEO_Z_POS, 1.0},
//    };

//    glUniformMatrix4fv(uniform_view, 1, GL_FALSE, (GLfloat*)&mViewMat);

    float ranges[3] = { 1.0, 1.0, 1.0 };
    glUniform3fv(uniform_fhRange, 1, ranges);

    for (int i = 0; i < 3; ++i){
        texids[i] = createTexture(RTC_PIXEL_FORMAT);
    }

//    float w = 720;
//    float h = 1280;
//    float nearVal = VIDEO_Z_POS;
//    float farVal = FAR_Z_POS;
//
//    float  left  = -w/2;
//    float  right = w/2;
//
//    float  top  =   h/2;
//    float  bottom = -h/2;
//
//    float A =  (right+left)/(right-left);
//    float B =(top+bottom)/(top-bottom);
//    float C= -(farVal+nearVal) /( farVal-nearVal );
//    float D= -2*(farVal*nearVal) /( farVal-nearVal );
//
//    // calculate the project matrix
//    mProjectMat = {
//            {2*nearVal/(right-left), 0,                     A,   0.0},
//            {0,                      2*nearVal/(top-bottom), B,  0.0},
//            {0.0,                    0.0,                    C,   D },
//            {0.0,                       0.0,                 -1.0, 0},
//    };
//    glUniformMatrix4fv(uniform_project, 1, GL_FALSE, (GLfloat*)&mProjectMat);

    mModelMat = glm::mat4(1.0f);
    glm::scale(mModelMat, glm::vec3(100.0f));
    glUniformMatrix4fv(uniform_model, 1, GL_FALSE, (GLfloat*)&mModelMat);

    glGenVertexArrays(1, &m_vertex_array_object);

    glBindVertexArray(m_vertex_array_object);

    glGenBuffers(1, &m_vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);

    glBufferData(GL_ARRAY_BUFFER, glShape->getVertexCount(), glShape->getVertexArray(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_index_buffer);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, glShape->getIndexCount(), glShape->getIndexArray(), GL_STATIC_DRAW);

    glBindVertexArray(0);

}

void GLVideoObject::clean() {
    GLObject::clean();
}

void GLVideoObject::draw(glm::mat4 projection, glm::mat4 view) {

    glUseProgram(mProgram);

    glUniformMatrix4fv(uniform_project, 1, GL_FALSE, (GLfloat*)&projection);
    glUniformMatrix4fv(uniform_view, 1, GL_FALSE, (GLfloat*)&view);
    glUniformMatrix4fv(uniform_model, 1, GL_FALSE, (GLfloat*)&mModelMat);

    for (int i = 0; i < 3; ++i){
        glActiveTexture((GLenum)(GL_TEXTURE0 + i));
        glBindTexture(RTC_PIXEL_FORMAT, texids[i]);
    }

    glBindVertexArray(m_vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vetex)*4, mFVertexBuffer, GL_STATIC_DRAW);

    if (attrib_pos >= 0){
        glEnableVertexAttribArray(attrib_pos);
        glVertexAttribPointer(attrib_pos, 3, GL_FLOAT,
                GL_FALSE, sizeof(gl_vetex), reinterpret_cast<void*>(offsetof(gl_vetex, x)));
    }

    if (attrib_col >= 0){
        glVertexAttribPointer(attrib_col, 3, GL_FLOAT,
                GL_FALSE, sizeof(gl_vetex), reinterpret_cast<void*>(offsetof(gl_vetex, nx)));
        glEnableVertexAttribArray(attrib_col);
    }

    if (attrib_uv >= 0){
        glVertexAttribPointer(attrib_uv, 2, GL_FLOAT,
                GL_FALSE, sizeof(gl_vetex),   reinterpret_cast<void*>(offsetof(gl_vetex, ux)));
        glEnableVertexAttribArray(attrib_uv);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void GLVideoObject::rotate(float degree, glm::vec3 direction) {
    GLObject::rotate(degree, direction);
}

void GLVideoObject::translate(glm::vec3 direction) {
    GLObject::translate(direction);
}

void GLVideoObject::scale(glm::vec3 direction) {
    GLObject::scale(direction);
}

void GLVideoObject::setCallback(GLObjectCallback *callback) {
    GLObject::setCallback(callback);
}

int GLVideoObject::getTexture() {
    return GLObject::getTexture();
}

void GLVideoObject::updateImage() {
    GLObject::updateImage();
}
