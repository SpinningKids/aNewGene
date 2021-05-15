
#include "Particles.h"

Vector3 NullForceField(Particles &p, int num) {
  return zero3;
}

Particles::Particles(int _num) : maxnum(_num), num(0), parts(new ParticleInfo[_num]), force(nullptr), wind(nullptr) {
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

void Particles::move(float dt) {
    for (ParticleInfo* p = parts; p != parts + num; ++p) {
        Vector3 a(0, 0, 0);
        if (force) a += (*force)(*p);
        Vector3 s(p->speed);
        if (wind) s -= (*wind)(*p);
        a -= (friction * 6.28f * p->size * p->size) * s;
        a *= dt / p->mass;
        p->position += (0.5 * a + p->speed) * dt;
        p->speed += a;
    }
}
