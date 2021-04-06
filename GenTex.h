
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/7/2002 12:40:02 PM
  Comments: Creation
 ************************************/

#ifndef GENTEX_H_
#define GENTEX_H_

#include "GLTexture.h"

GLTexture* perlin(int logsize, float freq, float amp, float base, float k, bool wrap);
GLTexture *circle(int logsize);
GLTexture *circle2(int logsize);
GLTexture *smoke(int logsize);

#endif // GENTEX_H_
