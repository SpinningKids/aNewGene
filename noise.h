
/************************************
  Code By: Pan/SpinningKids
  Revised on 3/15/2001 10:12:30 PM
  Comments: creation
 ************************************/

#ifndef NOISE_H_
#define NOISE_H_

extern float vnoise(int num, float x[]);
extern float vlattice(int num, int x[]);
extern float vnoise(float t);
extern float vlattice(int idx);

inline float vnoise(float x, float y) {
    float a[] = { x, y };
    return vnoise(2, a);
}

inline float vnoise(float x, float y, float z) {
    float a[] = { x, y, z };
    return vnoise(3, a);
}

inline float vnoise(float x, float y, float z, float w) {
    float a[] = { x, y, z, w };
    return vnoise(4, a);
}

inline float vlattice(int x, int y) {
    int a[] = { x, y };
    return vlattice(2, a);
}

inline float vlattice(int x, int y, int z) {
    int a[] = { x, y, z };
    return vlattice(3, a);
}

inline float vlattice(int x, int y, int z, int w) {
    int a[] = { x, y, z, w };
    return vlattice(4, a);
}


#endif // NOISE_H_
