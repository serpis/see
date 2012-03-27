#ifndef _MATH_HPP
#define _MATH_HPP

#include <iostream>
#include <cstring>
#include <cmath>
#include <cassert>

template <class T = float>
class vec2_t
{
public:
    typedef T component_type;

	vec2_t<T>();
	vec2_t<T>(const vec2_t<T> &v);
	vec2_t<T>(T _x, T _y);

	vec2_t<T> operator-() const;
	vec2_t<T> operator+(const vec2_t<T> &v) const;
	vec2_t<T> operator-(const vec2_t<T> &v) const;
	vec2_t<T> operator*(T s) const;
	vec2_t<T> operator*(const vec2_t<T> &v) const;
	vec2_t<T> &operator+=(const vec2_t<T> &v);
	vec2_t<T> &operator-=(const vec2_t<T> &v);
	bool operator==(const vec2_t<T> &v);
	T dot(const vec2_t<T> &v) const;
	T length() const;
	vec2_t<T> normalized() const;

	union
	{
		struct
		{
			T c[2];
		};
		struct
		{
			T x, y;
		};
	};
};

template <class T>
std::ostream &operator<<(std::ostream &os, const vec2_t<T> &v);
template <class T>
std::istream &operator>>(std::istream &is, vec2_t<T> &v);
template <class T>
vec2_t<T> operator*(T s, const vec2_t<T> &v);


template <class T = float>
class vec3_t
{
public:
    typedef T component_type;

	vec3_t<T>();
	vec3_t<T>(const vec3_t<T> &v);
	vec3_t<T>(const vec2_t<T> &v, T _z);
	vec3_t<T>(T _x, T _y, T _z);

	vec3_t<T> operator-() const;
	vec3_t<T> operator+(const vec3_t<T> &v) const;
	vec3_t<T> operator-(const vec3_t<T> &v) const;
	vec3_t<T> operator*(T s) const;
	vec3_t<T> operator/(T s) const;
	vec3_t<T> operator*(const vec3_t<T> &v) const;
	vec3_t<T> &operator+=(const vec3_t<T> &v);
	vec3_t<T> &operator-=(const vec3_t<T> &v);
	T dot(const vec3_t<T> &v) const;
	vec3_t<T> cross(const vec3_t<T> &v) const;
	T length() const;
	vec3_t<T> normalized() const;

	vec2_t<T> xy() const;
	vec2_t<T> yz() const;
	vec2_t<T> xz() const;

	vec3_t<T> reflect(const vec3_t<T> &normal) const;
	vec3_t<T> refract(const vec3_t<T> &normal, float eta) const;

	union
	{
		struct
		{
			T c[3];
		};
		struct
		{
			T x, y, z;
		};
	};
};

template <class T>
std::ostream &operator<<(std::ostream &os, const vec3_t<T> &v);
template <class T>
std::istream &operator>>(std::istream &is, vec3_t<T> &v);
template <class T>
vec3_t<T> operator*(T s, const vec3_t<T> &v);

template <class T = float>
class vec4_t
{
public:
    typedef T component_type;

	vec4_t<T>();
	vec4_t<T>(const vec4_t<T> &v);
	vec4_t<T>(const vec3_t<T> &v, T _w);
	vec4_t<T>(T _x, T _y, T _z, T _w);

	vec4_t<T> operator-() const;
	vec4_t<T> operator+(const vec4_t<T> &v) const;
	vec4_t<T> operator-(const vec4_t<T> &v) const;
	vec4_t<T> operator*(T s) const;
	vec4_t<T> &operator+=(const vec4_t<T> &v);
	vec4_t<T> &operator-=(const vec4_t<T> &v);
	T dot(const vec4_t<T> &v) const;
	// vec3_t<T> cross(const vec3_t<T> &v) const;
	T length() const;
	vec4_t<T> normalized() const;

	vec2_t<T> xy() const;
	vec3_t<T> xyz() const;

	union
	{
		struct
		{
			T c[4];
		};
		struct
		{
			T x, y, z, w;
		};
	};
};

template <class T>
std::ostream &operator<<(std::ostream &os, const vec4_t<T> &v);
template <class T>
vec4_t<T> operator*(T s, const vec4_t<T> &v);

template <class T = float>
class mat4_t
{
public:
	mat4_t();
	mat4_t(const mat4_t &m);

