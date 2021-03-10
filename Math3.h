
#ifndef __MATH3_H__
#define __MATH3_H__

#include <math.h>

struct Vector3;
struct Matrix3;

#define Colour Vector3
#define red xversor
#define green yversor
#define blue zversor

extern "C" const Vector3 zero3;
extern "C" const Vector3 xversor;
extern "C" const Vector3 yversor;
extern "C" const Vector3 zversor;
extern "C" const Matrix3 identity3;
extern "C" const Vector3 white;

inline float invsqrt(float x) {
  return 1.0f/sqrtf(x);
}

#define TWOPI           6.2831853071795f
#define HALFPI			1.5707963267948f
#define PI				3.1415926535897932384626433832795f
#define PIOVER180		0.0174532925199432957692369076848861f
#define PIUNDER180		57.2957795130823208767981548141052f
#define TWOPI10			62.831853071795f
#define DTOR            0.0174532925f
#define RTOD            57.2957795f

struct Matrix3 {

  float a, b, c, d, e, f, g, h, i;

  Matrix3() {}

  Matrix3(Matrix3 const &m) : a(m.a), b(m.b), c(m.c), d(m.d), e(m.e), f(m.f), g(m.g), h(m.h), i(m.i) {
  }

  Matrix3(float _a, float _b, float _c, float _d, float _e, float _f, float _g, float _h, float _i) : a(_a), b(_b), c(_c), d(_d), e(_e), f(_f), g(_g), h(_h), i(_i) {
  }

  Matrix3 &operator = (Matrix3 const &m) {
    a = m.a;
    b = m.b;
    c = m.c;
    d = m.d;
    e = m.e;
    f = m.f;
    g = m.g;
    h = m.h;
    i = m.i;
    return(* this);
  }

  Matrix3 &operator += (Matrix3 const &m) {
    a += m.a;
    b += m.b;
    c += m.c;
    d += m.d;
    e += m.e;
    f += m.f;
    g += m.g;
    h += m.h;
    i += m.i;
    return(* this);
  }

  Matrix3 &operator -= (Matrix3 const &m) {
    a -= m.a;
    b -= m.b;
    c -= m.c;
    d -= m.d;
    e -= m.e;
    f -= m.f;
    g -= m.g;
    h -= m.h;
    i -= m.i;
    return(* this);
  }

  Matrix3 &operator *= (float fact) {
    a *= fact;
    b *= fact;
    c *= fact;
    d *= fact;
    e *= fact;
    f *= fact;
    g *= fact;
    h *= fact;
    i *= fact;
    return(* this);
  }

  Matrix3 &operator *= (Matrix3 const &m) {
    register float t1, t2;

    a = (t1 = a) * m.a + b * m.d + c * m.g;
    b = t1 * m.b + (t2 = b) * m.e + c * m.h;
    c = t1 * m.c + t2 * m.f + c * m.i;

    d = (t1 = d) * m.a + e * m.d + f * m.g;
    e = t1 * m.b + (t2 = e) * m.e + f * m.h;
    f = t1 * m.c + t2 * m.f + f * m.i;

    g = (t1 = g) * m.a + h * m.d + i * m.g;
    h = t1 * m.b + (t2 = h) * m.e + i * m.h;
    i = t1 * m.c + t2 * m.f + i * m.i;

    return(* this);
  }

  Matrix3 &operator /= (float fact) {
    fact = 1.0f/fact;
    a *= fact;
    b *= fact;
    c *= fact;
    d *= fact;
    e *= fact;
    f *= fact;
    g *= fact;
    h *= fact;
    i *= fact;
    return(* this);
  }

  Matrix3 operator + () const {
    return(* this);
  }

  Matrix3 operator - () const {
    return Matrix3(-a, -b, -c, -d, -e, -f, -g, -h, -i);
  }

};

struct Vector3 {
  float x, y, z;

  Vector3() {
  }

  Vector3(Vector3 const &v) : x(v.x), y(v.y), z(v.z) {}

  Vector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}

  void normalize() {
    register float d = invsqrt(x*x+y*y+z*z);
    x *= d;
    y *= d;
    z *= d;
  }

  Vector3 &operator = (Vector3 const &v) {
    x = v.x;
  	y = v.y;
    z = v.z;
    return(*this);
  }

  Vector3 &operator += (Vector3 const &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return(*this);
  }

  Vector3 &operator -= (Vector3 const &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return(*this);
  }

  Vector3 &operator ^= (Vector3 const &v) {
    register float cy;
    register float cx = x;
    x = (cy = y) * v.z - v.y * z;
    y = v.x * z - cx * v.z;
    z = cx * v.y - v.x * cy;
    return(*this);
  }

  Vector3 &operator *= (float d) {
    x *= d;
    y *= d;
    z *= d;
    return(*this);
  }

  Vector3 &operator *= (Matrix3 const &m) {
    register float cx, cy;
    x *= m.a*(cx = x) + m.d*y + m.g*z;
    y *= m.b*cx + m.e*(cy = y) + m.h*z;
    z *= m.c*cx + m.f*cy + m.i*z;
    return(*this);
  }

  Vector3 &operator /= (float d) {
    d = 1.0f/d;
    x *= d;
    y *= d;
    z *= d;
    return(*this);
  }

  Vector3 operator + () const {
    return(*this);
  }

  Vector3 operator - () const {
    return Vector3(-x, -y, -z);
  }
};

