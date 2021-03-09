
// bilaterian metazoans
// Eukariotes

/************************************
  Code By: Pan/SpinningKids
  Revised on 5/5/2002 11:03:36 PM
  Comments: Creation of a demiurg
  Portions by: Rio/SpinningKids
 ************************************/

#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#define WINDOW_CLASS_NAME "SKCLASS"
#define WINDOW_TITLE "CoolPrint sample"

#include <Windows.h>
#include <mmsystem.h>
#include <GL/GLU.h>
#include <math.h>

#ifdef USETEXT
#include "GLFont.h"
#endif

#include "gentex.h"
#include "noise.h"
#include "minifmod/minifmod.h"
#include "resource.h"
#include "Particles.h"

#define WIDTH 2560
#define HEIGHT 1440
#define BITSPERPIXEL 32
#define FULLSCREEN
#define ORIGINAL_WIDTH 640
#define ORIGINAL_HEIGHT 480

#define LOGO_WIDTH (WIDTH*256/1280)
#define LOGO_HEIGHT (WIDTH*63/1280)

const double piover180 = 0.0174532925199432957692369076848861;

bool greets = false;

//globals

HINSTANCE g_hinstance;

#ifdef USETEXT
GLFont *fontarial;
#endif

Particles *fumo1;
Particles *fumo2;
Particles *fumo3;
Particles *fiori1;

GLTexture *smoke1;
GLTexture *pallino1;
GLTexture *pallino2;
GLTexture *greetscreds;
float landscape[100][100];

GLint mylist;
GLint mylandscape;
GLint mylandscape2;

// Music Related

bool music;
FMUSIC_MODULE *fmodule;

typedef struct {
	int length;
	int pos;
	void *data;
} MEMFILE;

unsigned int memopen(char *name)
{
	MEMFILE *memfile;

	memfile = new MEMFILE;

	{	// hey look some load from resource code!
		HRSRC		rec;
		HGLOBAL		handle;

		rec = FindResourceEx(GetModuleHandle(0),"RC_RTDATA", name, 0);
		handle = LoadResource(NULL, rec);
		memfile->data = LockResource(handle);
		memfile->length = SizeofResource(NULL, rec);
		memfile->pos = 0;
	}

	return (unsigned int)memfile;
}

void memclose(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;
	delete memfile;
}

int memread(void *buffer, int size, unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (memfile->pos + size >= memfile->length)
		size = memfile->length - memfile->pos;

	memcpy(buffer, (char *)memfile->data+memfile->pos, size);
	memfile->pos += size;
	
	return size;
}

void memseek(unsigned int handle, int pos, signed char mode)
{
	MEMFILE *memfile = (MEMFILE *)handle;

	if (mode == SEEK_SET) 
		memfile->pos = pos;
	else if (mode == SEEK_CUR) 
		memfile->pos += pos;
	else if (mode == SEEK_END)
		memfile->pos = memfile->length + pos;

	if (memfile->pos > memfile->length)
		memfile->pos = memfile->length;
}

int memtell(unsigned int handle)
{
	MEMFILE *memfile = (MEMFILE *)handle;
	return memfile->pos;
}

// Base GL Related

HWND		hWND;
HDC			hDC;
HGLRC		hRC;