	mat4_t operator*(const mat4_t &m) const;
	vec4_t<T> operator*(const vec4_t<T> &m) const;
	mat4_t operator*(T s) const;

	mat4_t &operator*=(const mat4_t &m);

	mat4_t transposed() const;
    mat4_t inverted() const;

	static mat4_t zeros();
	static mat4_t identity();

	static mat4_t perspective(float fovy, float aspect, float near, float far);
	static mat4_t orthogonal(float left, float right, float bottom, float top, float near, float far);

	static mat4_t lookat(const vec3_t<T> &pos, const vec3_t<T> &target, const vec3_t<T> &up);

	static mat4_t rotation(float angle, const vec3_t<T> &axis); // note: angle is in radians
	static mat4_t translation(const vec3_t<T> &offset);
	static mat4_t scale(const vec3_t<T> &s);

	// place and rotate object. dir doesn't have to be normalized
	static mat4_t place_object(const vec3_t<T> &pos, const vec3_t<T> &dir);

	union
	{
		struct
		{
			T c[16];
		};
		struct
		{
			// wat order?!!! :3
		};
	};
};

template <class T>
std::ostream &operator<<(std::ostream &os, const mat4_t<T> &m);
template <class T>
mat4_t<T> operator*(T s, const mat4_t<T> &m);

template <class T = float>
class quat_t
{
public:
    typedef T component_type;

	quat_t<T>();
	quat_t<T>(const quat_t<T> &q);
	quat_t<T>(T _x, T _y, T _z, T _w);
	quat_t<T>(const vec3_t<T> &axis, float rot);

	/*quat_t<T> operator-() const;
	quat_t<T> operator+(const quat_t<T> &v) const;
	quat_t<T> operator-(const quat_t<T> &v) const;
	quat_t<T> operator*(T s) const;
	quat_t<T> &operator+=(const quat_t<T> &v);
	quat_t<T> &operator-=(const quat_t<T> &v);*/
    quat_t<T> operator*(const quat_t<T> &q) const;
	quat_t<T> normalized() const;

    mat4_t<T> rotation_matrix() const;
    vec3_t<T> forward() const;
    vec3_t<T> up() const;
    vec3_t<T> right() const;

	union
	{
		struct
		{
			T c[4];
		};
		struct
		{
			T x, y, z, w;
		};
	};
};

template <class T>
vec2_t<T>::vec2_t()
	: x(0.0), y(0.0)
{
}

template <class T>
vec2_t<T>::vec2_t(const vec2_t<T> &v)
	: x(v.x), y(v.y)
{
}

template <class T>
vec2_t<T>::vec2_t(T _x, T _y)
	: x(_x), y(_y)
{
}

template <class T>
vec2_t<T> vec2_t<T>::operator-() const
{
	return -1.0f*(*this);
}

template <class T>
vec2_t<T> vec2_t<T>::operator+(const vec2_t<T> &v) const
{
	return vec2_t<T>(x+v.x, y+v.y);
}

template <class T>
vec2_t<T> vec2_t<T>::operator-(const vec2_t<T> &v) const
{
	return vec2_t<T>(x-v.x, y-v.y);
}

template <class T>
T vec2_t<T>::dot(const vec2_t<T> &v) const
{
	return x*v.x + y*v.y;
}

template <class T>
vec2_t<T> vec2_t<T>::operator*(T s) const
{
	return vec2_t<T>(s*x, s*y);
}

template <class T>
vec2_t<T> vec2_t<T>::operator*(const vec2_t<T> &v) const
{
	return vec2_t<T>(x*v.x, y*v.y);
}

template <class T>
vec2_t<T> &vec2_t<T>::operator+=(const vec2_t<T> &v)
{
	*this = *this + v;
	return *this;
}

template <class T>
vec2_t<T> &vec2_t<T>::operator-=(const vec2_t<T> &v)
{
	*this = *this - v;
	return *this;
}

template <class T>
bool vec2_t<T>::operator==(const vec2_t<T> &v)
{
	return x == v.x && y == v.y;
}

template <class T>
T vec2_t<T>::length() const
{
	return sqrtf(this->dot(*this));
}

template <class T>
vec2_t<T> vec2_t<T>::normalized() const
{
	return (1.0f/length())*(*this);
}

