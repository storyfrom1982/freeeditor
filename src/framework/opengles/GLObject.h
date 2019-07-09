//
// Created by yongge on 19-7-9.
//

#ifndef ANDROID_GLOBJECT_H
#define ANDROID_GLOBJECT_H

#include <string>

#ifdef __ANDROID__
# ifdef __cplusplus
extern "C" {
# endif
#  include <GLES3/gl3.h>
#  include <GLES3/gl3ext.h>
# ifdef __cplusplus
}
# endif
#endif


#include <glm.hpp>


namespace freee {

    class GLObject;

    class GLObjectCallback {
    public:
        virtual void onImageMakeReady() = 0;
        virtual void onObjectMakeReady(GLObject *obj) = 0;
    };


    class GLObject {

    public:

        virtual void build();

        virtual void clean();

        virtual void draw(glm::mat4 projection, glm::mat4 view);

        virtual void rotate(float degree, glm::vec3 direction);

        virtual void translate(glm::vec3 direction);

        virtual void scale(glm::vec3 direction);

        virtual void setCallback(GLObjectCallback *callback);

        virtual void setAnimation();

        virtual void setImage();

        virtual void setScenesSize(glm::vec3 size);

        virtual void setShader(std::string v, std::string f);

        virtual int getTexture();

        virtual int getObjectSize();

    protected:

        virtual void updateImage();


    protected:

        glm::mat4 mModelMat;

    };

}


#endif //ANDROID_GLOBJECT_H
