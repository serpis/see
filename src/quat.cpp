#include     <iostream>       // stream library
#include     <cmath>           // math routines
#include     <cstdlib>         // standard lib for exit() function

#include "quat.h"

using namespace std;

quat::quat( float rr, float ii, float jj, float kk )
: r(rr),i(ii),j(jj),k(kk)
{}

float quat::norm() const
{
  return r*r+i*i+j*j+k*k;
}

float quat::abs() const
{
  return sqrt(norm());
}

quat& quat::negate()
{
  r = -r;
  i = -i;
  j = -j;
  k = -k;
  return *this;
}

quat& quat::conjugate()
{
  i = -i;
  j = -j;
  k = -k;
  return *this;
}

quat& quat::square()
{
  float temp = 2*r;
  r = r*r-(i*i+j*j+k*k);
  i *= temp;
  j *= temp;
  k *= temp;
  return *this;
}

quat& quat::invert()
{
  float temp = norm();
  r /= temp;
  i /= -temp;
  j /= -temp;
  k /= -temp;
  return (*this);       // Okay, same norm.
}

quat& quat::operator+=(const quat& b)
{
  r+=b.r; i+=b.i; j+=b.j; k+=b.k; return *this;
}

quat& quat::operator-=(const quat& b)
{
  r-=b.r; i-=b.i; j-=b.j; k-=b.k; return *this;
}

quat& quat::operator*=(const float b)
{
  r*=b; i*=b; j*=b; k*=b; return *this;
}

quat& quat::operator/=(const float b)
{
  r/=b; i/=b; j/=b; k/=b; return *this;
}

quat& quat::operator*=(const quat& b)
{
  float t0 = (k-j)*(b.j-b.k);
  float t1 = (r+i)*(b.r+b.i);
  float t2 = (r-i)*(b.j+b.k);
  float t3 = (k+j)*(b.r-b.i);
  float t4 = (k-i)*(b.i-b.j);
  float t5 = (k+i)*(b.i+b.j);
  float t6 = (r+j)*(b.r-b.k);
  float t7 = (r-j)*(b.r+b.k);
  float t8 = t5+t6+t7;
  float t9 = (t4+t8)/2;
  r = t0+t9-t5;
  i = t1+t9-t8;
  j = t2+t9-t7;
  k = t3+t9-t6;
  return *this;
}

quat& quat::operator/=(const quat& b)
{
  quat temp = b;
  return (*this)*=(temp.invert());      // Ugh!
}

int operator==(const quat& a, const quat& b)
{
  return (a.r==b.r && a.i==b.i && a.j==b.j && a.k==b.k);
}

int operator!=(const quat& a, const quat& b)
{
  return !(a==b); // derived operator
}

quat operator-(quat& x)
{
  return x.negate();
}

quat sqrt(const quat& x)
{
  cerr<<"sqrt(const quat&) undefined because quat::sqrt() undefined."<<endl;
  exit(1);      // Should be derived from member function.
}

ostream& operator<<(ostream& ost, const quat& x)
{
  return ost<<"( "<<x.r<<", "<<x.i<<", "<<x.j<<", "<<x.k<<" )";
}

