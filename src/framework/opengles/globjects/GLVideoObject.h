//
// Created by yongge on 19-7-9.
//

#ifndef ANDROID_YV21OBJECT_H
#define ANDROID_YV21OBJECT_H


#include "../GLObject.h"

namespace freee {

    class GLVideoObject : public GLObject {
    public:

        GLVideoObject();
        ~GLVideoObject();

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
    };

}


#endif //ANDROID_YV21OBJECT_H
