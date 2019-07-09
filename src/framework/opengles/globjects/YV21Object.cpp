//
// Created by yongge on 19-7-9.
//

#include "YV21Object.h"


using namespace freee;

void YV21Object::build() {
    GLObject::build();
}

void YV21Object::clean() {
    GLObject::clean();
}

void YV21Object::draw(glm::mat4 projection, glm::mat4 view) {
    GLObject::draw(projection, view);
}

void YV21Object::rotate(float degree, glm::vec3 direction) {
    GLObject::rotate(degree, direction);
}

void YV21Object::translate(glm::vec3 direction) {
    GLObject::translate(direction);
}

void YV21Object::scale(glm::vec3 direction) {
    GLObject::scale(direction);
}

void YV21Object::setCallback(GLObjectCallback *callback) {
    GLObject::setCallback(callback);
}

void YV21Object::setAnimation() {
    GLObject::setAnimation();
}

void YV21Object::setImage() {
    GLObject::setImage();
}

void YV21Object::setScenesSize(glm::vec3 size) {
    GLObject::setScenesSize(size);
}

void YV21Object::setShader(std::string v, std::string f) {
    GLObject::setShader(v, f);
}

int YV21Object::getTexture() {
    return GLObject::getTexture();
}

int YV21Object::getObjectSize() {
    return GLObject::getObjectSize();
}

void YV21Object::updateImage() {
    GLObject::updateImage();
}
