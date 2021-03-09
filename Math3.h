
#ifndef __MATH3_H__
#define __MATH3_H__

#include <math.h>

#pragma pack(push, 1)
//#ifdef __WATCOMC__
  #define MATH3_BASE_ float
//#else
//  #ifdef  _MSC_VER
//    #define MATH3_BASE_ double
//  #else
//    #define MATH3_BASE_ float
//  #endif
//#endif

#ifndef MATH3_BASE
#define MATH3_BASE MATH3_BASE_
#endif

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

#if defined(__WATCOMC__) && defined(_M_IX86) && defined(MATH3_USEASM)
extern "C" const float sqrt_tbl[1024];
extern "C" void makeSqrtTbl();
#define unoemezzo 1.5
inline float invsqrt(const float &x) {
  float x2;
  *(unsigned int *) &x2 = (*(unsigned int *) &x)-0x00800000;
  float register x3 = sqrt_tbl[(*(unsigned int *) &x) >> 21];
  x3 *= (unoemezzo-x2*x3*x3);
  return  x3 * (unoemezzo-x2*x3*x3);
}
#pragma library ("pan.lib");
#else
inline MATH3_BASE invsqrt(MATH3_BASE x) {
  return (MATH3_BASE)(1.0/sqrt(x));
}
#endif

#define TWOPI           6.2831853071795
#define HALFPI			1.5707963267948
#define PI				3.1415926535897932384626433832795f
#define PIOVER180		0.0174532925199432957692369076848861f
#define PIUNDER180		57.2957795130823208767981548141052f
#define TWOPI10			62.831853071795
#define DTOR            0.0174532925
#define RTOD            57.2957795

//const	float piover180 = 0.0174532925f;


struct Matrix3 {

  MATH3_BASE a, b, c, d, e, f, g, h, i;

  Matrix3() {
  }

  Matrix3(Matrix3 const &m) : a(m.a), b(m.b), c(m.c), d(m.d), e(m.e), f(m.f), g(m.g), h(m.h), i(m.i) {
  }

  Matrix3(MATH3_BASE _a, MATH3_BASE _b, MATH3_BASE _c, MATH3_BASE _d, MATH3_BASE _e, MATH3_BASE _f, MATH3_BASE _g, MATH3_BASE _h, MATH3_BASE _i) : a(_a), b(_b), c(_c), d(_d), e(_e), f(_f), g(_g), h(_h), i(_i) {
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

  Matrix3 &operator *= (MATH3_BASE fact) {
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
    register MATH3_BASE t1, t2;

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

  Matrix3 &operator /= (MATH3_BASE fact) {
    fact = (MATH3_BASE)(1.0)/fact;
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
  MATH3_BASE x, y, z;

  Vector3() {
  }

  Vector3(Vector3 const &v) : x(v.x), y(v.y), z(v.z) {}

  Vector3(MATH3_BASE ax, MATH3_BASE ay, MATH3_BASE az) : x(ax), y(ay), z(az) {}

  void normalize() {
    register MATH3_BASE d = invsqrt(x*x+y*y+z*z);
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
    register MATH3_BASE cy;
    register MATH3_BASE cx = x;
    x = (cy = y) * v.z - v.y * z;
    y = v.x * z - cx * v.z;
    z = cx * v.y - v.x * cy;
    return(*this);
  }

  Vector3 &operator *= (MATH3_BASE d) {
    x *= d;
    y *= d;
    z *= d;
    return(*this);
  }

  Vector3 &operator *= (Matrix3 const &m) {
    register MATH3_BASE cx, cy;
    x *= m.a*(cx = x) + m.d*y + m.g*z;
    y *= m.b*cx + m.e*(cy = y) + m.h*z;
    z *= m.c*cx + m.f*cy + m.i*z;
    return(*this);
  }

  Vector3 &operator /= (MATH3_BASE d) {
    d = (MATH3_BASE)(1.0)/d;
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

#pragma pack(pop)

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
inline MATH3_BASE operator * (Vector3 const &v1, Vector3 const &v2) {
  return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

inline Vector3 operator * (Vector3 const &v, MATH3_BASE d) {
  return Vector3(v.x * d, v.y * d, v.z * d);
}

inline Vector3 operator * (MATH3_BASE d, Vector3 const &v) {
  return Vector3(v.x * d, v.y * d, v.z * d);
}

inline Vector3 operator / (Vector3 const &v, MATH3_BASE d) {
  d = (MATH3_BASE)(1.0)/d;
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

inline MATH3_BASE vsqr(Vector3 const &v) {
  return v.x*v.x+v.y*v.y+v.z*v.z;
}

inline MATH3_BASE abs(Vector3 const &v) {
  return (MATH3_BASE)sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

inline Vector3 normalized(Vector3 const &v) {
  register MATH3_BASE d = invsqrt(v.x*v.x+v.y*v.y+v.z*v.z);
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

inline Matrix3 operator * (Matrix3 const &m, MATH3_BASE fact) {
  return Matrix3(
    m.a * fact, m.b * fact, m.c * fact,
    m.d * fact, m.e * fact, m.f * fact,
    m.g * fact, m.h * fact, m.i * fact);
}

inline Matrix3 operator * (MATH3_BASE fact, Matrix3 const &m) {
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

inline Matrix3 operator / (Matrix3 const &m, MATH3_BASE fact) {
  fact = (MATH3_BASE)1.0/fact;
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
