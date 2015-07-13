#pragma once

#include <math.h>
#include <assert.h>


#define ong_MAX(a,b) (a > b ? a : b)
#define ong_MIN(a,b) (a < b ? a : b)

#define ong_clamp(a,min,max) ong_MIN(ong_MAX(a,min), max)

#define ong_PI 3.14159265359f

namespace ong
{
	//row vector
	struct vec3
	{
		float x;
		float y;
		float z;

		vec3() = default;
		vec3(float x, float y, float z) : x(x), y(y), z(z){};
		inline float& operator[](int idx)
		{
			assert(idx >= 0 && idx < 3);
			return (&x)[idx];
		}
		inline const float& operator[](int idx) const
		{
			assert(idx >= 0 && idx < 3);
			return (&x)[idx];
		}

	};

	// row matrix
	struct mat3x3
	{
		vec3 r0;
		vec3 r1;
		vec3 r2;

		inline mat3x3() = default;
		inline mat3x3(const vec3& r0, const vec3& r1, const vec3&r2) : r0(r0), r1(r1), r2(r2) {};
		inline vec3& operator[](int idx)
		{
			assert(idx >= 0 && idx < 3);
			return (&r0)[idx];
		}
		inline const vec3& operator[](int idx) const
		{
			assert(idx >= 0 && idx < 3);
			return (&r0)[idx];
		}
	};


	struct Quaternion
	{
		vec3 v;
		float w;
		Quaternion() = default;
		Quaternion(const vec3& v, float w) : v(v), w(w){};
	};


	// todo use matrices for multiple transforms
	struct Transform
	{
		vec3 p;
		Quaternion q;

		Transform() = default;
		Transform(const vec3& position, const Quaternion& orientation)
			: p(position), q(orientation) {};
	};


	// -- vec3 --

	inline vec3 operator*(float lhs, const vec3& rhs)
	{
		////TODO remove after finding bug
		//assert(lhs != INFINITY && lhs != -INFINITY && lhs != NAN);
		return vec3(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs);
	}

