
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/7/2002 12:40:02 PM
  Comments: Creation
 ************************************/

#ifndef GENTEX_H_
#define GENTEX_H_

#include "GLTexture.h"
#include <memory>

std::unique_ptr<GLTexture> circle(int logsize);
std::unique_ptr<GLTexture> circle2(int logsize);
std::unique_ptr<GLTexture> smoke(int logsize);

#endif // GENTEX_H_