template <class T>
std::ostream &operator<<(std::ostream &os, const vec2_t<T> &v)
{
	os << "(" << v.x << ", " << v.y << ")";

	return os;
}

template <class T>
std::istream &operator>>(std::istream &is, vec2_t<T> &v)
{
	is >> v.x >> v.y;
	return is;
}


template <class T>
vec2_t<T> operator*(T s, const vec2_t<T> &v)
{
	return v*s;
}

template <class T>
vec3_t<T>::vec3_t()
	: x(0.0f), y(0.0f), z(0.0f)
{
}

template <class T>
vec3_t<T>::vec3_t(const vec3_t<T> &v)
	: x(v.x), y(v.y), z(v.z)
{
}

template <class T>
vec3_t<T>::vec3_t(const vec2_t<T> &v, T _z)
	: x(v.x), y(v.y), z(_z)
{
}

template <class T>
vec3_t<T>::vec3_t(T _x, T _y, T _z)
	: x(_x), y(_y), z(_z)
{
}

template <class T>
vec3_t<T> vec3_t<T>::operator-() const
{
	return -1.0f*(*this);
}

template <class T>
vec3_t<T> vec3_t<T>::operator+(const vec3_t<T> &v) const
{
	return vec3_t<T>(x+v.x, y+v.y, z+v.z);
}
template <class T>
vec3_t<T> vec3_t<T>::operator-(const vec3_t<T> &v) const
{
	return vec3_t<T>(x-v.x, y-v.y, z-v.z);
}
template <class T>
T vec3_t<T>::dot(const vec3_t<T> &v) const
{
	return x*v.x + y*v.y + z*v.z;
}

