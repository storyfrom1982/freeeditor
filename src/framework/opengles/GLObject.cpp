//
// Created by yongge on 19-7-9.
//

#include "GLObject.h"


using namespace freee;



void GLObject::build() {

}

void GLObject::clean() {

}

void GLObject::draw(glm::mat4 projection, glm::mat4 view) {

}

void GLObject::rotate(float degree, glm::vec3 direction) {

}

void GLObject::translate(glm::vec3 direction) {

}

void GLObject::scale(glm::vec3 direction) {

}

void GLObject::setCallback(GLObjectCallback *callback) {

}

void GLObject::setAnimation() {

}

void GLObject::setImage() {

}

void GLObject::setScenesSize(glm::vec3 size) {

}

void GLObject::setShader(std::string v, std::string f) {

}

int GLObject::getTexture() {
    return 0;
}

int GLObject::getObjectSize() {
    return 0;
}

void GLObject::updateImage() {

}

GLuint GLObject::createProgram(const char *vertex_shader, const char *fragment_shader) {
    GLint status;
    GLuint prog = glCreateProgram();
    GLuint vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(vert_shdr);
    glCompileShader(frag_shdr);
    glGetShaderiv(vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(prog, vert_shdr);
    glAttachShader(prog, frag_shdr);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);
    return prog;
}

GLuint GLObject::createTexture(GLenum pixelFormat) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(pixelFormat, id);
    glTexParameteri(pixelFormat, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(pixelFormat, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(pixelFormat, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(pixelFormat, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return id;
}
