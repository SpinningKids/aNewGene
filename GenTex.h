
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/7/2002 12:40:02 PM
  Comments: Creation
 ************************************/

#ifndef _GENTEX_H_
#define _GENTEX_H_

#ifdef _MSC_VER
#pragma once
#pragma pack(push, _PACK_GENTEX_H_)
#endif //_MSC_VER

#include "GLTexture.h"

GLTexture *perlin(int logsize, float freq, float amp, float base, float k, bool wrap);
GLTexture *circle(int logsize);
GLTexture *circle2(int logsize);
GLTexture *smoke(int logsize);

#ifdef _MSC_VER
#pragma pack(pop, _PACK_GENTEX_H_)
#endif //_MSC_VER

#endif //_GENTEX_H_
