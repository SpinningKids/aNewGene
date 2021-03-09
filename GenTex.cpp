
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/7/2002 12:40:36 PM
  Comments: Creation
 ************************************/

#include "GenTex.h"
#include "noise.h"
#include <math.h>

GLTexture *perlin(int logsize, float freq, float amp, float base, float k, bool wrap) {
  GLTexture *prln = new GLTexture(logsize);
  int size = prln->getSize();
  GLfloat *tex = prln->getImage();
  int fillsize = size;
  if (wrap) {
    fillsize >>= 1;
  }
  float m = freq/fillsize;
  float ncoord[3];
  for(int i = 0; i < fillsize; i++) {
    ncoord[0] = m*i;
    for(int j = 0; j < fillsize; j++) {
      ncoord[1] = m*j;
      float col = vnoise(2, ncoord)*amp+base;
      col = (col < 0) ? 0: (col > 1) ? 1: col;
      tex[(((i<<logsize)+j)<<2)+0] = powf(col, 1.f/k);
      tex[(((i<<logsize)+j)<<2)+1] = col;
      tex[(((i<<logsize)+j)<<2)+2] = powf(col, k);
      tex[(((i<<logsize)+j)<<2)+3] = 1;
    }
  }
  if (wrap)
    for(int i = 0; i < fillsize; i++)
      for(int j = 0; j < fillsize; j++)
        for(int k = 0; k < 4; k++)
          tex[((((size-1-i)<<logsize)+j)<<2)+k] = tex[(((i<<logsize)+size-1-j)<<2)+k] = tex[((((size-1-i)<<logsize)+size-1-j)<<2)+k] = tex[(((i<<logsize)+j)<<2)+k];
  prln->update();
  return prln;
}

GLTexture *circle(int logsize) {
  GLTexture *prln = new GLTexture(logsize);
  int size = prln->getSize();
  GLfloat *tex = prln->getImage();
  float m = 2.0f/(size-1);
  for(int i = 0; i < size; i++) {
    float x = m*i-1.f;
    for(int j = 0; j < size; j++) {
      float y = m*j-1.f;
      float col;
      float r = x*x+y*y;
      float kr = 2*sqrt(x*x+y*y)-1.f;
      if (r>1)
        col = 0;
      else
        col = (1-r)*(1-r);//abs(kr);
//      col = 0.3*(2+0.5*(vnoise(5*r, 10*x, 10*y)+vnoise(10*r, 20*x, 20*y)+vnoise(20*r, 40*x, 40*y)+vnoise(40*r, 80*x, 80*y)))*(1-r);
      tex[(((i<<logsize)+j)<<2)+0] = 1-col*0.25;
      tex[(((i<<logsize)+j)<<2)+1] = 1-col*0.25;
      tex[(((i<<logsize)+j)<<2)+2] = 1-col*0.25;
      tex[(((i<<logsize)+j)<<2)+3] = col;
    }
  }
  prln->update();
  return prln;
}

GLTexture *smoke(int logsize) {
  GLTexture *prln = new GLTexture(logsize);
  int size = prln->getSize();
  GLfloat *tex = prln->getImage();
  float m = 2.0f/(size-1);
  for(int i = 0; i < size; i++) {
    float x = m*i-1.f;
    for(int j = 0; j < size; j++) {
      float y = m*j-1.f;
      float col;
      float r = x*x+y*y;
      float kr = 2*sqrt(x*x+y*y)-1.f;
//      if (r>1)
//        col = 0;
//      else
//        col = 1-r;//abs(kr);
      col = 0.3*(2+0.5*(vnoise(5*r, 10*x, 10*y)+vnoise(10*r, 20*x, 20*y)+vnoise(20*r, 40*x, 40*y)+vnoise(40*r, 80*x, 80*y)))*(1-r);
      tex[(((i<<logsize)+j)<<2)+0] = 1;
      tex[(((i<<logsize)+j)<<2)+1] = 1;
      tex[(((i<<logsize)+j)<<2)+2] = 1;
      tex[(((i<<logsize)+j)<<2)+3] = col;
    }
  }
  prln->update();
  return prln;
}

GLTexture *circle2(int logsize) {
  GLTexture *prln = new GLTexture(logsize);
  int size = prln->getSize();
  GLfloat *tex = prln->getImage();
  float m = 2.0f/(size-1);
  for(int i = 0; i < size; i++) {
    float x = m*i-1.f;
    for(int j = 0; j < size; j++) {
      float y = m*j-1.f;
      float col;
      float r = x*x+y*y;
      float kr = 2*sqrt(x*x+y*y)-1.f;
      if (r>1)
        col = 0;
      else {
        float tmp = 1;
        if (r > 0.95) tmp = (1-r)/0.05;
        col = tmp*(1-0.5*(1-r)*(1-r)) ;//abs(kr);
      }
//      col = 0.3*(2+0.5*(vnoise(5*r, 10*x, 10*y)+vnoise(10*r, 20*x, 20*y)+vnoise(20*r, 40*x, 40*y)+vnoise(40*r, 80*x, 80*y)))*(1-r);
      tex[(((i<<logsize)+j)<<2)+0] = 1;
      tex[(((i<<logsize)+j)<<2)+1] = 1;
      tex[(((i<<logsize)+j)<<2)+2] = 1;
      tex[(((i<<logsize)+j)<<2)+3] = col;
    }
  }
  prln->update();
  return prln;
}

