
// bilaterian metazoans
// Eukariotes

/************************************
  Code By: Pan/SpinningKids
  Revised on 5/5/2002 11:03:36 PM
  Comments: Creation of a demiurg
  Portions by: Rio/SpinningKids
 ************************************/

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "windows/resource.h"
#else
#include "SDL.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#endif

#include "Globals.h"
#include "GenTex.h"
#include "noise.h"
#include "Particles.h"
#include "utils.h"

#include <memory>
#include <numeric>
#include <tuple>

using namespace std;

bool greets = false;

//globals
/*##########################################################*/
/*Standard Var definitions :								*/
/*##########################################################*/

Particles* smoke1_particles;
Particles* smoke2_particles;
Particles* smoke3_particles;
Particles* flower_particles;

unique_ptr<GLTexture> smoke_texture;
unique_ptr<GLTexture> smallseed_texture;
unique_ptr<GLTexture> bigseed_texture;
unique_ptr<GLTexture> greetscreds_texture;
float landscape[100][100];

GLint tree_list_base;
GLint landscape1_list;
GLint landscape2_list;

// Particles Stuff

class FixedField : Field {
    Vector3 value;
public:
    FixedField(const Vector3& _value) : value(_value) {}
    Vector3 operator () (const ParticleInfo&) const override { return value; }
};

class  NoiseField : public Field {
    float scale, amplitude;
    Vector3 carrier;
public:
    NoiseField(float _scale, float _amplitude, const Vector3& _carrier = zero3) : scale(_scale), amplitude(_amplitude), carrier(_carrier) {}
    Vector3 operator () (const ParticleInfo& p) const override {
        Vector3 a = scale * p.position;
        return { carrier.x + amplitude * vnoise(a.y, a.z), carrier.y + amplitude * vnoise(a.z, a.x), carrier.z + amplitude * vnoise(a.x, a.y) };
    }
};

class ArchimedesField : public Field {
    float density;
public:
    ArchimedesField(float _density) : density(_density) {}
    Vector3 operator () (const ParticleInfo& p) const override {
        constexpr float fourthirdspi = 4.1887902047863909846168578443727f;
        return { 0, density - p.mass / (fourthirdspi * p.size * p.size * p.size), 0 };
    }
};

struct ZSpringField : Field {
    float k;
    ZSpringField(float _k) : k(_k) {}
    Vector3 operator () (const ParticleInfo& p) const override {
        return { -k * p.position.x, -k * p.position.y, 0 };
    }
};

//effects

static void glVertex3v3v(const Vector3& v) {
    float tmp[3];
    static_assert(sizeof(Vector3) == sizeof(tmp));
    memcpy(tmp, &v, sizeof(tmp));
    glVertex3fv(tmp);
}

static void algheTraICapelli(float t) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    panViewOrthoModified();
    const int cosi = 120;
    const int steps = 20;
    for (int i = 0; i < cosi; i++) {
        float phase = i * 6.28f / cosi + 0.1f * vnoise((float)i, t);
        float rho = 300 - t * 30 + 20 * vnoise(t, (float)i) + 150 * vlattice(i);
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= steps; j++) {
            float ph = phase + 0.1f * vnoise((float)i, 0.1f * j, t);
            float sz = 0.2f * (j + 1);
            float rh = rho + 10 * (j + 1) + vnoise(0.1f * j, (float)i, t);
            float xb = rh * cosf(ph) + 320;
            float yb = rh * sinf(ph) + 240;
            glColor4f(0.8f, 0.1f * vlattice(1, i) + 0.05f, 0.1f * vlattice(2, i) + 0.05f, 0.01f * (steps - j));
            float sph = sz * sinf(ph);
            float cph = sz * cosf(ph);
            glVertex2f(xb - sph, yb + cph);
            glVertex2f(xb + sph, yb - cph);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
}

static std::tuple<Vector3, Vector3> getStupidVectors() {
    float model[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, model);
    return {
        {model[0] + model[1], model[4] + model[5], model[8] + model[9]},
        {model[1] - model[0], model[5] - model[4], model[9] - model[8] }
    };
}

