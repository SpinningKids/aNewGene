
#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "Math3.h"

struct Particles;

struct Field {
  virtual Vector3 operator () (Particles &p, int num) = 0;
};

//typedef Vector3 Field(Particles &, int);
//Vector3 NullField(Particles &p, int num);

struct ParticleInfo {
  Vector3 position;
  Vector3 speed;
  float size;
  float mass;
  float r, g, b, a;
  double timestamp;
//  bool active;
};

struct Particles {
  int maxnum, num;
  ParticleInfo *parts;
  Field *force;
  Field *wind;

  float friction;

  Particles(int _num);
  void add(const Vector3 &p, const Vector3 &v, float s, float m, float r, float g, float b, float a, double timestamp);
  void remove(int n);

  void setFriction(float f) { friction = f; }
  float getFriction() { return friction; }

  void setForceField(Field *f) { force = f; }
  Field *getForceField() { return force; }

  void setWind(Field *f) { wind = f; }
  Field *getWind() { return wind; }

  void move(float dt);

};

#endif // _PARTICLES_H_