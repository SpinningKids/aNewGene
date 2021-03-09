
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/16/2002 10:11:37 AM
  Comments: creation
 ************************************/

#ifndef _GLTEXTURE_H_
#define _GLTEXTURE_H_

#ifdef _MSC_VER
#pragma once
#pragma pack(push, _PACK_GLTEXTURE_H_)
#endif //_MSC_VER

#include <windows.h>
#include <gl/gl.h>

class GLTexture {
  GLfloat *mem;
  int bpp, logsize, size;
  GLuint txt;
  bool init;
public:
  explicit GLTexture(int _logsize);

  ~GLTexture() {
    delete[] mem;
    glDeleteTextures(1,&txt);
  }

  GLfloat *getImage() { return mem; }
  int getSize() { return size; }

  void update();
  void use();
  int getID(){ return txt; }
};

#ifdef _MSC_VER
#pragma pack(pop, _PACK_GLTEXTURE_H_)
#endif //_MSC_VER

#endif //_GLTEXTURE_H_