static void drawBillboardQuad(const Vector3& p, const Vector3 &sur, const Vector3 &sul, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glTexCoord2f(0, 0);
    glVertex3v3v(p - sur);
    glTexCoord2f(1, 0);
    glVertex3v3v(p - sul);
    glTexCoord2f(1, 1);
    glVertex3v3v(p + sur);
    glTexCoord2f(0, 1);
    glVertex3v3v(p + sul);
}

static void drawParticles(const Particles& ps, const GLTexture& tex) {

    auto [ur, ul] = getStupidVectors();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    tex.use();

    glBegin(GL_QUADS);
    for (const auto& pi : ps.parts) {
        drawBillboardQuad(pi.position, pi.size * ur, pi.size * ul, pi.r, pi.g, pi.b, pi.a);
    }
    glEnd();
    GLTexture::disable();
    glDisable(GL_BLEND);
}

void drawSmoke(float t) {
    static float ot = t;

    const float dt = t - ot;
    if (dt != 0)
    {
        smoke1_particles->move(dt);
    }

    const Vector3 centre = std::accumulate(smoke1_particles->parts.begin(), smoke1_particles->parts.end(), Vector3{ 0.f, 0.f, 0.f }, [](auto acc, auto& el) { return acc + el.position; }) / smoke1_particles->parts.size();
    panViewPerspective(90, 0.1, 2000.);
    gluLookAt(150, 300, 150, centre.x, centre.y, centre.z, 0, 1, 0);

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    constexpr float a[3]{ 1,1,1 };
    glFogfv(GL_FOG_COLOR, a);
    glFogf(GL_FOG_START, 300);
    glFogf(GL_FOG_END, 800);


    glDisable(GL_BLEND);
    GLTexture::disable();

    glBegin(GL_QUADS);

    glColor4f(0.7f, 0.75f, 0.8f, 1);

    glVertex3f(-50 * 30 - 900, -100, -50 * 30 - 600);
    glVertex3f(50 * 30 - 900, -100, -50 * 30 - 600);
    glVertex3f(50 * 30 - 900, -100, 50 * 30 - 600);
    glVertex3f(-50 * 30 - 900, -100, 50 * 30 - 600);

    //    glColor4f(0.7, 0.75, 0.8, 1);

    glVertex3f(-50 * 30 - 900, 600, -50 * 30 - 600);
    glVertex3f(50 * 30 - 900, 600, -50 * 30 - 600);
    glVertex3f(50 * 30 - 900, 600, 50 * 30 - 600);
    glVertex3f(-50 * 30 - 900, 600, 50 * 30 - 600);

    glEnd();

    glCallList(landscape1_list);

    glDisable(GL_FOG);

    drawParticles(*smoke1_particles, *smoke_texture);

    ot = t;
};

