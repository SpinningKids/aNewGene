
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/16/2002 10:11:50 AM
  Comments: creation
 ************************************/

#include "GLTexture.h"
#include <memory.h>
#ifdef WIN32
#include <Windows.h>
#endif
#include <gl/GL.h>

GLTexture::GLTexture(int logsize) : mem_(new GLfloat[4 * (1 << logsize) * (1 << logsize)]), size_(1 << logsize), init_(false) {
    glGenTextures(1, &txt_);
}

GLTexture::~GLTexture() {
    delete[] mem_;
    glDeleteTextures(1, &txt_);
}

void GLTexture::update() {
    glBindTexture(GL_TEXTURE_2D, txt_);
    if (!init_) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_, size_, 0, GL_RGBA, GL_FLOAT, mem_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        init_ = true;
    } else {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size_, size_, GL_RGBA, GL_FLOAT, mem_);
    }
}

void GLTexture::use() const {
    if (!init_) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glBindTexture(GL_TEXTURE_2D, txt_);
        glEnable(GL_TEXTURE_2D);
    }
}
