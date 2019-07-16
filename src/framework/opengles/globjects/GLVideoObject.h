//
// Created by yongge on 19-7-9.
//

#ifndef ANDROID_YV21OBJECT_H
#define ANDROID_YV21OBJECT_H


#include "../GLObject.h"
#include "../GLShape.h"

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

        virtual int getTexture();

    protected:
        virtual void updateImage();
    };

}


#endif //ANDROID_YV21OBJECT_H