void drawSmoke2(float t, int v) {
    static float ot = t;

    const float dt = t - ot;
    if (dt != 0) {
        smoke2_particles->move(dt);
        flower_particles->move(dt);
    }

    smoke2_particles->parts.erase(remove_if(smoke2_particles->parts.begin(), smoke2_particles->parts.end(), [](auto& p) { return p.position.z > 900; }), smoke2_particles->parts.end());

    panViewOrtho();

    glBegin(GL_QUADS);
    glColor4f(1, 1, 1, 1);
    glVertex2i(0, HEIGHT / 2);
    glVertex2i(WIDTH, HEIGHT / 2);
    glColor4f(0.85f, 0.875f, 0.9f, 1);
    glVertex2i(WIDTH, 0);
    glVertex2i(0, 0);
    glEnd();

    panViewPerspective(90, 0.1, 2000.);

    gluLookAt(50 * sin(t * 1) + ((v) ? (150) : (-150)), 50 * cos(t * 1) + 150, 100, 0., 0., 0., 0, 1, 0);

    drawParticles(*smoke2_particles, *smoke_texture);
    glTranslatef(0, 50, 0);
    drawParticles(*flower_particles, *smallseed_texture);

    ot = t;
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const int cosi = 30;
    const float thick = 20;
    const float k = 200.f / steps;

    for (int i = 0; i < cosi; i++) {

        float s = 0.5f * thick / steps; //(i*thick/cosi)/steps;

        Vector3 p = pf * (-3.0f * i / cosi);
        float phx = tphx;
        float phz = tphz;
        glBegin(GL_QUAD_STRIP);
        glColor4f(0.3f, 0.3f, 0.0f, 0.000);

        Vector3 delta = (thick / 2 * up.y) * rt - (thick / 2 * rt.y) * up;

        glVertex3v3v(p + delta);
        glVertex3v3v(p - delta);

        float cr = vlattice(1, i) * 0.2f;
        float cg = vlattice(2, i) * 0.2f;
        float cb = vlattice(3, i) * 0.2f;

        float rot = vlattice(4, i);

        for (int j = 0; j <= steps; j++) {
            float rotx = vnoise(0.1f * i, rot * 1.2f * j / steps + t * 0.15f + t * vlattice(i) * 0.05f) * j * j * 10.0f / (steps * steps) + 0.1f * vnoise((60.f * j) / steps) + 0.001f * vlattice(i) + 0.5f * vnoise(t * 0.3f - j * 0.1f, 1 * ((float)i) / cosi);
            float rotz = vnoise(0.1f * i + 10, rot * 1.2f * j / steps + t * 0.15f + t * vlattice(i) * 0.05f) * j * j * 10.0f / (steps * steps) + 0.1f * vnoise((60.f * j) / steps) + 0.001f * vlattice(i) + 0.5f * vnoise(t * 0.3f + 3 - j * 0.1f, 1 * ((float)i) / cosi);

            rotx = 0.07f * rotx;
            rotz = 0.07f * rotz;
            phx = phx + rotx;
            phz = phz + rotz;
            delta.x = sinf(phx);
            delta.y = cosf(phx) * cosf(phz);
            delta.z = sinf(phz);

            p += k * delta;

            //   new delta

            float sz = s * (steps - j);
            delta = (sz * (delta * up)) * rt - (sz * (delta * rt)) * up;

            float alpha = 0.005f * (j + 10) + 0.25f;
            float alpha2 = (j < 10) ? 0.666f : (j < 15) ? (j / 15.0f) : 1;

            switch (view) {
            case 0:
                glColor4f(
                    (cr + 0.3f - j * 0.001f) * alpha + 1 - alpha,
                    (cg + 0.3f) * alpha + 1 - alpha,
                    (0.0025f * (j + 10)) * alpha + 1 - alpha,
                    alpha2
                );
                break;
            case 1:
                glColor4f(
                    (cr + 0.5f - j * 0.001f) * alpha + 1 - alpha,
                    (cg + 0.2f) * alpha + 1 - alpha,
                    (0.0025f * (j + 10)) * alpha + 1 - alpha,
                    alpha2
                );
                break;
            case 2:
                glColor4f(
                    (j * 0.001f) * alpha + 1 - alpha,
                    (cg + 0.25f) * alpha + 1 - alpha,
                    (cb + 0.525f + 0.0025f * j) * alpha + 1 - alpha,
                    alpha2
                );
                break;
            case 3:
                glColor4f(
                    (cr + 0.3f - j * 0.001f) * alpha + 1 - alpha,
                    (cg + 0.2f) * alpha + 1 - alpha,
                    (cb + 0.425f + 0.0025f * j) * alpha + 1 - alpha,
                    alpha2
                );
                break;
            }

            glVertex3v3v(p + delta);
            glVertex3v3v(p - delta);
        }
        glEnd();

    }
    glDisable(GL_BLEND);
}

