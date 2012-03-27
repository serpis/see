#ifndef QUAT_H
#define QUAT_H
#include <iostream>   // stream library

class quat
{
  friend int operator==(const quat& a, const quat& b);
  friend int operator!=(const quat& a, const quat& b);
  friend quat operator-(quat& x);
  friend quat operator+(const quat& x, const quat& y);
  friend quat operator-(const quat& x, const quat& y);
  friend quat operator*(const quat& x, const float y);
  friend quat operator*(const quat& x, const quat& y);
  friend quat operator/(const quat& x, const quat& y);
  friend quat sqrt(const quat& x);
  friend std::ostream& operator<<(std::ostream& ost, const quat& x);
private:
  float r;
  float i;
  float j;
  float k;
public:
  quat() {}                   // uninitialized

  quat(float rr) : r(rr),i(0),j(0),k(0) {}

  quat( float, float, float, float );
  quat(const quat& b): r(b.r),i(b.i),j(b.j),k(b.k) {} // copy constructor

  float R() const { return r ;}
  float I() const { return i ;}
  float J() const { return j ;}
  float K() const { return k ;}

  float norm() const;
  float abs() const;

  quat& operator=(const quat& b) {r=b.r; i=b.i; j=b.j; k=b.k; return *this;}

  quat& negate();
  quat& conjugate();
  quat& square();

  quat& invert();

  quat& operator+=(const quat& b);
  quat& operator-=(const quat& b);
  quat& operator*=(const float b);
  quat& operator/=(const float b);
  quat& operator*=(const quat& b);
  quat& operator/=(const quat& b);
};

inline quat operator+(const quat& x, const quat& y)
{
  quat z = x;
  return z += y; // derived operator
}

inline quat operator-(const quat& x, const quat& y)
{
  quat z = x;
  return z -= y; // derived operator
}

inline quat operator*(const quat& x, const float y)
{
  quat z = x;
  return z *= y; // derived operator
}

inline quat operator*(const quat& x, const quat& y)
{
  quat z = x;
  return z *= y; // derived operator
}

inline quat operator/(const quat& x, const quat& y)
{
  quat z = x;
  return z /= y; // derived operator
}

#endif
