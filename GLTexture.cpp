
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/16/2002 10:11:50 AM
  Comments: creation
 ************************************/

#include "GLTexture.h"
#include <memory.h>

GLTexture::GLTexture(int _logsize) : logsize(_logsize), size(1<<_logsize), mem(new GLfloat[4*(1<<_logsize)*(1<<_logsize)]) {
  glGenTextures(1, &txt);
//  memset(mem, 0, size*size*4*sizeof(float));
//  glBindTexture(GL_TEXTURE_2D, txt);
  init = false;
}

void GLTexture::update() {
  glBindTexture(GL_TEXTURE_2D, txt);
  if (!init) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size, size, 0, GL_RGBA, GL_FLOAT, mem);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    init = true;
  } else
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size, size, GL_RGBA, GL_FLOAT, mem);
}

void GLTexture::use() {
  if (!init)
    glDisable(GL_TEXTURE_2D);
  else {
    glBindTexture(GL_TEXTURE_2D, txt);
    glEnable(GL_TEXTURE_2D);
  }
}
