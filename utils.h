#ifndef UTILS_H_
#define UTILS_H_

#include "GLTexture.h"

/* Timing Functions */
void skInitTimer();
float skGetTime();
float skGetFPS();

/************************************************************/
/*            Other useless stuff                           */
/************************************************************/

void panViewOrtho();
void panViewOrthoModified();
void panViewPerspective();
void panViewPerspectiveFOV(float fov);
void skStopMusic();
bool skHiddenPart();
void skQuitDemo();
GLTexture* skLoadTexture(int resid, int logsize);

extern void skDraw();
extern void skInitDemoStuff();
extern void skUnloadDemoStuff();

#endif // UTILS_H_
