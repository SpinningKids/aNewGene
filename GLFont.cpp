
/************************************
  Code By: Pan/SpinningKids
  Revised on 2/2/2002 10:27:58 PM
  Comments: Class creation
  Portions by: Rio/SpinningKids, NeHe
 ************************************/

#include "GLFont.h"
#include <stdio.h>

GLFont::GLFont(HDC hDC, const char *s) {
  HFONT	font = CreateFont(	12,							// Height Of Font
		0,								// Width Of Font
		0,								// Angle Of Escapement
		0,								// Orientation Angle
		900,					// Font Weight
		FALSE,							// Italic
		FALSE,							// Underline
		FALSE,							// Strikeout
		ANSI_CHARSET,					// Character Set Identifier
		OUT_TT_PRECIS,					// Output Precision
		CLIP_DEFAULT_PRECIS,			// Clipping Precision
		ANTIALIASED_QUALITY,			// Output Quality
		FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
		s);				// Font Name

	SelectObject(hDC, font);

	base = glGenLists(96);

  wglUseFontOutlines(hDC, 32, 96, base,
		0.015f,							// Deviation From The True Outlines
		0.0f,							// Font Thickness In The Z Direction
		WGL_FONT_POLYGONS, gmf);

  DeleteObject(font);
}

void GLFont::printf(const char *fmt, ...) const {
	char text[256];
	va_list		ap;

	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

  unsigned int l = strlen(text);

 	float length=0;
//	for (unsigned int loop=0;loop<l;loop++)
//		length += gmf[text[loop]].gmfCellIncX;

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base-32);									// Sets The Base Character to 0
	glCallLists(l, GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
  glListBase(0);
	glPopAttrib();										// Pops The Display List Bits

}

void GLFont::printChar(char c, float lsx, float lsy, float lr, float tracking) const {
  c -= 32;

  glPushMatrix();

    glTranslatef(gmf[c].gmfCellIncX/2.0f, gmf[c].gmfBlackBoxY/2.0f, 0);
    glRotatef(lr, 0, 0, 1);
    glScalef(lsx, lsy, 1);
    glTranslatef(-gmf[c].gmfCellIncX/2.0f, -gmf[c].gmfBlackBoxY/2.0f, 0);

    glCallList(base+c);
  glPopMatrix();										// Pops The Display List Bits
  glTranslatef(gmf[c].gmfCellIncX*(tracking+1), 0, 0);
}

void GLFont::printfx(float x, float y, float gs, float gr, float lsx, float lsy, float lr, float tracking, const char *fmt, ...) const {
	char text[256];
	va_list		ap;

	if (fmt == NULL)
		return;
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);

  unsigned int l = strlen(text);

  glPushMatrix();
  glTranslatef(x, y, 0);
  glRotatef(gr, 0, 0, 1);
  glScalef(gs, -gs, 1);
  for(unsigned int i = 0; i < l; i++)
    printChar(text[i], lsx, lsy, lr, tracking);
    
  glPopMatrix();										// Pops The Display List Bits
}

GLFont::~GLFont() {
  glDeleteLists(base, 96);
}