	inline vec3 operator+(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	inline vec3& operator+=(vec3& lhs, const vec3& rhs)
	{
		lhs = lhs + rhs;
		return lhs;
	}

	inline vec3 operator-(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}


	inline vec3& operator-=(vec3& lhs, const vec3& rhs)
	{
		lhs = lhs - rhs;
		return lhs;
	}

	inline bool operator ==(const vec3& lhs, const vec3& rhs)
	{
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	inline bool operator !=(const vec3& lhs, const vec3& rhs)
	{
		return !(lhs == rhs);
	}



	inline vec3 operator-(const vec3& lhs)
	{
		return vec3(-lhs.x, -lhs.y, -lhs.z);
	}



	inline float dot(const vec3& lhs, const vec3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	inline vec3 cross(const vec3& lhs, const vec3& rhs)
	{
		return vec3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
	}

	inline float lengthSq(const vec3& v)
	{
		return dot(v, v);
	}

	inline float length(const vec3& v)
	{
		return sqrt(lengthSq(v));
	}


	inline vec3 normalize(const vec3& v)
	{
		return 1.0f / sqrt(dot(v, v)) * v;
	}


	inline vec3 hadamardProduct(const vec3& lhs, const vec3&rhs)
	{
		return vec3(lhs.x * rhs.x, lhs.y*rhs.y, lhs.z*rhs.z);
	}


	inline mat3x3 outerproduct(const vec3& lhs, const vec3& rhs)
	{
		return mat3x3(lhs.x * rhs, lhs.y*rhs, lhs.z * rhs);
	}

	// mat3x3


	inline mat3x3 identity()
	{
		return mat3x3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	}

	inline mat3x3 transpose(const mat3x3& m)
	{
		return mat3x3(
			vec3(m[0][0], m[1][0], m[2][0]),
			vec3(m[0][1], m[1][1], m[2][1]),
			vec3(m[0][2], m[1][2], m[2][2]));
	}
	inline mat3x3 operator+(const mat3x3& lhs, const mat3x3& rhs)
	{
		return mat3x3(
			lhs[0] + rhs[0],
			lhs[1] + rhs[1],
			lhs[2] + rhs[2]
			);
	}

	inline mat3x3 operator-(const mat3x3& lhs, const mat3x3& rhs)
	{
		return mat3x3(
			lhs[0] - rhs[0],
			lhs[1] - rhs[1],
			lhs[2] - rhs[2]
			);
	}


	inline mat3x3 operator*(float lhs, const mat3x3& rhs)
	{
		return mat3x3(lhs * rhs[0], lhs * rhs[1], lhs* rhs[2]);
	}



	// multiply matrix and row vector
	inline vec3 operator*(const vec3& lhs, const mat3x3& rhs)
	{
		mat3x3 rhsT = transpose(rhs);
		return vec3(dot(lhs, rhsT[0]), dot(lhs, rhsT[1]), dot(lhs, rhsT[2]));
	}



	// multiply matrix and column vector
	inline vec3 operator*(const mat3x3& lhs, const vec3& rhs)
	{
		return vec3(dot(lhs[0], rhs), dot(lhs[1], rhs), dot(lhs[2], rhs));
	}



	inline mat3x3 operator*(const mat3x3& lhs, const mat3x3& rhs)
	{
		mat3x3 rhsT = transpose(rhs);

		return mat3x3
			(
			vec3(dot(lhs[0], rhsT[0]), dot(lhs[0], rhsT[1]), dot(lhs[0], rhsT[2])),
			vec3(dot(lhs[1], rhsT[0]), dot(lhs[1], rhsT[1]), dot(lhs[1], rhsT[2])),
			vec3(dot(lhs[2], rhsT[0]), dot(lhs[2], rhsT[1]), dot(lhs[2], rhsT[2]))
			);
	}


	inline vec3 rotMatToEulerAngles(const mat3x3& m)
	{
		vec3 result;
		result.x = -sin(m[2][0]);
		result.y = atan2(m[2][1] / cos(result.x), m[2][2] / cos(result.x));
		result.z = atan2(m[1][0] / cos(result.x), m[0][0] / cos(result.x));

		return result;

	}


	// todo quick
	inline float deteminant(const mat3x3& m)
	{
		return dot(m[0], cross(m[1], m[2]));
	}

	// todo quick
	inline mat3x3 inverse(const mat3x3& m)
	{
		mat3x3 _m;

		_m[0] = vec3(m[1][1] * m[2][2] - m[1][2] * m[2][1],
			m[0][2] * m[2][1] - m[0][1] * m[2][2],
			m[0][1] * m[1][2] - m[0][2] * m[1][1]);

		_m[1] = vec3(m[1][2] * m[2][0] - m[1][0] * m[2][2],
			m[0][0] * m[2][2] - m[0][2] * m[2][0],
			m[0][2] * m[1][0] - m[0][0] * m[1][2]);

		_m[2] = vec3(m[1][0] * m[2][1] - m[1][1] * m[2][0],
			m[0][1] * m[2][0] - m[0][0] * m[2][1],
			m[0][0] * m[1][1] - m[0][1] * m[1][0]);

		return 1.0f / deteminant(m) * _m;

	}


	// quaternion

	inline Quaternion QuatFromAxisAngle(const vec3& axis, float angle)
	{
		return Quaternion(sin(0.5f*angle)*normalize(axis), cos(0.5f*angle));
	}


	// not yet good
	inline vec3 QuatToEulerAngles(const Quaternion& q)
	{
		vec3 result;

		float sqw = q.w*q.w;
		float sqx = q.v.x*q.v.x;
		float sqy = q.v.y*q.v.y;
		float sqz = q.v.z*q.v.z;

		float test = q.v.x*q.v.y + q.v.z*q.w;
		if (test > 0.499f)
		{
			result.x = 2 * atan2(q.v.x, q.w);
			result.y = 0.5f*ong_PI;
			result.z = 0;
			return result;
		}
		if (test < -0.499)
		{
			result.x = 2.0f * atan2(q.v.x, q.w);
			result.y = -0.5f * ong_PI;
			result.z = 0;
			return result;
		}


		result.x = atan2(2.0f * q.v.y*q.w - 2.0f * q.v.x*q.v.z, sqx - sqy - sqz + sqw);
		result.y = asin(2.0f * test);
		result.z = atan2(2.0f * q.v.x*q.w - 2.0f * q.v.y*q.v.z, -sqx + sqy - sqz + sqw);

		return result;
	}

	inline Quaternion QuatFromEulerAngles(float yaw, float pitch, float roll)
	{
		return Quaternion(
			vec3(
			cos(roll*0.5f) * sin(pitch*0.5f) * cos(yaw*0.5f) + sin(roll*0.5f) * cos(pitch*0.5f) * sin(yaw*0.5f),
			cos(roll*0.5f) * cos(pitch*0.5f) * sin(yaw*0.5f) - sin(roll*0.5f) * sin(pitch*0.5f) * cos(yaw*0.5f),
			sin(roll*0.5f) * cos(pitch*0.5f) * cos(yaw*0.5f) - cos(roll*0.5f) * sin(pitch*0.5f) * sin(yaw*0.5f)
			),
			cos(roll*0.5f) * cos(pitch*0.5f) * cos(yaw*0.5f) + sin(roll*0.5f) * sin(pitch*0.5f) * sin(yaw*0.5f)

			);
	}

	// vectors should be normalized!
	inline Quaternion QuatFromTwoVectors(const vec3& from, const vec3& to)
	{
		float r = 1.0f + ong::dot(from, to);
		ong::vec3 w;
		if (r <= 0.0f)
		{
			r = 0;

			w = abs(from.x) > abs(from.z) ? vec3(-from.y, from.x, 0.f)
				: vec3(0.f, -from.z, from.y);
		}
		else
		{
			w = ong::cross(from, to);
		}

		return Quaternion(w, r);
	}

	inline mat3x3 toRotMat(const Quaternion& q)
	{
		float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

		s = 2.0f / (q.v.x * q.v.x + q.v.y*q.v.y + q.v.z*q.v.z + q.w*q.w);

		xs = s*q.v.x; ys = s * q.v.y; zs = s*q.v.z;
		wx = q.w*xs; wy = q.w * ys; wz = q.w*zs;
		xx = q.v.x*xs; xy = q.v.x*ys; xz = q.v.x*zs;
		yy = q.v.y*ys; yz = q.v.y*zs; zz = q.v.z*zs;

		return mat3x3(
			vec3(1.0f - (yy + zz), xy - wz, xz + wy),
			vec3(xy + wz, 1.0f - (xx + zz), yz - wx),
			vec3(xz - wy, yz + wx, 1.0f - (xx + yy))
			);

		//float _2x = 2.0f*q.v.x;
		//float _2y = 2.0f*q.v.y;
		//float _2z = 2.0f*q.v.z;

		//return mat3x3(
		//{ 1.0f - _2y*q.v.y - _2z*q.v.z, _2x*q.v.y - _2z*q.w, _2x*q.v.z + _2y*q.w },
		//{ _2x*q.v.y + _2z*q.w, 1.0f - _2x*q.v.x - _2z*q.v.z, _2y*q.v.z - _2x*q.w },
		//{ _2x*q.v.z - _2y*q.w, _2y*q.v.z + _2x*q.w, 1.0f - _2x*q.v.x - _2y*q.v.y }
		//);
	}


	inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion(
			lhs.w * rhs.v + rhs.w * lhs.v + cross(lhs.v, rhs.v),
			lhs.w * rhs.w - dot(lhs.v, rhs.v)
			);
	}

	inline Quaternion conjugate(const Quaternion& q)
	{
		return Quaternion(-q.v, q.w);
	}

	inline float lengthSq(const Quaternion& q)
	{
		return dot(q.v, q.v) + q.w*q.w;
	}

	inline Quaternion normalize(const Quaternion& q)
	{
		float denom = 1.0f / sqrt(lengthSq(q));

		return Quaternion(denom*q.v, denom*q.w);
	}


	inline vec3 rotate(const vec3& v, const Quaternion& q)
	{
		return (q* Quaternion(v, 0.0f) * conjugate(q)).v;
	}


	// transform


	inline Transform invTransform(const Transform& t)
	{
		return Transform(rotate(-t.p, conjugate(t.q)), conjugate(t.q));
	}

	inline vec3 transformVec3(const vec3& v, const Transform& t)
	{
		return rotate(v, t.q) + t.p;
	}

	inline vec3 transformVec3(const vec3& v, const vec3& t, const mat3x3& rot)
	{
		return rot * v + t;
	}


	inline vec3 invTransformVec3(const vec3& v, const Transform& t)
	{
		return rotate(v - t.p, conjugate(t.q));
	}

	inline vec3 invTransformVec3(const vec3& v, const vec3& t, const mat3x3& rot)
	{
		return transpose(rot) * (v - t);
	}

	inline Transform transformTransform(const Transform& t1, const Transform& t2)
	{
		return Transform(transformVec3(t1.p, t2), t2.q * t1.q);
	}

	inline Transform invTransformTransform(const Transform& t1, const Transform& t2)
	{
		return Transform(rotate(t1.p - t2.p, conjugate(t2.q)), conjugate(t2.q) * t1.q);
	}
}