static void panViewOrthoModified() {
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    int w = ORIGINAL_HEIGHT * WIDTH / HEIGHT;
    if (w >= ORIGINAL_WIDTH) {
        int margin = (w - ORIGINAL_WIDTH) / 2;
        glOrtho(-margin, ORIGINAL_WIDTH + margin, ORIGINAL_HEIGHT, 0, -1, 1);
    } else {
        int h = ORIGINAL_WIDTH * HEIGHT / WIDTH;
        int margin = (h - ORIGINAL_HEIGHT) / 2;
        glOrtho(0, WIDTH, ORIGINAL_HEIGHT + margin, -margin, -1, 1);
    }
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void panViewOrtho() {
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void panViewPerspectiveFOV(float fov)
{
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,((float)WIDTH)/HEIGHT,0.1f,600.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void skSwapBuffers() {
  glFinish();
  glFlush();
  SwapBuffers(hDC);
}

// textureload

GLTexture *loadTexture(int resid, int logsize) {
  static HDC displaydc = CreateDC("DISPLAY", NULL, NULL, NULL);
  GLTexture *ret = new GLTexture(logsize);
  int txsize = ret->getSize();
  HBITMAP h = (HBITMAP)LoadImage(g_hinstance, (const char *)resid, IMAGE_BITMAP, txsize, txsize, LR_CREATEDIBSECTION);
  DIBSECTION ds;
  GetObject(h, sizeof(ds), &ds);
  BITMAPINFOHEADER a = {
    sizeof(BITMAPINFOHEADER),
    txsize,
    -txsize,
    1,
    32,
    BI_RGB,
    4*txsize*txsize,
    0,
    0,
    0,
    0
  };
  unsigned char *texture = new unsigned char[a.biSizeImage];
  GetDIBits(displaydc, h, 0, txsize, texture, (BITMAPINFO *)&a, DIB_RGB_COLORS);
  GLfloat *tex = ret->getImage();
  const float un255 = 1.0f/255.0f;
  int i;
  for(i = 0; i < txsize*192; i++) {
    tex[i*4] = tex[i*4+1] = tex[i*4+2] = texture[i*4]*un255;
    tex[i*4+3] = 1.0f;
  }
  for(; i < txsize*txsize; i++) {
    tex[i*4] = tex[i*4+1] = tex[i*4+2] = 1.0f;
    tex[i*4+3] = 1.0f-texture[i*4]*un255;
  }
  ret->update();
//    free((void *)a);
  DeleteObject(h);
  return ret;
}

// timing

static __int64 timerstart;
static __int64 timerfrq;

extern "C" {
  HWAVEOUT FSOUND_WaveOutHandle;
}

static void panInitTimer() {
	QueryPerformanceCounter((LARGE_INTEGER *)&timerstart);
	QueryPerformanceFrequency((LARGE_INTEGER *)&timerfrq);
}

static double panGetTime() {
  if (music)
  {
    MMTIME mmtime;
    mmtime.wType = TIME_SAMPLES;
    waveOutGetPosition(FSOUND_WaveOutHandle, &mmtime, sizeof(mmtime));
    return mmtime.u.ticks/44100.f;
  }
  else {
    __int64 a;
	  QueryPerformanceCounter((LARGE_INTEGER *)&a);
	  return (double)(a - timerstart)/(double)(timerfrq);
  }
}

// windows

static LRESULT CALLBACK skWinProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch(msg) {
    case WM_KEYDOWN: 
      if ((int)wp != VK_ESCAPE)
        break;
    case WM_CLOSE: 
    case WM_DESTROY: 
      PostQuitMessage(0);
		  return 0;
  } 
  return DefWindowProc(wnd, msg, wp, lp);
} 

// Particles Stuff

struct FixedField : Field {
  Vector3 value;
  FixedField(const Vector3 &_value) : value(_value) {}

  virtual Vector3 operator () (Particles &p, int i) { return value; }
};

struct NoiseField : Field {
  float scale, amplitude;
  Vector3 carrier;

  NoiseField(float _scale, float _amplitude, const Vector3 &_carrier = zero3) : scale(_scale), amplitude(_amplitude), carrier(_carrier) {}
  
  virtual Vector3 operator () (Particles &p, int i) { 
    Vector3 a = scale*p.parts[i].position;
    return Vector3(carrier.x+amplitude*vnoise(a.y, a.z), carrier.y+amplitude*vnoise(a.z, a.x), carrier.z+amplitude*vnoise(a.x, a.y));
  }
};

#define quattroterzipigreco 4.1887902047863909846168578443727

struct ArchimedesField : Field {
  float density;
  ArchimedesField(float _density) : density(_density) {}
  virtual Vector3 operator () (Particles &p, int i) { 
    float r = p.parts[i].size;
    return Vector3(0, density-p.parts[i].mass/(quattroterzipigreco*r*r*r), 0);
  }
};

struct ZSpringField : Field {
  float k;
  ZSpringField(float _k) : k(_k) {}
  virtual Vector3 operator () (Particles &p, int i) { 
//    float r = sqrt(sqr(p.parts[i].position.x)+sqr(p.parts[i].position.y));
    return -k*Vector3(p.parts[i].position.x, p.parts[i].position.y, 0);
  }
};

//effects

static void algheTraICapelli(float t) {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  panViewOrthoModified();
  const int cosi = 120;
  const int steps = 20;
  int i, j;
  for(i = 0; i < cosi; i++) {
    float phase = i*6.28/cosi + 0.1*vnoise(i, t);
    float rho = 300-t*30+20*vnoise(t, i)+150*vnoise(i);
    glBegin(GL_QUAD_STRIP);
    for(j = 0; j <= steps; j++) {
      float ph = phase+0.1*vnoise(i, 0.1*j, t);
      float sz = 0.2*(j+1);
      float rh = rho+10*(j+1)+vnoise(0.1*j, i, t);
      float xb = rh*cos(ph)+320;
      float yb = rh*sin(ph)+240;
      glColor4f(0.8f, 0.1*vlattice(1, i)+0.05, 0.1*vlattice(2, i)+0.05, 0.01*(steps-j));
      float sph = sz*sin(ph);
      float cph = sz*cos(ph);
      glVertex2f(xb-sph, yb+cph);
      glVertex2f(xb+sph, yb-cph);
    }
    glEnd();
  }
  glDisable(GL_BLEND);
}

void getStupidVectors(Vector3 & ur, Vector3 & ul) {
  float model[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, model);
  ur.x = model[0]+model[1];
  ur.y = model[4]+model[5];
  ur.z = model[8]+model[9];
  ul.x = model[1]-model[0];
  ul.y = model[5]-model[4]; 
  ul.z = model[9]-model[8];
}

void drawParticles(Particles &ps, GLTexture *tex) {

  Vector3 ur, ul;

  getStupidVectors(ur, ul);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (tex) {
    glEnable(GL_TEXTURE_2D);
    tex->use();
  } else
    glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

  glBegin(GL_QUADS);
  for(int i = 0; i < ps.num ; i++) {

    Vector3 &p = ps.parts[i].position;

    float s = ps.parts[i].size;

    glColor4f(ps.parts[i].r, ps.parts[i].g, ps.parts[i].b, ps.parts[i].a);

    Vector3 sur(ur);
    Vector3 sul(ul);

    sur *= s;
    sul *= s;
  
    glTexCoord2f(0, 0);
    glVertex3fv((float *)&(p-sur));
    glTexCoord2f(1, 0);
    glVertex3fv((float *)&(p-sul));
    glTexCoord2f(1, 1);
    glVertex3fv((float *)&(p+sur));
    glTexCoord2f(0, 1);
    glVertex3fv((float *)&(p+sul));
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void drawSmoke(float t) {
  static float ot = t;

  float dt = t - ot;
  if (dt != 0)
    fumo1->move(dt);

  Vector3 centre(0,0,0);
  int i;
  for(i = 0; i < fumo1->num; i++)
    centre += fumo1->parts[i].position;

  if (fumo1->num > 0)
    centre /= fumo1->num;

  float fov = 90;
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,((float)WIDTH)/HEIGHT,0.1f,2000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  gluLookAt(150, 300, 150, centre.x, centre.y, centre.z, 0, 1, 0);

  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  float a[3]= {1,1,1};
  glFogfv(GL_FOG_COLOR, a);
  glFogf(GL_FOG_START, 300);
  glFogf(GL_FOG_END, 800);


  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);

  glBegin(GL_QUADS);

  glColor4f(0.7, 0.75, 0.8, 1);
  
  glVertex3f(-50*30-900, -100, -50*30-600);
  glVertex3f(50*30-900, -100, -50*30-600);
  glVertex3f(50*30-900, -100, 50*30-600);
  glVertex3f(-50*30-900, -100, 50*30-600);

//  glColor4f(0.7, 0.75, 0.8, 1);
  
  glVertex3f(-50*30-900, 600, -50*30-600);
  glVertex3f(50*30-900, 600, -50*30-600);
  glVertex3f(50*30-900, 600, 50*30-600);
  glVertex3f(-50*30-900, 600, 50*30-600);

  glEnd();

  glCallList(mylandscape);

  glDisable(GL_FOG);

  drawParticles(*fumo1, smoke1);

  ot = t;

#ifdef USETEXT
  panViewOrtho();
  glTranslatef(20, 70, 0);
  glScalef(20, -20, 1);
  glColor4f(0,0,0,1);
  fontarial->printf("%d", fumo1->num);
#endif
};

void drawSmoke2(float t, int v) {
  static float ot = t;

  float dt = t - ot;
  if (dt != 0) {
    fumo2->move(dt);
    fiori1->move(dt);
  }

  int i;
  for(i = 0; i < fumo2->num; i++)
    if (fumo2->parts[i].position.z > 900) {
        fumo2->remove(i);
        i--;
    }
/*      
  for(i = 0; i < fiori1->num; i++) {
//    if (fiori1->parts[i].position.z > 750) 
//      fiori1->parts[i].position.z -= 1500;
    float r = 0.7+vlattice(i, 6)*0.07;
    float g = 0.515+vlattice(i, 7)*0.0515;
    float b = 0.67+vlattice(i, 8)*0.067;
    float a = 0.5f;
    if (t-fiori1->parts[i].timestamp < 0.05) {
      float l = (t-fiori1->parts[i].timestamp)/0.05;
      fiori1->parts[i].r = 1-l+r*l;
      fiori1->parts[i].g = 1-l+g*l;
      fiori1->parts[i].b = 1-l+b*l;
      fiori1->parts[i].a = a*l; 
    } else {
      fiori1->parts[i].r = r; //1+(r-1)/2;
      fiori1->parts[i].g = g; //1+(g-1)/2;
      fiori1->parts[i].b = b; //1+(b-1)/2;
      fiori1->parts[i].a = a;
    }
  }
*/
  Vector3 centre(0,0,0);
/*
  for(i = 0; i < fumo2->num; i++)
    centre += fumo2->parts[i].position;
 
  centre /= fumo2->num;
*/
  float fov = 90;

  panViewOrtho();

  glBegin(GL_QUADS);
  glColor4f(1,1,1,1);
  glVertex2f(0, HEIGHT/2);
  glVertex2f(WIDTH, HEIGHT/2);
  glColor4f(0.85, 0.875, 0.9, 1);
  glVertex2f(WIDTH, 0);
  glVertex2f(0, 0);
  glEnd();


	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,((float)WIDTH)/HEIGHT,0.1f,2000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  gluLookAt(centre.x+50*sin(t*1)+((v)?(150):(-150)), centre.y+50*cos(t*1)+150, centre.z+100, centre.x, centre.y, centre.z, 0, 1, 0);

  drawParticles(*fumo2, smoke1);
  glTranslatef(0, 50, 0);
  drawParticles(*fiori1, pallino1);

  ot = t;

#ifdef USETEXT
  panViewOrtho();

  glColor4f(0,0,0,1);
  glTranslatef(20, 70, 0);
  glScalef(20, -20, 1);
  glColor4f(0,0,0,1);
  fontarial->printf("%d", fumo2->num);
#endif
};


static void elefantiTraICapelli3d(float t, int steps, float tphx, float tphz, int view) {

  float model[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, model);
  Vector3 rt(model[0], model[4], model[8]);
  Vector3 up(model[1], model[5], model[9]);
  Vector3 pf(model[2], model[6], model[10]);

  rt.normalize();
  up.normalize();
  pf.normalize();

  glEnable(GL_DEPTH_TEST);
/*
  glBegin(GL_QUADS);
  glVertex3f(100, 0, 100);
  glVertex3f(-100, 0, 100);
  glVertex3f(-100, 0, -100);
  glVertex3f(100, 0, -100);
  glEnd();
*/
//  glDisable(GL_DEPTH_TEST);
//  glDepthMask(false);
  


  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// panViewOrtho();

  const int cosi = 30;
  const float thick = 20;
  const float k = 200/steps;

  int i, j;


  for(i = 0; i < cosi; i++) {
    
    float s = 0.5*thick/steps; //(i*thick/cosi)/steps;
    
    Vector3 p = pf*(-3.0*i/cosi);
    float phx = tphx;
    float phz = tphz;
    glBegin(GL_QUAD_STRIP);
    glColor4f(0.3f, 0.3f, 0.0f, 0.000);

    Vector3 delta = (thick/2*up.y)*rt-(thick/2*rt.y)*up;

    glVertex3fv((float *)&(p+delta));
    glVertex3fv((float *)&(p-delta));

    float cr = vlattice(1, i)*0.2f;
    float cg = vlattice(2, i)*0.2f;
    float cb = vlattice(3, i)*0.2f;

    float rot = vlattice(4, i);

    for(j = 0; j <= steps; j++) {
//      float rotx = vnoise(0.1f*i+ rot*1.2f*j/steps+t*0.15+ t*vlattice(i, 0)*0.05f)*j*j*10.0f/(steps*steps)+0.1f*vnoise((60.f*j)/steps)+0.001f*vlattice(1, i);
//      float rotz = vnoise(0.1f*i+ rot*1.2f*j/steps+t*0.15+ t*vlattice(i, 1)*0.05f)*j*j*10.0f/(steps*steps)+0.1f*vnoise((60.f*j)/steps)+0.001f*vlattice(2, i);

      float rotx = vnoise(0.1f*i, rot*1.2f*j/steps+t*0.15+ t*vlattice(i)*0.05f)*j*j*10.0f/(steps*steps)+0.1f*vnoise((60.f*j)/steps)+0.001f*vlattice(i)+0.5*vnoise(t*0.3-j*0.1, 1*((float)i)/cosi);
      float rotz = vnoise(0.1f*i+10, rot*1.2f*j/steps+t*0.15+ t*vlattice(i)*0.05f)*j*j*10.0f/(steps*steps)+0.1f*vnoise((60.f*j)/steps)+0.001f*vlattice(i)+0.5*vnoise(t*0.3+3-j*0.1, 1*((float)i)/cosi);

//      float rotx = 2*vnoise(t*0.2-j*0.1, 20*((float)i)/cosi);
//      float rotz = 2*vnoise(t*0.2+3-j*0.1, 20*((float)i)/cosi);
//      rot = 0.04*(rot-0.5)*(1.0*steps-j)/steps;
      rotx = 0.07*rotx;
      rotz = 0.07*rotz;
      phx = phx+rotx;
      phz = phz+rotz;
//      float k1 = k+vnoise(i, j, t)*0.01;
      float sz = s*(steps-j);

      delta.x = sin(phx);
      delta.y = cos(phx)*cos(phz);
      delta.z = sin(phz);

      p += k*delta;

// new delta

      delta = (sz*(delta*up))*rt-(sz*(delta*rt))*up;

      float alpha = 0.005*(j+10)+0.25;
      float alpha2 = (j<10)?0.666:(j<15)?(j/15.0f):1;


      switch (view) {
      case 0: glColor4f(
          (cr+0.3f-j*0.001f)*alpha+1-alpha,
          (cg+0.3f)*alpha+1-alpha,
          (0.0025*(j+10))*alpha+1-alpha,
          alpha2
        );
        break;
      case 1: glColor4f(
          (cr+0.5f-j*0.001f)*alpha+1-alpha,
          (cg+0.2f)*alpha+1-alpha,
          (0.0025*(j+10))*alpha+1-alpha,
          alpha2
        );
        break;
      case 2: glColor4f(
          (j*0.001f)*alpha+1-alpha,
          (cg+0.25f)*alpha+1-alpha,
          (cb+0.525f+ 0.0025*j)*alpha+1-alpha,
          alpha2
        );
        break;
      case 3: glColor4f(
          (cr+0.3f-j*0.001f)*alpha+1-alpha,
          (cg+0.2f)*alpha+1-alpha,
          (cb+0.425f+ 0.0025*j)*alpha+1-alpha,
          alpha2
        );
        break;
      }

//      glColor4f(cr*0.2f+0.3f-j*0.001f, cg*0.2f+0.3f, cb*0.2f+0.0f, 0.0025*(j+10));
      
//      glColor4f(cr*0.1f+0.9f-j*0.001f, cg*0.1f+0.9f, cb*0.1f+0.7f, 0.0025*(j+10));

      glVertex3fv((float *)&(p+delta));
      glVertex3fv((float *)&(p-delta));
    }
    glEnd();

  }
  glDisable(GL_BLEND);
//  glDepthMask(true);
}

void drawLandscapeTrees(float t, int view) {
  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  float a[3]= {1,1,1};
  glFogfv(GL_FOG_COLOR, a);
  glFogf(GL_FOG_START, 600);
  glFogf(GL_FOG_END, 1500);

  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  
  glCallList(mylandscape2);
  int i;
  for(i = 0; i < 2; i++) {
    glNewList(mylist+i, GL_COMPILE);
    if (view == 4)
      elefantiTraICapelli3d(t+i*100, 30, 0, 0, (2+i) & 3);
    else
      elefantiTraICapelli3d(t+i*100, 30, 0, 0, view & 3);
    glEndList();
  }

  for(i = 10; i < 30; i+=2) {
    for(int j = 10; j < 30; j+=2) {
      glPushMatrix();
      glTranslatef((i+0.5)*100-2000, (landscape[i][j]+landscape[i+1][j]+landscape[i+1][j+1]+landscape[i][j+1]), (j+0.5)*100-2000);
      glCallList(mylist+(((i+j)/2)&1));
      glPopMatrix();
    }
  }
}

void scenaElefantiSferici(float t, int view) {
  float fov = 60;
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,((float)WIDTH)/HEIGHT,0.1f,1500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  gluLookAt(100*cos(-0.3*t), 0, 100*sin(-0.3*t+view), 0, 0, 0, 0, 1, 0);

  Vector3 ur, ul;

  getStupidVectors(ur, ul);

  int i;
  Vector3 pos[10];
  float size[10];
  for(i = 0; i < 10; i++) {
    pos[i] = 50*Vector3(vnoise(t*0.1, 1, i), vnoise(t*0.1, 2, i), vnoise(t*0.1, 3, i));
    size[i] = vnoise(i)*10+20;
    glPushMatrix();

    glTranslatef(pos[i].x, pos[i].y, pos[i].z);
    glRotatef(360*vnoise(t*0.1, 5, i), 1, 0, 0);
    glRotatef(360*vnoise(t*0.1, 6, i), 0, 1, 0);
    glScalef(size[i]/150.0f, size[i]/150.0f, size[i]/150.0f);
    glTranslatef(0, -140, 0);

    elefantiTraICapelli3d(t, 10, 0, 0, view);

    glPopMatrix();
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//  glDepthMask(false);
  pallino2->use();
  glDisable(GL_DEPTH_TEST);
  glBegin(GL_QUADS);
  for(i = 0; i < 10; i++) {
    glColor4f(0.7+vlattice((i), 6)*0.07, 0.515+vlattice((i), 7)*0.0515, 0.67+vlattice((i), 8)*0.067, 0.5f);

    Vector3 sur(ur);
    Vector3 sul(ul);

    sur *= size[i];
    sul *= size[i];
  
    glTexCoord2f(0, 0);
    glVertex3fv((float *)&(pos[i]-sur));
    glTexCoord2f(1, 0);
    glVertex3fv((float *)&(pos[i]-sul));
    glTexCoord2f(1, 1);
    glVertex3fv((float *)&(pos[i]+sur));
    glTexCoord2f(0, 1);
    glVertex3fv((float *)&(pos[i]+sul));
   
  }
//  glEnable(GL_DEPTH_TEST);
  glEnd();    
  glDisable(GL_TEXTURE_2D);
//  glDepthMask(true);
}

void scenaElefanti(float t, int view, int view2) {
  float fov = 60;
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,((float)WIDTH)/HEIGHT,0.1f,1500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  if (view2) {
    gluLookAt(200*cos(0.3*t+view), 200+100*view, 200*sin(0.3*t+view), 0, 100, 0, 0, 1, 0);
  } else {
    gluLookAt(50*cos(-0.3*t), 150, 50*sin(-0.3*t+view), 0, 100, 0, 0, 1, 0);
  }

  drawLandscapeTrees(t, view);

  glDepthMask(false);

  int i;

  static float ot = t;
  if (!view2) {
    static bool once = true;
    if (once) {
      for(i = 0; i < fiori1->num; i++) {
        fiori1->parts[i].r = fiori1->parts[i].r*0.5+0.5;
        fiori1->parts[i].g = fiori1->parts[i].g*0.5+0.5;
        fiori1->parts[i].b = fiori1->parts[i].b*0.5+0.5;
        fiori1->parts[i].a *= 0.8;
      }
      once = false;
    }    
    if (ot != t)
      fiori1->move(t-ot);
    for(i = 0; i < fiori1->num; i++)
      if (fiori1->parts[i].position.z>750) fiori1->parts[i].position.z -= 1500;
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 100, 0);
    drawParticles(*fiori1, pallino1);
  } //else
    //reset = true;
  ot = t;
  glDepthMask(true);
}

void scenaElefanti2(float t) {
  float fov = 60;
	glViewport(0,0,WIDTH,HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov,((float)WIDTH)/HEIGHT,0.1f,1500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  gluLookAt(150+(t-115)*40, 700, 150+(t-115)*40, 0, 0, 0, 0, 1, 0);

  drawLandscapeTrees(t, 4);

  glDepthMask(false);
//static float ot = t;
//if (ot != t)
//  fumo3->move(t-ot);
  drawParticles(*fumo3, smoke1);
  glDepthMask(true);
}

void drawFlashBianco(float aperture) {
  panViewOrtho();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  if (greets)
    greetscreds->use();
  else
    glDisable(GL_TEXTURE_2D);
  glColor4f(1, 1, 1, aperture);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(0, 0);
  glTexCoord2f(1.0f, 0);
  glVertex2f(WIDTH, 0);
  glTexCoord2f(1.0f, 0.75f);
  glVertex2f(WIDTH, HEIGHT);
  glTexCoord2f(0, 0.75f);
  glVertex2f(0, HEIGHT);
  glEnd();
}

// mainloop

static void skDraw() {

  float t = static_cast<float>(panGetTime());
  if (t>136) {
    PostQuitMessage(0);
    return;
  }

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClearDepth(1);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  static const float base = 7.685;
  int timing = (int)(t/base);
  float basetime = timing*base;
  if (timing > 11) {
    timing = 12+(t-11.25*base)*2/base;
    basetime = 11.25*base+(timing-12)*base/2;
  }
  float scenetime = t-basetime;
  
  int noting = (int)(scenetime*4/base);
  float basenote = basetime+(noting*base)/4;

  int mezzing = (int)((t-basenote)*32/base);
  float basemezz = basenote+(mezzing*base)/32;

  int quarting = (int)((t-basenote)*64/base);
  float basequart = basenote+(quarting*base)/64;

//  timing += 20;
  if (timing < 2) { // vermi 1                    timing 0, 1
    if (scenetime < 1.5) 
      algheTraICapelli(t*0.5-1);
    else if (scenetime < 1.675) 
      algheTraICapelli(t*0.5-0.125*2-1);
    else if (scenetime < 3.833)
      algheTraICapelli(t*0.5-0.25*2-1);
    else if (scenetime < 5.7495)
      algheTraICapelli(t*0.5+0.25-1);
    else
      algheTraICapelli(t*0.5+0.75-1);
  } else if (timing < 4) { // vermi 2             timing 2, 3
    algheTraICapelli(5*timing+t*0.5-5+0.5*noting);
  } else if (timing < 7) { // fumo che sale       timing 4, 5, 6
    drawSmoke(t);
    if (timing == 6)
      fumo1->force = 0;
  } else if (timing < 11) { // pallini nel fumo   timing 7, 8, 9, 10
    static int onoting = 128;
    static int spos = 0;
    int noting2 = (((timing - 7) & 1) << 6)+(noting << 4) + quarting;
    if (noting2 < onoting) {
      spos = 0;
    }
    const int syncs[] = {4, 6, 14, 32, 34, 36, 38, 44, 48, 49, 68, 70, 78, 84, 88, 90, 96, 98, 100, 102};
    while (noting2 >= syncs[spos]) {
      for(int i = 0; i < 5; i++)
        fiori1->add(Vector3(50*vlattice((spos*5+i), 1), 50*vlattice((spos*5+i), 2), 100*vlattice((spos*5+i), 3)-100-spos*10), zero3, (vlattice((spos*5+i), 4)*10+30)/3, (vlattice((spos*5+i), 5)*10+30)/600, 0.7+vlattice((spos*5+i), 6)*0.07, 0.515+vlattice((spos*5+i), 7)*0.0515, 0.67+vlattice((spos*5+i), 8)*0.067, 0.5f, t);
      spos++;
    }
    onoting = noting;
    drawSmoke2(t, (timing-7)/2); // i pallini devono comparire a tempo!
  } else if (timing < 14) { // filler             timing 11, 12, 13
    scenaElefantiSferici(t, noting);
  } else if (timing < 22) { // lietofine                           timing 14 a seguire
    scenaElefanti(t, timing - 14, noting);
  } else { // dubbio
    scenaElefanti2(t);
    panViewOrtho();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, vnoise(t*20)*2+1.5);
    greetscreds->use();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0.75+ 1 / 256.0);
    glVertex2f(WIDTH-LOGO_WIDTH, HEIGHT-LOGO_HEIGHT);
    glTexCoord2f(0, 1.0);
    glVertex2f(WIDTH-LOGO_WIDTH, HEIGHT);
    glTexCoord2f(1.0, 1.0);
    glVertex2f(WIDTH, HEIGHT);
    glTexCoord2f(1.0, 0.75 + 1 / 256.0);
    glVertex2f(WIDTH, HEIGHT-LOGO_HEIGHT);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }

  if (((timing == 3) && (noting == 3) && (mezzing == 7)) ||
    ((timing == 6) && (noting == 3) && (mezzing == 7)) ||
    ((timing == 10) && (noting == 3) && (mezzing == 7)) ||
    ((timing == 13) && (noting == 1) && (mezzing == 7)) ||
    ((timing == 21) && (noting == 1) && (mezzing == 7))) {
    drawFlashBianco(((t-basemezz)*32/base));
  }
  if (((timing == 4) && (noting == 0) && (mezzing == 0)) ||
    ((timing == 7) && (noting == 0) && (mezzing == 0)) ||
    ((timing == 11) && (noting == 0) && (mezzing == 0)) ||
    ((timing == 14) && (noting == 0) && (mezzing == 0)) ||
    ((timing == 22) && (noting == 0) && (mezzing == 0))) {
    drawFlashBianco(1-((t-basemezz)*32/base));
  }

#ifdef USETEXT
  panViewOrtho();
  glTranslatef(20, 20, 0);
  glScalef(20, -20, 1);
  glColor4f(0,0,0,1);
  fontarial->printf("%2.2f   %2.2f   %2.2f   %d   %2.2f   %d   %d", t, basetime, scenetime, timing, basenote, noting, quarting);
#endif

/*
  if (timing < 3) { // fumo che sale       timing 4, 5, 6
    drawSmoke(t);
    if (timing == 2)
      fumo1->force = 0;
  } else if (timing < 7) { // pallini nel fumo   timing 7, 8, 9, 10
    drawSmoke2(t); // i pallini devono comparire a tempo!
  } else if (timing < 14) { // filler             timing 11
  } else {
  }
*/
/*
  elefantiTraICapelli3d(t, 40, 0, 0);
  glDepthMask(false);
  fiori1->move(0.1);
  drawParticles(*fiori1,pallino1); // i pallini devono comparire a tempo!
  glDepthMask(true);
*/
  // fumo con pallini che compaiono a tempo
  // scena ripieno
  // albero, erba, pallini, a svanire.
  skSwapBuffers();
}

void skInitDemoStuff()
{
  int i;

  greetscreds = loadTexture(IDB_GCBITMAP, 8);

  mylist = glGenLists(8);
  mylandscape = glGenLists(1);
  mylandscape2 = glGenLists(1);

  for(i = 0; i < 100; i++) {
    for(int j = 0; j < 100; j++) {
      landscape[i][j] = 100*vnoise(i/5.0f, j/5.0f);
    }
  }  

  glNewList(mylandscape, GL_COMPILE);
  glBegin(GL_QUADS);
  for(i = 0; i < 95; i+=2) {
    for(int j = 0; j < 99; j++) {
      float g1 = vlattice(i/10, j/10, 1)*0.03;
      float g2 = vlattice((i+5)/10, (j)/10, 2)*0.03;
      float g3 = vlattice(i/10, (j+5)/10, 2)*0.03;
      glColor4f(0.75+g1, 0.8+g2, 0.7+g3, 1);
      glVertex3f((i-50)*30-900, landscape[i][j], (j-50)*30-600);
      glVertex3f((i-49)*30-900, landscape[i+1][j], (j-50)*30-600);
      glVertex3f((i-49)*30-900, landscape[i+1][j+1], (j-49)*30-600);
      glVertex3f((i-50)*30-900, landscape[i][j+1], (j-49)*30-600);
    }
  }  
  glEnd();
  glEndList();

  glNewList(mylandscape2, GL_COMPILE);
  glBegin(GL_QUADS);
  for(i = 0; i < 40; i+=2) {
    for(int j = 0; j < 40; j++) {
      float g1 = vlattice(i/10, j/10, 1)*0.015;
      float g2 = vlattice((i+5)/10, (j)/10, 2)*0.015;
      float g3 = vlattice(i/10, (j+5)/10, 2)*0.015;
//      if (j == 98) g = 0;
      glColor4f(0.875+g1, 0.9+g2, 0.85+g3, 1);
      glVertex3f((i)*100-2000, 4*landscape[i][j], (j)*100-2000);
      glVertex3f((i+1)*100-2000, 4*landscape[i+1][j], (j)*100-2000);
      glVertex3f((i+1)*100-2000, 4*landscape[i+1][j+1], (j+1)*100-2000);
      glVertex3f((i)*100-2000, 4*landscape[i][j+1], (j+1)*100-2000);
    }
  }  
  glEnd();
  glEndList();

#ifdef USETEXT
  fontarial = new GLFont(hDC, "Arial");
#endif
//	texturefont = perlin(7, 100, 0.6f, 0.5f, 2.5, true);

  fumo1 = new Particles(150);
  for(i = 0; i < fumo1->maxnum; i++) {
    float g = vlattice(i)*0.2+0.2;
    fumo1->add(Vector3(300*vlattice(i, 1), 50*vlattice(i, 1), 300*vlattice(i, 3)), zero3, 5*(vlattice(i, 4)*10+30), vlattice(i, 5)*10+30, g, g, g, 0.1, 0);
  }
//    fumo1->add(Vector3(200*vlattice(i, 1), 80*vlattice(i, 2), 200*vlattice(i, 3)), zero3, vlattice(i, 4)*10+30, vlattice(i, 5)*10+30);
  fumo1->setFriction(0.0001/5.0f);
  fumo1->setForceField(new ArchimedesField(95));
  fumo1->setWind(new NoiseField(0.01, 95));
  smoke1 = smoke(8);

  fumo2 = new Particles(250);
  for(i = 0; i < fumo2->maxnum; i++) {
    float g = vlattice(i)*0.2+0.2;
    fumo2->add(Vector3(400*vlattice(i, 1)-200, 0, 1500*vlattice(i, 3)-750), 100*Vector3(vlattice(i, 10), vlattice(i, 11)+0.2, vlattice(i, 12)), 5*(vlattice(i, 4)*10+30), vlattice(i, 5)*10+30, g, g, g, 0.1, 0);
  }
//    fumo1->add(Vector3(200*vlattice(i, 1), 80*vlattice(i, 2), 200*vlattice(i, 3)), zero3, vlattice(i, 4)*10+30, vlattice(i, 5)*10+30);
  fumo2->setFriction(0.0001);
  fumo2->setForceField(new ZSpringField(0.5));
  fumo2->setWind(new NoiseField(0.05, 150, Vector3(0, 0, 100)));
  for (i = 0; i < 100; i++) {
    fumo2->move(0.02);
    for(int j = 0; j < fumo2->num; j++) 
      if (fumo2->parts[j].position.z > 900) fumo2->parts[j].position.z -= 1800;
  }
  for(i = 0; i < fumo2->num; i++) 
    fumo2->parts[i].speed.x += 75;


  fumo3 = new Particles(150);
  for(i = 0; i < fumo1->maxnum; i++) {
    float r = vlattice(i, 7)*0.5+0.5;
    float g = vlattice(i, 8)*0.5+0.5;
    float b = vlattice(i, 9)*0.5+0.5;
    fumo3->add(Vector3(1500*vlattice(i, 1), 50*vlattice(i, 1)+500, 1500*vlattice(i, 3)), zero3, 5*(vlattice(i, 4)*10+30), vlattice(i, 5)*10+30, r, g, b, 0.2, 0);
  }
//    fumo1->add(Vector3(200*vlattice(i, 1), 80*vlattice(i, 2), 200*vlattice(i, 3)), zero3, vlattice(i, 4)*10+30, vlattice(i, 5)*10+30);
  fumo3->setFriction(0);
  fumo3->setForceField(0);
  fumo3->setWind(0);


  fiori1 = new Particles(400);
//  for(i = 0; i < fiori1->maxnum; i++)
//    fiori1->add(Vector3(200*vlattice(i, 1)-100, 200*vlattice(i, 1)-100, 1500*vlattice(i, 3)-750), zero3, (vlattice(i, 4)*10+30)/3, (vlattice(i, 5)*10+30)/60, 0.7, 0.515, 0.67, 0.5f);
//    fumo1->add(Vector3(200*vlattice(i, 1), 80*vlattice(i, 2), 200*vlattice(i, 3)), zero3, vlattice(i, 4)*10+30, vlattice(i, 5)*10+30);
  fiori1->setFriction(0.0001);
  fiori1->setForceField(new ZSpringField(0.03));
  fiori1->setWind(new NoiseField(0.05, 200, Vector3(0, 0, 100)));

  pallino1=circle(4);
  pallino2=circle2(8);


}

int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR lpcmdline,int ncmdshow) {
  if (strcmpi(lpcmdline,"/g") == 0)
    greets = true;
  g_hinstance = hinstance;
  WNDCLASSEX winclass = {sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW | CS_OWNDC, (WNDPROC)(skWinProc), 0, 0, 0, 0, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_APPWORKSPACE), 0, WINDOW_CLASS_NAME, 0}; // this is long
	winclass.hInstance		= hinstance;
  HDC				l_hDC;
	l_hDC=GetDC(GetDesktopWindow());
	if (!RegisterClassEx(&winclass)) 
    return 0;
	PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, BITSPERPIXEL, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 16, 1, 0, PFD_MAIN_PLANE, 0, 0, 0, 0}; // but his is even longer....
	unsigned int  iPixelFormat; 
	RECT windowRect = {0, 0, WIDTH, HEIGHT};
  DWORD windowStyle, windowExtendedStyle;

