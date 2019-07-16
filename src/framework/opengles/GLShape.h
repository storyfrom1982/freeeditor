//
// Created by yongge on 19-7-9.
//

#ifndef ANDROID_GLSHAPE_H
#define ANDROID_GLSHAPE_H

#include "opengles.h"

namespace freee {

    struct  gl_vetex {
        struct { float   x,  y, z; };
        struct { float   nx, ny, nz; };
        struct { float   ux, uy; };
    };


    class GLShape {

    public:

        virtual gl_vetex* getVertexArray();
        virtual GLubyte* getIndexArray();
        virtual GLuint getVertexCount();
        virtual GLuint getIndexCount();
//        virtual GLsizei getStride();
//        virtual GLint getPositionVertexSize();
//        virtual GLint getColorVertexSize();
//        virtual GLint getTextureVertexSize();
//        virtual void* getPositionPointer();
//        virtual void* getColorPointer();
//        virtual void* getTexturePointer();

    };

}


#endif //ANDROID_GLSHAPE_H
