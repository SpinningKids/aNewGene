
#include "Particles.h"

Vector3 NullForceField(Particles &p, int num) {
  return zero3;
}

Particles::Particles(int _num) : force(nullptr), wind(nullptr) {
    parts.reserve(_num);
}

void Particles::add(const Vector3 &p, const Vector3 &v, float s, float m, float r, float g, float b, float a, double timestamp) {
    parts.push_back({ p, v, s, m, r, g, b, a, timestamp });
}

void Particles::move(float dt) {
    for (auto &p: parts) {
        Vector3 a(0, 0, 0);
        if (force) a += (*force)(p);
        Vector3 s(p.speed);
        if (wind) s -= (*wind)(p);
        a -= (friction * 6.28f * p.size * p.size) * s;
        a *= dt / p.mass;
        p.position += (0.5 * a + p.speed) * dt;
        p.speed += a;
    }
}
