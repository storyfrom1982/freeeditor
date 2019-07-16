//
// Created by yongge on 19-7-9.
//

#include "GLPlaneShape.h"


using namespace freee;

#define        VERTS           4


static gl_vetex  planeVetex[4] =
        {
                {{-1,  1,  0},  {1.0,	1.0,	1},   {0,    0}},
                {{ 1,  1,  0},	{1.0,	1.0,	1},	 {1.0,	0}},
                {{-1, -1,  0},  {1.0,	1.0,	1},	 {0,	1.0}},
                {{ 1, -1,  0},  {1.0,	1.0,	1},	 {1.0,	1.0}}
        };

static const GLubyte nPlaneIndexes[6] = {
        0, 1, 2,  1, 3, 2
};


gl_vetex* GLPlaneShape::getVertexArray() {
    return &planeVetex[0];
}

GLubyte* GLPlaneShape::getIndexArray() {
    return (GLubyte*)&nPlaneIndexes[0];
}

GLuint GLPlaneShape::getVertexCount() {
    return sizeof(planeVetex) * 4;
}

GLuint GLPlaneShape::getIndexCount() {
    return sizeof(nPlaneIndexes);
}