void drawLandscapeTrees(float t, int view) {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    constexpr float a[3]{ 1,1,1 };
    glFogfv(GL_FOG_COLOR, a);
    glFogf(GL_FOG_START, 600.f);
    glFogf(GL_FOG_END, 1500.f);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glCallList(landscape2_list);
    for (int i = 0; i < 2; i++) {
        glNewList(tree_list_base + i, GL_COMPILE);
        if (view == 4) {
            elefantiTraICapelli3d(t + i * 100.f, 30, 0.f, 0.f, (2 + i) & 3);
        } else {
            elefantiTraICapelli3d(t + i * 100.f, 30, 0.f, 0.f, view & 3);
        }
        glEndList();
    }

    for (int i = 10; i < 30; i += 2) {
        for (int j = 10; j < 30; j += 2) {
            glPushMatrix();
            glTranslatef((i + 0.5f) * 100 - 2000, (landscape[i][j] + landscape[i + 1][j] + landscape[i + 1][j + 1] + landscape[i][j + 1]), (j + 0.5f) * 100 - 2000);
            glCallList(tree_list_base + (((i + j) / 2) & 1));
            glPopMatrix();
        }
    }
}

void scenaElefantiSferici(float t, int view) {
    panViewPerspective(60, 0.1, 1500.);
    gluLookAt(100 * cos(-0.3 * t), 0, 100 * sin(-0.3 * t + view), 0, 0, 0, 0, 1, 0);

    auto [ur, ul] = getStupidVectors();

    struct Sphere {
        Vector3 pos;
        float size;
        float r, g, b, a;
    };

    Sphere seeds[10];
    for (int i = 0; i < 10; i++) {
        seeds[i].pos = 50 * Vector3(vnoise(t * 0.1f, 1, (float)i), vnoise(t * 0.1f, 2, (float)i), vnoise(t * 0.1f, 3, (float)i));
        seeds[i].size = vlattice(i) * 10 + 20;
        seeds[i].r = 0.7f + vlattice((i), 6) * 0.07f;
        seeds[i].g = 0.515f + vlattice((i), 7) * 0.0515f;
        seeds[i].b = 0.67f + vlattice((i), 8) * 0.067f;
        seeds[i].a = 0.5f;
        glPushMatrix();

        glTranslatef(seeds[i].pos.x, seeds[i].pos.y, seeds[i].pos.z);
        glRotatef(360 * vnoise(t * 0.1f, 5, (float)i), 1, 0, 0);
        glRotatef(360 * vnoise(t * 0.1f, 6, (float)i), 0, 1, 0);
        glScalef(seeds[i].size / 150.0f, seeds[i].size / 150.0f, seeds[i].size / 150.0f);
        glTranslatef(0, -140, 0);

        elefantiTraICapelli3d(t, 10, 0, 0, view);

        glPopMatrix();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    bigseed_texture->use();
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_QUADS);
    for (auto &[pos, size, r, g, b, a]: seeds) {
        drawBillboardQuad(pos, size * ur, size * ul, r, g, b, a);
    }
    glEnd();
    GLTexture::disable();
}

void scenaElefanti(float t, int view, int view2) {
    panViewPerspective(60, 0.1, 1500.);
    if (view2) {
        gluLookAt(200 * cos(0.3 * t + view), 200 + 100 * view, 200 * sin(0.3 * t + view), 0, 100, 0, 0, 1, 0);
    } else {
        gluLookAt(50 * cos(-0.3 * t), 150, 50 * sin(-0.3 * t + view), 0, 100, 0, 0, 1, 0);
    }

    drawLandscapeTrees(t, view);

    glDepthMask(false);

    static float ot = t;
    if (!view2) {
        static bool once = true;
        if (once) {
            for (auto& p : flower_particles->parts) {
                p.r = p.r * 0.5f + 0.5f;
                p.g = p.g * 0.5f + 0.5f;
                p.b = p.b * 0.5f + 0.5f;
                p.a *= 0.8f;
            }
            once = false;
        }
        if (ot != t) {
            flower_particles->move(t - ot);
        }
        for (auto& p : flower_particles->parts) {
            if (p.position.z > 750) p.position.z -= 1500;
        }
        glRotatef(90, 0, 1, 0);
        glTranslatef(0, 100, 0);
        drawParticles(*flower_particles, *smallseed_texture);
    }
    ot = t;
    glDepthMask(true);
}

