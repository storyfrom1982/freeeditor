//
// Created by yongge on 19-7-9.
//

#ifndef ANDROID_GLPLANESHAPE_H
#define ANDROID_GLPLANESHAPE_H


#include "../GLShape.h"


namespace freee {


    class GLPlaneShape : public GLShape {
    public:

        gl_vetex* getVertexArray() override;

        GLubyte* getIndexArray() override;

        GLuint getVertexCount() override;

        GLuint getIndexCount() override;
    };

}


#endif //ANDROID_GLPLANESHAPE_H
