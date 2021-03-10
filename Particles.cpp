
#include "Particles.h"
#include "noise.h"

Vector3 NullForceField(Particles &p, int num) {
  return zero3;
}

Particles::Particles(int _num) : maxnum(_num), num(0), parts(new ParticleInfo[_num]), force(0), wind(0) {
}

void Particles::add(const Vector3 &p, const Vector3 &v, float s, float m, float r, float g, float b, float a, double timestamp) {
  if (num < maxnum) {
    parts[num].position = p;
    parts[num].speed = v;
    parts[num].size = s;
    parts[num].mass = m;
    parts[num].r = r;
    parts[num].g = g;
    parts[num].b = b;
    parts[num].a = a;
    parts[num].timestamp = timestamp;
  }
  num++;
}

void Particles::remove(int n) {
  for(int i = n+1; i < num; i++)
    parts[i-1] = parts[i];
  num--;
} 

void Particles::move(float dt) {
  for(int i = 0; i < num; i++) {
//      Vector3 a(1.4/(1.f+pn.y*pn.y)+vnoise(pn.y, pn.z), vnoise(pn.z, pn.x), vnoise(pn.x, pn.y));
    Vector3 a(0,0,0);
    if (force) a += (*force)(*this, i);
    Vector3 s(parts[i].speed);
    if (wind) s -= (*wind)(*this, i);
    a -= (friction*6.28f*parts[i].size*parts[i].size)*s;
    a *= dt/parts[i].mass;     
    parts[i].position += (0.5*a+parts[i].speed)*dt;
    parts[i].speed += a;
  }
}