void scenaElefanti2(float t) {
    panViewPerspective(60, 0.1, 1500.);
    gluLookAt(150 + (t - 115) * 40, 700, 150 + (t - 115) * 40, 0, 0, 0, 0, 1, 0);

    drawLandscapeTrees(t, 4);

    glDepthMask(false);
    drawParticles(*smoke3_particles, *smoke_texture);
    glDepthMask(true);
}

void drawWhiteFlash(float aperture) {
    panViewOrtho();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    if (greets) {
        greetscreds_texture->use();
    } else {
        GLTexture::disable();
    }
    glColor4f(1, 1, 1, aperture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2i(0, 0);
    glTexCoord2f(1.0f, 0);
    glVertex2i(WIDTH, 0);
    glTexCoord2f(1.0f, 0.75f);
    glVertex2i(WIDTH, HEIGHT);
    glTexCoord2f(0, 0.75f);
    glVertex2i(0, HEIGHT);
    glEnd();
}

// mainloop

void skDraw() {

    float t = skGetTime();
    if (t > 136) {
        skQuitDemo();
        return;
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearDepth(1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    static const float base = 7.685f;
    int timing = (int)(t / base);
    float basetime = timing * base;
    if (timing > 11) {
        timing = 12 + (int)((t - 11.25f * base) * 2 / base);
        basetime = 11.25f * base + (timing - 12) * base / 2;
    }
    const float scenetime = t - basetime;

    const int noting = (int)(scenetime * 4 / base);
    const float basenote = basetime + (noting * base) / 4;

    const int mezzing = (int)((t - basenote) * 32 / base);
    const float basemezz = basenote + (mezzing * base) / 32;

    const int quarting = (int)((t - basenote) * 64 / base);
    //float basequart = basenote + (quarting * base) / 64;

    //    timing += 20;
    if (timing < 2) { // worms 1                    timing 0, 1
        if (scenetime < 1.5) {
            algheTraICapelli(t * 0.5f - 1);
        } else if (scenetime < 1.675f) {
            algheTraICapelli(t * 0.5f - 0.125f * 2 - 1);
        } else if (scenetime < 3.833f) {
            algheTraICapelli(t * 0.5f - 0.25f * 2 - 1);
        } else if (scenetime < 5.7495f) {
            algheTraICapelli(t * 0.5f + 0.25f - 1);
        } else {
            algheTraICapelli(t * 0.5f + 0.75f - 1);
        }
    } else if (timing < 4) { // worms 2             timing 2, 3
        algheTraICapelli(5 * timing + t * 0.5f - 5 + 0.5f * noting);
    } else if (timing < 7) { // rising smoke       timing 4, 5, 6
        drawSmoke(t);
        if (timing == 6) {
            smoke1_particles->force = nullptr;
        }
    } else if (timing < 11) { // seeds in the smoke   timing 7, 8, 9, 10
        static int onoting = 128;
        static int spos = 0;
        const int noting2 = (((timing - 7) & 1) << 6) + (noting << 4) + quarting;
        if (noting2 < onoting) {
            spos = 0;
        }
        const int syncs[] = { 4, 6, 14, 32, 34, 36, 38, 44, 48, 49, 68, 70, 78, 84, 88, 90, 96, 98, 100, 102 };
        while (noting2 >= syncs[spos]) {
            for (int i = 0; i < 5; i++) {
                flower_particles->add(
                    { 50 * vlattice((spos * 5 + i), 1), 50 * vlattice((spos * 5 + i), 2), 100 * vlattice((spos * 5 + i), 3) - 100 - spos * 10 }, zero3, (vlattice((spos * 5 + i), 4) * 10 + 30) / 3, (vlattice((spos * 5 + i), 5) * 10 + 30) / 600, 0.7f + vlattice((spos * 5 + i), 6) * 0.07f, 0.515f + vlattice((spos * 5 + i), 7) * 0.0515f, 0.67f + vlattice((spos * 5 + i), 8) * 0.067f, 0.5f, t);
            }
            spos++;
        }
        onoting = noting;
        drawSmoke2(t, (timing - 7) / 2); // seeds must appear in sync with music!
    } else if (timing < 14) { // filler             timing 11, 12, 13
        scenaElefantiSferici(t, noting);
    } else if (timing < 22) { // happy ending?                           timing 14 and following
        scenaElefanti(t, timing - 14, noting);
    } else { // doubt
        const int logo_width = WIDTH * 256 / 1280;
        const int logo_height = WIDTH * 63 / 1280;
        scenaElefanti2(t);
        panViewOrtho();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0, 0, 0, vnoise(t * 20) * 2 + 1.5f);
        greetscreds_texture->use();
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0.75 + 1 / 256.0);
        glVertex2i(WIDTH - logo_width, HEIGHT - logo_height);
        glTexCoord2f(0, 1.0);
        glVertex2i(WIDTH - logo_width, HEIGHT);
        glTexCoord2f(1.0, 1.0);
        glVertex2i(WIDTH, HEIGHT);
        glTexCoord2f(1.0, 0.75 + 1 / 256.0);
        glVertex2i(WIDTH, HEIGHT - logo_height);
        glEnd();
        GLTexture::disable();
    }

    if (((timing == 3) && (noting == 3) && (mezzing == 7)) ||
        ((timing == 6) && (noting == 3) && (mezzing == 7)) ||
        ((timing == 10) && (noting == 3) && (mezzing == 7)) ||
        ((timing == 13) && (noting == 1) && (mezzing == 7)) ||
        ((timing == 21) && (noting == 1) && (mezzing == 7))) {
        drawWhiteFlash(((t - basemezz) * 32 / base));
    }
    if (((timing == 4) && (noting == 0) && (mezzing == 0)) ||
        ((timing == 7) && (noting == 0) && (mezzing == 0)) ||
        ((timing == 11) && (noting == 0) && (mezzing == 0)) ||
        ((timing == 14) && (noting == 0) && (mezzing == 0)) ||
        ((timing == 22) && (noting == 0) && (mezzing == 0))) {
        drawWhiteFlash(1 - ((t - basemezz) * 32 / base));
    }
}

void skInitDemoStuff() {
    greetscreds_texture = skLoadTexture(IDB_GCBITMAP, 8);

    tree_list_base = glGenLists(8);
    landscape1_list = glGenLists(1);
    landscape2_list = glGenLists(1);

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            landscape[i][j] = 100 * vnoise(i / 5.0f, j / 5.0f);
        }
    }

    glNewList(landscape1_list, GL_COMPILE);
    glBegin(GL_QUADS);
    for (int i = 0; i < 95; i += 2) {
        for (int j = 0; j < 99; j++) {
            float g1 = vlattice(i / 10, j / 10, 1) * 0.03f;
            float g2 = vlattice((i + 5) / 10, (j) / 10, 2) * 0.03f;
            float g3 = vlattice(i / 10, (j + 5) / 10, 2) * 0.03f;
            glColor4f(0.75f + g1, 0.8f + g2, 0.7f + g3, 1);
            glVertex3f((i - 50.f) * 30.f - 900.f, landscape[i][j], (j - 50.f) * 30.f - 600.f);
            glVertex3f((i - 49.f) * 30.f - 900.f, landscape[i + 1][j], (j - 50.f) * 30.f - 600.f);
            glVertex3f((i - 49.f) * 30.f - 900.f, landscape[i + 1][j + 1], (j - 49.f) * 30.f - 600.f);
            glVertex3f((i - 50.f) * 30.f - 900.f, landscape[i][j + 1], (j - 49.f) * 30.f - 600.f);
        }
    }
    glEnd();
    glEndList();

    glNewList(landscape2_list, GL_COMPILE);
    glBegin(GL_QUADS);
    for (int i = 0; i < 40; i += 2) {
        for (int j = 0; j < 40; j++) {
            //            if (j == 98) g = 0;
            glColor4f(
                0.875f + vlattice(i / 10, j / 10, 1) * 0.015f,
                0.9f + vlattice((i + 5) / 10, j / 10, 2) * 0.015f,
                0.85f + vlattice(i / 10, (j + 5) / 10, 2) * 0.015f,
                1);
            glVertex3f((i) * 100.f - 2000.f, 4 * landscape[i][j], (j) * 100.f - 2000.f);
            glVertex3f((i + 1) * 100.f - 2000.f, 4 * landscape[i + 1][j], (j) * 100.f - 2000.f);
            glVertex3f((i + 1) * 100.f - 2000.f, 4 * landscape[i + 1][j + 1], (j + 1) * 100.f - 2000.f);
            glVertex3f((i) * 100.f - 2000.f, 4 * landscape[i][j + 1], (j + 1) * 100.f - 2000.f);
        }
    }
    glEnd();
    glEndList();

    smoke1_particles = new Particles(150);
    for (size_t i = 0; i < smoke1_particles->parts.capacity(); i++) {
        float g = vlattice(i) * 0.2f + 0.2f;
        smoke1_particles->add({ 300 * vlattice(i, 1), 50 * vlattice(i, 1), 300 * vlattice(i, 3) }, zero3, 5 * (vlattice(i, 4) * 10 + 30), vlattice(i, 5) * 10 + 30, g, g, g, 0.1f, 0);
    }
    smoke1_particles->setFriction(0.0001f / 5.0f);
    smoke1_particles->setForceField(new ArchimedesField(95));
    smoke1_particles->setWind(new NoiseField(0.01f, 95));
    smoke_texture = smoke(8);

    smoke2_particles = new Particles(250);
    for (size_t i = 0; i < smoke2_particles->parts.capacity(); i++) {
        float g = vlattice(i) * 0.2f + 0.2f;
        smoke2_particles->add({ 400 * vlattice(i, 1) - 200, 0, 1500 * vlattice(i, 3) - 750 }, { 100 * vlattice(i, 10), 100 * vlattice(i, 11) + 0.2f, 100 * vlattice(i, 12) }, 5 * (vlattice(i, 4) * 10 + 30), vlattice(i, 5) * 10 + 30, g, g, g, 0.1f, 0);
    }
    smoke2_particles->setFriction(0.0001f);
    smoke2_particles->setForceField(new ZSpringField(0.5f));
    smoke2_particles->setWind(new NoiseField(0.05f, 150, { 0, 0, 100 }));
    for (int i = 0; i < 100; i++) {
        smoke2_particles->move(0.02f);
        for (auto& p : smoke2_particles->parts) {
            if (p.position.z > 900) p.position.z -= 1800;
        }
    }
    for (auto& p : smoke2_particles->parts) {
        p.speed.x += 75;
    }

    smoke3_particles = new Particles(150);
    for (size_t i = 0; i < smoke3_particles->parts.capacity(); i++) {
        smoke3_particles->add(
            { 1500 * vlattice(i, 1), 50 * vlattice(i, 1) + 500, 1500 * vlattice(i, 3) },
            zero3,
            5 * (vlattice(i, 4) * 10 + 30),
            vlattice(i, 5) * 10 + 30,
            vlattice(i, 7) * 0.5f + 0.5f,
            vlattice(i, 8) * 0.5f + 0.5f,
            vlattice(i, 9) * 0.5f + 0.5f,
            0.2f,
            0);
    }
    smoke3_particles->setFriction(0);
    smoke3_particles->setForceField(nullptr);
    smoke3_particles->setWind(nullptr);

    flower_particles = new Particles(400);
    flower_particles->setFriction(0.0001f);
    flower_particles->setForceField(new ZSpringField(0.03f));
    flower_particles->setWind(new NoiseField(0.05f, 200, { 0, 0, 100 }));

    smallseed_texture = circle(4);
    bigseed_texture = circle2(8);
}

void skUnloadDemoStuff()
{

}