template <class T>
vec3_t<T> vec3_t<T>::cross(const vec3_t<T> &v) const
{
	return vec3_t<T>(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

template <class T>
vec3_t<T> vec3_t<T>::operator*(T s) const
{
	return vec3_t<T>(s*x, s*y, s*z);
}

template <class T>
vec3_t<T> vec3_t<T>::operator/(T s) const
{
	return vec3_t<T>(x/s, y/s, z/s);
}

template <class T>
vec3_t<T> vec3_t<T>::operator*(const vec3_t<T> &v) const
{
	return vec3_t<T>(x*v.x, y*v.y, z*v.z);
}

template <class T>
vec3_t<T> &vec3_t<T>::operator+=(const vec3_t<T> &v)
{
	*this = *this + v;
	return *this;
}
template <class T>
vec3_t<T> &vec3_t<T>::operator-=(const vec3_t<T> &v)
{
	*this = *this - v;
	return *this;
}

template <class T>
T vec3_t<T>::length() const
{
	return sqrtf(this->dot(*this));
}

template <class T>
vec3_t<T> vec3_t<T>::normalized() const
{
	return (1.0f/length())*(*this);
}

template <class T>
vec2_t<T> vec3_t<T>::xy() const
{
	return vec2_t<T>(x, y);
}

template <class T>
vec2_t<T> vec3_t<T>::yz() const
{
	return vec2_t<T>(y, z);
}

template <class T>
vec2_t<T> vec3_t<T>::xz() const
{
	return vec2_t<T>(x, z);
}

template <class T>
std::ostream &operator<<(std::ostream &os, const vec3_t<T> &v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

template <class T>
std::istream &operator>>(std::istream &is, vec3_t<T> &v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

template <class T>
vec3_t<T> operator*(T s, const vec3_t<T> &v)
{
	return v*s;
}

template <class T>
vec3_t<T> vec3_t<T>::reflect(const vec3_t<T> &normal) const
{
	return *this - 2.0f * normal.dot(*this) * normal;
}

template <class T>
vec3_t<T> vec3_t<T>::refract(const vec3_t<T> &normal, float eta) const
{
	float k = 1.0f - eta * eta * (1.0f - normal.dot(*this) * normal.dot(*this));

	if (k < 0.0f)
	{
		return vec3_t<T>(0.0f, 0.0f, 0.0f);
	}
	else
	{
		return eta * *this - (eta * normal.dot(*this) + sqrtf(k)) * normal;
	}
}

template <class T>
vec4_t<T>::vec4_t()
	: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

template <class T>
vec4_t<T>::vec4_t(const vec4_t<T> &v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{
}

template <class T>
vec4_t<T>::vec4_t(const vec3_t<T> &v, T _w)
	: x(v.x), y(v.y), z(v.z), w(_w)
{
}

template <class T>
vec4_t<T>::vec4_t(T _x, T _y, T _z, T _w)
	: x(_x), y(_y), z(_z), w(_w)
{
}

template <class T>
vec4_t<T> vec4_t<T>::operator-() const
{
	return -1.0f*(*this);
}

template <class T>
vec4_t<T> vec4_t<T>::operator+(const vec4_t<T> &v) const
{
	return vec4_t<T>(x+v.x, y+v.y, z+v.z, w+v.w);
}
template <class T>
vec4_t<T> vec4_t<T>::operator-(const vec4_t<T> &v) const
{
	return vec4_t<T>(x-v.x, y-v.y, z-v.z, w-v.w);
}

template <class T>
vec4_t<T> vec4_t<T>::operator*(T s) const
{
	return vec4_t<T>(s*x, s*y, s*z, s*w);
}

template <class T>
vec4_t<T> &vec4_t<T>::operator+=(const vec4_t<T> &v)
{
	*this = *this + v;
	return *this;
}
template <class T>
vec4_t<T> &vec4_t<T>::operator-=(const vec4_t<T> &v)
{
	*this = *this - v;
	return *this;
}

template <class T>
T vec4_t<T>::dot(const vec4_t<T> &v) const
{
	return x*v.x + y*v.y + z*v.z + w*v.w;
}
template <class T>
T vec4_t<T>::length() const
{
	return sqrtf(this->dot(*this));
}

template <class T>
vec4_t<T> vec4_t<T>::normalized() const
{
	return (1.0f/length())*(*this);
}

template <class T>
vec2_t<T> vec4_t<T>::xy() const
{
	return vec2_t<T>(x, y);
}

template <class T>
vec3_t<T> vec4_t<T>::xyz() const
{
	return vec3_t<T>(x, y, z);
}

template <class T>
std::ostream &operator<<(std::ostream &os, const vec4_t<T> &v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";

	return os;
}

template <class T>
vec4_t<T> operator*(T s, const vec4_t<T> &v)
{
	return v*s;
}

template <class T>
mat4_t<T>::mat4_t()
{
	memset(c, 0, sizeof(c));
	c[0] = c[5] = c[10] = c[15] = T(1.0);
}

template <class T>
mat4_t<T>::mat4_t(const mat4_t<T> &m)
{
	memcpy(c, m.c, sizeof(c));
}

template <class T>
mat4_t<T> mat4_t<T>::operator*(const mat4_t<T> &m) const
{
	mat4_t<T> res = zeros();
	for (int j = 0; j < 4; j++)
	{
		for (int k = 0; k < 4; k++)
		{
			for (int i = 0; i < 4; i++)
			{
				res.c[i + j*4] += c[i + k*4]*m.c[k + j*4];
			}
		}
	}
	return res;
}
template <class T>
vec4_t<T> mat4_t<T>::operator*(const vec4_t<T> &v) const
{
	vec4_t<T> res;
	res.x = c[0]*v.x + c[4]*v.y + c[8]*v.z + c[12]*v.w;
	res.y = c[1]*v.x + c[5]*v.y + c[9]*v.z + c[13]*v.w;
	res.z = c[2]*v.x + c[6]*v.y + c[10]*v.z + c[14]*v.w;
	res.w = c[3]*v.x + c[7]*v.y + c[11]*v.z + c[15]*v.w;
	return res;
}

template <class T>
mat4_t<T> &mat4_t<T>::operator*=(const mat4_t<T> &m)
{
	*this = *this * m;
	return *this;
}

template <class T>
mat4_t<T> mat4_t<T>::operator*(T s) const
{
	mat4_t<T> m = *this;
	for (int i = 0; i < 16; i++)
		m.c[i] *= s;
	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::transposed() const
{
	mat4_t<T> m;
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			m.c[i * 4 + j] = this->c[j * 4 + i];
		}
	}

	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::inverted() const
{
    // stolen from GLUT via http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
    mat4_t<T> res;
    int i;

    res.c[0] = c[5]  * c[10] * c[15] - 
             c[5]  * c[11] * c[14] - 
             c[9]  * c[6]  * c[15] + 
             c[9]  * c[7]  * c[14] +
             c[13] * c[6]  * c[11] - 
             c[13] * c[7]  * c[10];

    res.c[4] = -c[4]  * c[10] * c[15] + 
              c[4]  * c[11] * c[14] + 
              c[8]  * c[6]  * c[15] - 
              c[8]  * c[7]  * c[14] - 
              c[12] * c[6]  * c[11] + 
              c[12] * c[7]  * c[10];

    res.c[8] = c[4]  * c[9] * c[15] - 
             c[4]  * c[11] * c[13] - 
             c[8]  * c[5] * c[15] + 
             c[8]  * c[7] * c[13] + 
             c[12] * c[5] * c[11] - 
             c[12] * c[7] * c[9];

    res.c[12] = -c[4]  * c[9] * c[14] + 
               c[4]  * c[10] * c[13] +
               c[8]  * c[5] * c[14] - 
               c[8]  * c[6] * c[13] - 
               c[12] * c[5] * c[10] + 
               c[12] * c[6] * c[9];

    res.c[1] = -c[1]  * c[10] * c[15] + 
              c[1]  * c[11] * c[14] + 
              c[9]  * c[2] * c[15] - 
              c[9]  * c[3] * c[14] - 
              c[13] * c[2] * c[11] + 
              c[13] * c[3] * c[10];

    res.c[5] = c[0]  * c[10] * c[15] - 
             c[0]  * c[11] * c[14] - 
             c[8]  * c[2] * c[15] + 
             c[8]  * c[3] * c[14] + 
             c[12] * c[2] * c[11] - 
             c[12] * c[3] * c[10];

    res.c[9] = -c[0]  * c[9] * c[15] + 
              c[0]  * c[11] * c[13] + 
              c[8]  * c[1] * c[15] - 
              c[8]  * c[3] * c[13] - 
              c[12] * c[1] * c[11] + 
              c[12] * c[3] * c[9];

    res.c[13] = c[0]  * c[9] * c[14] - 
              c[0]  * c[10] * c[13] - 
              c[8]  * c[1] * c[14] + 
              c[8]  * c[2] * c[13] + 
              c[12] * c[1] * c[10] - 
              c[12] * c[2] * c[9];

    res.c[2] = c[1]  * c[6] * c[15] - 
             c[1]  * c[7] * c[14] - 
             c[5]  * c[2] * c[15] + 
             c[5]  * c[3] * c[14] + 
             c[13] * c[2] * c[7] - 
             c[13] * c[3] * c[6];

    res.c[6] = -c[0]  * c[6] * c[15] + 
              c[0]  * c[7] * c[14] + 
              c[4]  * c[2] * c[15] - 
              c[4]  * c[3] * c[14] - 
              c[12] * c[2] * c[7] + 
              c[12] * c[3] * c[6];

    res.c[10] = c[0]  * c[5] * c[15] - 
              c[0]  * c[7] * c[13] - 
              c[4]  * c[1] * c[15] + 
              c[4]  * c[3] * c[13] + 
              c[12] * c[1] * c[7] - 
              c[12] * c[3] * c[5];

    res.c[14] = -c[0]  * c[5] * c[14] + 
               c[0]  * c[6] * c[13] + 
               c[4]  * c[1] * c[14] - 
               c[4]  * c[2] * c[13] - 
               c[12] * c[1] * c[6] + 
               c[12] * c[2] * c[5];

    res.c[3] = -c[1] * c[6] * c[11] + 
              c[1] * c[7] * c[10] + 
              c[5] * c[2] * c[11] - 
              c[5] * c[3] * c[10] - 
              c[9] * c[2] * c[7] + 
              c[9] * c[3] * c[6];

    res.c[7] = c[0] * c[6] * c[11] - 
             c[0] * c[7] * c[10] - 
             c[4] * c[2] * c[11] + 
             c[4] * c[3] * c[10] + 
             c[8] * c[2] * c[7] - 
             c[8] * c[3] * c[6];

    res.c[11] = -c[0] * c[5] * c[11] + 
               c[0] * c[7] * c[9] + 
               c[4] * c[1] * c[11] - 
               c[4] * c[3] * c[9] - 
               c[8] * c[1] * c[7] + 
               c[8] * c[3] * c[5];

    res.c[15] = c[0] * c[5] * c[10] - 
              c[0] * c[6] * c[9] - 
              c[4] * c[1] * c[10] + 
              c[4] * c[2] * c[9] + 
              c[8] * c[1] * c[6] - 
              c[8] * c[2] * c[5];

    T det = c[0] * res.c[0] + c[1] * res.c[4] + c[2] * res.c[8] + c[3] * res.c[12];

    assert(det != 0.0f);

    for (int i = 0; i < 16; i++)
    {
        res.c[i] = res.c[i] / det;
    }

    return res;
}

template <class T>
mat4_t<T> mat4_t<T>::zeros()
{
	mat4_t<T> m;
	memset(m.c, 0, sizeof(m.c));
	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::identity()
{
	mat4_t<T> m = zeros();
	m.c[0] = T(1.0);
	m.c[5] = T(1.0);
	m.c[10] = T(1.0);
	m.c[15] = T(1.0);
	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::perspective(float fovy, float aspect, float near, float far)
{
	float f = 1/tanf(fovy/2);

	mat4_t<T> m = zeros();

	m.c[0] = T(f/aspect);

	m.c[5] = T(f);

	m.c[10] = T((far+near)/(near-far));
	m.c[11] = T(-1.0);

	m.c[14] = T(2*(far*near)/(near-far));

	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::orthogonal(float left, float right, float bottom, float top, float near, float far)
{
	mat4_t<T> m = zeros();

	m.c[0] = 2/(right-left);

	m.c[5] = 2/(top-bottom);

	m.c[10] = -2/(far-near);

	m.c[12] = -(right + left)/(right - left);
	m.c[13] = -(top + bottom)/(top - bottom);
	m.c[14] = -(far + near)/(far - near);
	m.c[15] = 1;

	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::lookat(const vec3_t<T> &pos, const vec3_t<T> &target, const vec3_t<T> &up)
{
	vec3_t<T> f = (target-pos).normalized();
	vec3_t<T> up_prim = up.normalized();

	vec3_t<T> s = f.cross(up_prim);
	vec3_t<T> u = s.cross(f);

	mat4_t<T> m = zeros();

	m.c[0] = T(s.x);
	m.c[1] = T(u.x);
	m.c[2] = T(-f.x);

	m.c[4] = T(s.y);
	m.c[5] = T(u.y);
	m.c[6] = T(-f.y);

	m.c[8] = T(s.z);
	m.c[9] = T(u.z);
	m.c[10] = T(-f.z);

	m.c[15] = T(1.0);

	return m*translation(-pos);
}

template <class T>
mat4_t<T> mat4_t<T>::rotation(float angle, const vec3_t<T> &axis)
{
	float c = cosf(angle);
	float s = sinf(angle);

	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	mat4_t<T> m = zeros();

	m.c[0]  = x*x*(1-c)+c;
	m.c[1]  = y*x*(1-c)+z*s;
	m.c[2]  = x*z*(1-c)-y*s;

	m.c[4]  = x*y*(1-c)-z*s;
	m.c[5]  = y*y*(1-c)+c;
	m.c[6]  = y*z*(1-c)+x*s;

	m.c[8]  = x*z*(1-c)+y*s;
	m.c[9]  = y*z*(1-c)-x*s;
	m.c[10] = z*z*(1-c)+c;

	m.c[15] = 1;

	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::translation(const vec3_t<T> &offset)
{
	mat4_t<T> m = identity();

	m.c[12] = offset.x;
	m.c[13] = offset.y;
	m.c[14] = offset.z;

	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::scale(const vec3_t<T> &s)
{
	mat4_t<T> m = identity();

	m.c[0] = s.x;
	m.c[5] = s.y;
	m.c[10] = s.z;

	return m;
}

template <class T>
mat4_t<T> mat4_t<T>::place_object(const vec3_t<T> &pos, const vec3_t<T> &dir)
{
	mat4_t<T> m = identity();

	vec3_t<T> forward = dir.normalized();
	vec3_t<T> right = forward.cross(vec3_t<T>(0.0f, 1.0f, 0.0f)).normalized();
	vec3_t<T> up = right.cross(forward);

	m.c[0] = forward.x;
	m.c[1] = forward.y;
	m.c[2] = forward.z;

	m.c[4] = up.x;
	m.c[5] = up.y;
	m.c[6] = up.z;

	m.c[8] = right.x;
	m.c[9] = right.y;
	m.c[10] = right.z;

	m.c[12] = pos.x;
	m.c[13] = pos.y;
	m.c[14] = pos.z;

	return m;
}

template <class T>
std::ostream &operator<<(std::ostream &os, const mat4_t<T> &m)
{
	os << "(";
	// rows i, columns j
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			// indexing this way because the matrix is stored column-major
			os << m.c[i + j*4];

			if (i != 4-1 || j != 4-1)
			{
				if (j == 3)
					os << ";";
				else
					os << ",";
				os << " ";
			}
		}
	}
	os << ")";

	return os;
}

template <class T>
mat4_t<T> operator*(T s, const mat4_t<T> &m)
{
	return m*s;
}

template <class T>
quat_t<T>::quat_t()
	: x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{
}

template <class T>
quat_t<T>::quat_t(const quat_t<T> &v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{
}

template <class T>
quat_t<T>::quat_t(T _x, T _y, T _z, T _w)
	: x(_x), y(_y), z(_z), w(_w)
{
}

template <class T>
quat_t<T>::quat_t(const vec3_t<T> &axis, float rot)
    : x(axis.x * sinf(0.5f * rot)),
      y(axis.y * sinf(0.5f * rot)),
      z(axis.z * sinf(0.5f * rot)),
      w(cosf(0.5f * rot))
{
}

template <class T>
quat_t<T> quat_t<T>::operator*(const quat_t<T> &q) const
{
    //float x = w*q.x + x*q.w + y*q.z - z*q.y;
    //float y = w*q.y - x*q.z + y*q.w + z*q.x;
    //float z = w*q.z + x*q.y - y*q.x + z*q.w;
    //float w = w*q.w - x*q.x - y*q.y - z*q.z;
    //return quat_t<T>(x, y, z, w);

    float t0 = (z-y)*(q.y-q.z);
    float t1 = (w+x)*(q.w+q.x);
    float t2 = (w-x)*(q.y+q.z);
    float t3 = (z+y)*(q.w-q.x);
    float t4 = (z-x)*(q.x-q.y);
    float t5 = (z+x)*(q.x+q.y);
    float t6 = (w+y)*(q.w-q.z);
    float t7 = (w-y)*(q.w+q.z);
    float t8 = t5+t6+t7;
    float t9 = (t4+t8)/2;
    float w = t0+t9-t5;
    float x = t1+t9-t8;
    float y = t2+t9-t7;
    float z = t3+t9-t6;
    return quat_t<T>(x, y, z, w);
}


template <class T>
quat_t<T> quat_t<T>::normalized() const
{
    T len = sqrtf(x*x + y*y + z*z + w*w);
    return quat_t<T>(x / len, y / len, z / len, w / len);
}

template <class T>
mat4_t<T> quat_t<T>::rotation_matrix() const
{
    mat4_t<T> res;

    res.c[0] = 1 - 2*y*y - 2*z*z;
    res.c[1] = 2*x*y + 2*w*z;
    res.c[2] = 2*x*z - 2*w*y;
    res.c[3] = 0;

    res.c[4] = 2*x*y - 2*w*z;
    res.c[5] = 1 - 2*x*x - 2*z*z;
    res.c[6] = 2*y*z + 2*w*x;
    res.c[7] = 0;

    res.c[8] = 2*x*z + 2*w*y;
    res.c[9] = 2*y*z - 2*w*x;
    res.c[10] = 1 - 2*x*x - 2*y*y;
    res.c[11] = 0;

    res.c[12] = 0;
    res.c[13] = 0;
    res.c[14] = 0;
    res.c[15] = 1;

    return res;
}

template <class T>
vec3_t<T> quat_t<T>::forward() const
{
    return (rotation_matrix() * vec4_t<T>(1.0f, 0.0f, 0.0f, 0.0f)).xyz();
}

template <class T>
vec3_t<T> quat_t<T>::up() const
{
    return (rotation_matrix() * vec4_t<T>(0.0f, 1.0f, 0.0f, 0.0f)).xyz();
}

template <class T>
vec3_t<T> quat_t<T>::right() const
{
    return (rotation_matrix() * vec4_t<T>(0.0f, 0.0f, 1.0f, 0.0f)).xyz();
}


#endif // _MATH_HPP

