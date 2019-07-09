//
// Created by yongge on 19-7-9.
//

#include "GLVideoObject.h"


using namespace freee;

GLVideoObject::GLVideoObject() {

}

GLVideoObject::~GLVideoObject() {

}

void GLVideoObject::build() {
    GLObject::build();
}

void GLVideoObject::clean() {
    GLObject::clean();
}

void GLVideoObject::draw(glm::mat4 projection, glm::mat4 view) {
    GLObject::draw(projection, view);
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

void GLVideoObject::setAnimation() {
    GLObject::setAnimation();
}

void GLVideoObject::setImage() {
    GLObject::setImage();
}

void GLVideoObject::setScenesSize(glm::vec3 size) {
    GLObject::setScenesSize(size);
}

void GLVideoObject::setShader(std::string v, std::string f) {
    GLObject::setShader(v, f);
}

int GLVideoObject::getTexture() {
    return GLObject::getTexture();
}

int GLVideoObject::getObjectSize() {
    return GLObject::getObjectSize();
}

void GLVideoObject::updateImage() {
    GLObject::updateImage();
}
