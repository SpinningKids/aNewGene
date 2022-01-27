
/************************************
  Code By: Pan/SpinningKids
  Revised on 3/15/2001 10:11:47 PM
  Comments: creation
 ************************************/

#include "noise.h"

#include <cassert>
#include <stdlib.h>
#include <math.h>

static int pan_random() {
    return (rand() << 16) + (rand() << 1) + (rand() & 1);
}

static void pan_srandom(int seed) {
    srand(seed);
}

// maths

#define FLOOR(x) ((int)floorf(x))

static float catmullrom(float x, const float knots[4]) {
    float c3 = -0.5f * knots[0] + 1.5f * knots[1] - 1.5f * knots[2] + 0.5f * knots[3];
    float c2 = knots[0] - 2.5f * knots[1] + 2.f * knots[2] - 0.5f * knots[3];
    float c1 = -0.5f * knots[0] + 0.5f * knots[2];
    float c0 = knots[1];
    return (float)(((c3 * x + c2) * x + c1) * x + c0);
}

#define TABSIZE 65536
#define TABMASK (TABSIZE - 1)
#define PERM(x) (perm[x & TABMASK])

float valueTab[TABSIZE];
int perm[TABSIZE];

bool valueTabInit(int seed) {
    float* table = valueTab;
    pan_srandom(seed);
    for (int i = 0; i < TABSIZE; i++) {
        *table++ = (float)(1.f - 2.f * i / (TABSIZE - 1));
        perm[i] = i;
    }
    for (int i = 0; i < 16 * (TABSIZE); i++) {
        int i1 = pan_random() & TABMASK;
        int i2 = pan_random() & TABMASK;
        int tmp = perm[i1];
        perm[i1] = perm[i2];
        perm[i2] = tmp;
    }
    return true;
}

static bool init = valueTabInit(665);

int index(int num, int idx[]) {
    int newidx = 0;
    for (int i = 0; i < num; i++) {
        newidx = PERM(newidx + idx[i]);
    }
    return newidx & (TABMASK);
}

float vlattice(int num, int idx[]) {
    return valueTab[index(num, idx)];
}

float vlattice(int idx) {
    return valueTab[PERM(idx)];
}

float vnoise2(int off, int idx, int num, int ix[], float fx[]) {
    float knots[4];
    ix[off] -= 1;
    for (int i = -1; i <= 2; i++) {
        if (off > 0) {
            knots[i + 1] = vnoise2(off - 1, PERM(idx + ix[off]), num, ix, fx);
        } else {
            knots[i + 1] = valueTab[PERM(idx + ix[off])];//vlattice(num, ix);
        }
        ix[off]++;
    }
    ix[off] -= 3;
    return catmullrom(fx[off], knots);
}

float vnoise(int num, float x[]) {
    assert(num < 16);
    int ix[16];
    float fx[16];
    for (int i = 0; i < num; i++) {
        ix[i] = FLOOR(x[i]);
        fx[i] = x[i] - ix[i];
    }
    return vnoise2(num - 1, 0, num, ix, fx);
}

float vnoise(float t) {
    int ti = FLOOR(t);
    float tf = t - ti;
    ti--;
    float t0 = valueTab[PERM(ti)];
    ti++;
    float t1 = valueTab[PERM(ti)];
    ti++;
    float t2 = valueTab[PERM(ti)];
    ti++;
    float t3 = valueTab[PERM(ti)];
    return 0.5f * ((((3 * t1 - 3 * t2 + t3 - t0) * tf + 2 * t0 - 5 * t1 + 4 * t2 - t3) * tf + t2 - t0) * tf) + t1;
}