inline Vector3 operator + (Vector3 const &v1, Vector3 const &v2) {
  return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline Vector3 operator - (Vector3 const &v1, Vector3 const &v2) {
  return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

//cross
inline Vector3 operator ^ (Vector3 const &v1, Vector3 const &v2) {
  return Vector3(
    v1.y * v2.z - v1.z * v2.y,
    v1.z * v2.x - v1.x * v2.z,
    v1.x * v2.y - v1.y * v2.x);
}

//dot
inline float operator * (Vector3 const &v1, Vector3 const &v2) {
  return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

inline Vector3 operator * (Vector3 const &v, float d) {
  return Vector3(v.x * d, v.y * d, v.z * d);
}

inline Vector3 operator * (float d, Vector3 const &v) {
  return Vector3(v.x * d, v.y * d, v.z * d);
}

inline Vector3 operator / (Vector3 const &v, float d) {
  d = 1.0f/d;
  return Vector3(v.x * d, v.y * d, v.z * d);
}

inline Vector3 operator && (Vector3 const &v1, Vector3 const &v2) {
  return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline bool operator == (Vector3 const &v1, Vector3 const &v2) {
  return(v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
}

inline bool operator != (Vector3 const &v1, Vector3 const &v2) {
  return(v1.x != v2.x || v1.y != v2.y || v1.z != v2.z);
}

inline float vsqr(Vector3 const &v) {
  return v.x*v.x+v.y*v.y+v.z*v.z;
}

inline float abs(Vector3 const &v) {
  return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
}

inline Vector3 normalized(Vector3 const &v) {
  register float d = invsqrt(v.x*v.x+v.y*v.y+v.z*v.z);
  return Vector3(v.x*d, v.y*d,v.z*d);
}

inline Matrix3 operator + (Matrix3 const &m1, Matrix3 const &m2) {
  return Matrix3(
    m1.a + m2.a, m1.b + m2.b, m1.c + m2.c,
    m1.d + m2.d, m1.e + m2.e, m1.f + m2.f,
    m1.g + m2.g, m1.h + m2.h, m1.i + m2.i);
}

inline Matrix3 operator - (Matrix3 const &m1, Matrix3 const &m2) {
  return Matrix3(
    m1.a - m2.a, m1.b - m2.b, m1.c - m2.c,
    m1.d - m2.d, m1.e - m2.e, m1.f - m2.f,
    m1.g - m2.g, m1.h - m2.h, m1.i - m2.i);
}

inline Matrix3 operator * (Matrix3 const &m1, Matrix3 const &m2) {
  return Matrix3(
    m1.a * m2.a + m1.b * m2.d + m1.c * m2.g,
    m1.a * m2.b + m1.b * m2.e + m1.c * m2.h,
    m1.a * m2.c + m1.b * m2.f + m1.c * m2.i,
    m1.d * m2.a + m1.e * m2.d + m1.f * m2.g,
    m1.d * m2.b + m1.e * m2.e + m1.f * m2.h,
    m1.d * m2.c + m1.e * m2.f + m1.f * m2.i,
    m1.g * m2.a + m1.h * m2.d + m1.i * m2.g,
    m1.g * m2.b + m1.h * m2.e + m1.i * m2.h,
    m1.g * m2.c + m1.h * m2.f + m1.i * m2.i);
}

inline Matrix3 operator * (Matrix3 const &m, float fact) {
  return Matrix3(
    m.a * fact, m.b * fact, m.c * fact,
    m.d * fact, m.e * fact, m.f * fact,
    m.g * fact, m.h * fact, m.i * fact);
}

inline Matrix3 operator * (float fact, Matrix3 const &m) {
  return Matrix3(
    m.a * fact, m.b * fact, m.c * fact,
    m.d * fact, m.e * fact, m.f * fact,
    m.g * fact, m.h * fact, m.i * fact);
}

inline Vector3 operator * (Matrix3 const &m1, Vector3 const &m2) {
  return Vector3(
    m1.a * m2.x + m1.b * m2.y + m1.c * m2.z,
    m1.d * m2.x + m1.e * m2.y + m1.f * m2.z,
    m1.g * m2.x + m1.h * m2.y + m1.i * m2.z);
}

inline Vector3 operator * (Vector3 const &m1, Matrix3 const &m2) {
  return Vector3(
    m1.x * m2.a + m1.y * m2.d + m1.z * m2.g,
    m1.x * m2.b + m1.y * m2.e + m1.z * m2.h,
    m1.x * m2.c + m1.y * m2.f + m1.z * m2.i);
}

inline Matrix3 operator / (Matrix3 const &m, float fact) {
  fact = 1.0f/fact;
  return Matrix3(
    m.a * fact, m.b * fact, m.c * fact,
    m.d * fact, m.e * fact, m.f * fact,
    m.g * fact, m.h * fact, m.i * fact);
}

inline bool operator == (Matrix3 const &m1, Matrix3 const &m2) {
  return(
    m1.a == m2.a && m1.b == m2.b && m1.c == m2.c &&
    m1.d == m2.d && m1.e == m2.e && m1.f == m2.f &&
    m1.g == m2.g && m1.h == m2.h && m1.i == m2.i);
}

inline bool operator != (Matrix3 const &m1, Matrix3 const &m2) {
  return(
    m1.a != m2.a || m1.b != m2.b || m1.c != m2.c ||
    m1.d != m2.d || m1.e != m2.e || m1.f != m2.f ||
    m1.g != m2.g || m1.h != m2.h || m1.i != m2.i);
}

#endif
