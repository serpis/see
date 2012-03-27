#include <math.h>
#include <string.h>

#include "math.hpp"

/*
vec2_t::vec2_t()
	: x(0.0f), y(0.0f)
{
}

vec2_t::vec2_t(const vec2_t &v)
	: x(v.x), y(v.y)
{
}

vec2_t::vec2_t(float _x, float _y)
	: x(_x), y(_y)
{
}

vec2_t vec2_t::operator-() const
{
	return -1.0f*(*this);
}

vec2_t vec2_t::operator+(const vec2_t &v) const
{
	return vec2_t(x+v.x, y+v.y);
}

vec2_t vec2_t::operator-(const vec2_t &v) const
{
	return vec2_t(x-v.x, y-v.y);
}

float vec2_t::dot(const vec2_t &v) const
{
	return x*v.x + y*v.y;
}

vec2_t vec2_t::operator*(float s) const
{
	return vec2_t(s*x, s*y);
}

vec2_t vec2_t::operator*(const vec2_t &v) const
{
	return vec2_t(x*v.x, y*v.y);
}

vec2_t &vec2_t::operator+=(const vec2_t &v)
{
	*this = *this + v;
	return *this;
}

vec2_t &vec2_t::operator-=(const vec2_t &v)
{
	*this = *this - v;
	return *this;
}

bool vec2_t::operator==(const vec2_t &v)
{
	return x == v.x && y == v.y;
}

float vec2_t::length() const
{
	return sqrtf(this->dot(*this));
}

vec2_t vec2_t::normalized() const
{
	return (1.0f/length())*(*this);
}

std::ostream &operator<<(std::ostream &os, const vec2_t &v)
{
	os << "(" << v.x << ", " << v.y << ")";

	return os;
}

std::istream &operator>>(std::istream &is, vec2_t &v)
{
	is >> v.x >> v.y;
	return is;
}


vec2_t operator*(float s, const vec2_t &v)
{
	return v*s;
}

vec3_t::vec3_t()
	: x(0.0f), y(0.0f), z(0.0f)
{
}

vec3_t::vec3_t(const vec3_t &v)
	: x(v.x), y(v.y), z(v.z)
{
}

vec3_t::vec3_t(const vec2_t &v, float _z)
	: x(v.x), y(v.y), z(_z)
{
}

vec3_t::vec3_t(float _x, float _y, float _z)
	: x(_x), y(_y), z(_z)
{
}

vec3_t vec3_t::operator-() const
{
	return -1.0f*(*this);
}

vec3_t vec3_t::operator+(const vec3_t &v) const
{
	return vec3_t(x+v.x, y+v.y, z+v.z);
}
vec3_t vec3_t::operator-(const vec3_t &v) const
{
	return vec3_t(x-v.x, y-v.y, z-v.z);
}
float vec3_t::dot(const vec3_t &v) const
{
	return x*v.x + y*v.y + z*v.z;
}

vec3_t vec3_t::cross(const vec3_t &v) const
{
	return vec3_t(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

vec3_t vec3_t::operator*(float s) const
{
	return vec3_t(s*x, s*y, s*z);
}

vec3_t vec3_t::operator/(float s) const
{
	return vec3_t(x/s, y/s, z/s);
}

vec3_t vec3_t::operator*(const vec3_t &v) const
{
	return vec3_t(x*v.x, y*v.y, z*v.z);
}

vec3_t &vec3_t::operator+=(const vec3_t &v)
{
	*this = *this + v;
	return *this;
}
vec3_t &vec3_t::operator-=(const vec3_t &v)
{
	*this = *this - v;
	return *this;
}

float vec3_t::length() const
{
	return sqrtf(this->dot(*this));
}

vec3_t vec3_t::normalized() const
{
	return (1.0f/length())*(*this);
}

vec2_t vec3_t::xy() const
{
	return vec2_t(x, y);
}

vec2_t vec3_t::yz() const
{
	return vec2_t(y, z);
}

vec2_t vec3_t::xz() const
{
	return vec2_t(x, z);
}

std::ostream &operator<<(std::ostream &os, const vec3_t &v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	return os;
}

std::istream &operator>>(std::istream &is, vec3_t &v)
{
	is >> v.x >> v.y >> v.z;
	return is;
}

vec3_t operator*(float s, const vec3_t &v)
{
	return v*s;
}

vec3_t vec3_t::reflect(const vec3_t &normal) const
{
	return *this - 2.0f * normal.dot(*this) * normal;
}

vec3_t vec3_t::refract(const vec3_t &normal, float eta) const
{
	float k = 1.0f - eta * eta * (1.0f - normal.dot(*this) * normal.dot(*this));

	if (k < 0.0f)
	{
		return vec3_t(0.0f, 0.0f, 0.0f);
	}
	else
	{
		return eta * *this - (eta * normal.dot(*this) + sqrtf(k)) * normal;
	}
}

vec4_t::vec4_t()
	: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}

vec4_t::vec4_t(const vec4_t &v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{
}

vec4_t::vec4_t(const vec3_t &v, float _w)
	: x(v.x), y(v.y), z(v.z), w(_w)
{
}

vec4_t::vec4_t(float _x, float _y, float _z, float _w)
	: x(_x), y(_y), z(_z), w(_w)
{
}

vec4_t vec4_t::operator-() const
{
	return -1.0f*(*this);
}

vec4_t vec4_t::operator+(const vec4_t &v) const
{
	return vec4_t(x+v.x, y+v.y, z+v.z, w+v.w);
}
vec4_t vec4_t::operator-(const vec4_t &v) const
{
	return vec4_t(x-v.x, y-v.y, z-v.z, w-v.w);
}

vec4_t vec4_t::operator*(float s) const
{
	return vec4_t(s*x, s*y, s*z, s*w);
}

vec4_t &vec4_t::operator+=(const vec4_t &v)
{
	*this = *this + v;
	return *this;
}
vec4_t &vec4_t::operator-=(const vec4_t &v)
{
	*this = *this - v;
	return *this;
}

float vec4_t::dot(const vec4_t &v) const
{
	return x*v.x + y*v.y + z*v.z + w*v.w;
}
float vec4_t::length() const
{
	return sqrtf(this->dot(*this));
}

vec4_t vec4_t::normalized() const
{
	return (1.0f/length())*(*this);
}

vec3_t vec4_t::xyz() const
{
	return vec3_t(x, y, z);
}

std::ostream &operator<<(std::ostream &os, const vec4_t &v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";

	return os;
}

vec4_t operator*(float s, const vec4_t &v)
{
	return v*s;
}

mat4_t::mat4_t()
{
	memset(c, 0, sizeof(c));
	c[0] = c[5] = c[10] = c[15] = 1;
}

mat4_t::mat4_t(const mat4_t &m)
{
	memcpy(c, m.c, sizeof(c));
}

mat4_t mat4_t::operator*(const mat4_t &m) const
{
	mat4_t res = zeros();
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
vec4_t mat4_t::operator*(const vec4_t &v) const
{
	vec4_t res;
	res.x = c[0]*v.x + c[4]*v.y + c[8]*v.z + c[12]*v.w;
	res.y = c[1]*v.x + c[5]*v.y + c[9]*v.z + c[13]*v.w;
	res.z = c[2]*v.x + c[6]*v.y + c[10]*v.z + c[14]*v.w;
	res.w = c[3]*v.x + c[7]*v.y + c[11]*v.z + c[15]*v.w;
	return res;
}

mat4_t &mat4_t::operator*=(const mat4_t &m)
{
	*this = *this * m;
	return *this;
}

mat4_t mat4_t::operator*(float s) const
{
	mat4_t m = *this;
	for (int i = 0; i < 16; i++)
		m.c[i] *= s;
	return m;
}

mat4_t mat4_t::transposed() const
{
	mat4_t m;
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 4; i++)
		{
			m.c[i * 4 + j] = this->c[j * 4 + i];
		}
	}

	return m;
}

mat4_t mat4_t::zeros()
{
	mat4_t m;
	memset(m.c, 0, sizeof(m.c));
	return m;
}

mat4_t mat4_t::identity()
{
	mat4_t m = zeros();
	m.c[0] = 1.0f;
	m.c[5] = 1.0f;
	m.c[10] = 1.0f;
	m.c[15] = 1.0f;
	return m;
}

mat4_t mat4_t::perspective(float fovy, float aspect, float near, float far)
{
	float f = 1/tanf(fovy/2);

	mat4_t m = zeros();

	m.c[0] = f/aspect;

	m.c[5] = f;

	m.c[10] = (far+near)/(near-far);
	m.c[11] = -1.0f;

	m.c[14] = 2*(far*near)/(near-far);

	return m;
}

mat4_t mat4_t::orthogonal(float left, float right, float bottom, float top, float near, float far)
{
	mat4_t m = zeros();

	m.c[0] = 2/(right-left);

	m.c[5] = 2/(top-bottom);

	m.c[10] = -2/(far-near);

	m.c[12] = -(right + left)/(right - left);
	m.c[13] = -(top + bottom)/(top - bottom);
	m.c[14] = -(far + near)/(far - near);
	m.c[15] = 1;

	return m;
}

mat4_t mat4_t::lookat(const vec3_t &pos, const vec3_t &target, const vec3_t &up)
{
	vec3_t f = (target-pos).normalized();
	vec3_t up_prim = up.normalized();

	vec3_t s = f.cross(up_prim);
	vec3_t u = s.cross(f);

	mat4_t m = zeros();

	m.c[0] = s.x;
	m.c[1] = u.x;
	m.c[2] = -f.x;

	m.c[4] = s.y;
	m.c[5] = u.y;
	m.c[6] = -f.y;

	m.c[8] = s.z;
	m.c[9] = u.z;
	m.c[10] = -f.z;

	m.c[15] = 1.0f;

	return m*translation(-pos);
}

mat4_t mat4_t::rotation(float angle, const vec3_t &axis)
{
	float c = cosf(angle);
	float s = sinf(angle);

	float x = axis.x;
	float y = axis.y;
	float z = axis.z;

	mat4_t m = zeros();

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

mat4_t mat4_t::translation(const vec3_t &offset)
{
	mat4_t m = identity();

	m.c[12] = offset.x;
	m.c[13] = offset.y;
	m.c[14] = offset.z;

	return m;
}

mat4_t mat4_t::scale(const vec3_t &s)
{
	mat4_t m = identity();

	m.c[0] = s.x;
	m.c[5] = s.y;
	m.c[10] = s.z;

	return m;
}

mat4_t mat4_t::place_object(const vec3_t &pos, const vec3_t &dir)
{
	mat4_t m = identity();

	vec3_t forward = dir.normalized();
	vec3_t right = forward.cross(vec3_t(0.0f, 1.0f, 0.0f)).normalized();
	vec3_t up = right.cross(forward);

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

std::ostream &operator<<(std::ostream &os, const mat4_t &m)
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

mat4_t operator*(float s, const mat4_t &m)
{
	return m*s;
}
*/