#ifdef FULLSCREEN
  int oldwidth = GetDeviceCaps(l_hDC, HORZRES);
  int oldheight = GetDeviceCaps(l_hDC, VERTRES);
  int oldbpp = GetDeviceCaps(l_hDC, BITSPIXEL);
  int oldfreq = GetDeviceCaps(l_hDC, VREFRESH);

	DEVMODE dmScreenSettings;
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));
	dmScreenSettings.dmSize				= sizeof (DEVMODE);
	dmScreenSettings.dmPelsWidth		= WIDTH;
	dmScreenSettings.dmPelsHeight		= HEIGHT;
	dmScreenSettings.dmBitsPerPel		= BITSPERPIXEL;
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
  if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
    MessageBox(0, "FullScreen mode not available", WINDOW_TITLE, MB_OK);
  	windowStyle = WS_OVERLAPPEDWINDOW;
  	windowExtendedStyle = WS_EX_APPWINDOW;
    AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
  } else {
  	ShowCursor(false);
  	windowStyle = WS_POPUP;
  	windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_TOPMOST;
  }
#else
	windowStyle = WS_OVERLAPPEDWINDOW;
	windowExtendedStyle = WS_EX_APPWINDOW;
  AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
#endif
  if (hWND = CreateWindowEx (windowExtendedStyle, WINDOW_CLASS_NAME, WINDOW_TITLE, windowStyle, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, HWND_DESKTOP, 0, hinstance, NULL)) { // so i can say that this one is not that long
  	hDC = GetDC(hWND);
  	iPixelFormat = ChoosePixelFormat(hDC, &pfd);
  	SetPixelFormat(hDC, iPixelFormat, &pfd); 
  	if (hRC = wglCreateContext(hDC)) 
      wglMakeCurrent(hDC, hRC); 
  	ShowWindow(hWND, SW_NORMAL);
    skInitDemoStuff();
    FSOUND_File_SetCallbacks(memopen, memclose, memread, memseek, memtell);
    if (music = (FSOUND_Init(44100, 0) && (fmodule = FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_RC_RTDATA1), NULL))))
      FMUSIC_PlaySong(fmodule);  
    panInitTimer();
    while (true) {
  		MSG msg;
  		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) { 
  			if (msg.message == WM_QUIT) 
          break;
  			TranslateMessage(&msg);
  			DispatchMessage(&msg);
  		} else
  			skDraw();
  	}
    if (music) {
  	  FMUSIC_FreeSong(fmodule);
  	  FSOUND_Close();
    }
  } 
  else
    MessageBox(GetDesktopWindow(), "Can't create window", "SKerror", MB_OK);
#ifdef FULLSCREEN
  static DEVMODE mode;
  mode.dmSize=sizeof(mode);
  mode.dmBitsPerPel=oldbpp;
  mode.dmPelsWidth=oldwidth;
  mode.dmPelsHeight=oldheight;
  mode.dmDisplayFrequency = oldfreq;
  mode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
  ChangeDisplaySettings(&mode, 0);
	ShowCursor(true);
#endif
  ExitProcess(0);
  return 0;
}


