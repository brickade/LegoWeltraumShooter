#pragma once


#include "myMath.h"

namespace ong
{



	struct Plane
	{
		vec3 n;
		float d;
	};

	struct Line
	{
		vec3 o;
		vec3 d; //normalized
	};


	//Plane

	inline Plane planeFromABC(const vec3& a, const vec3& b, const vec3& c)
	{
		Plane p;
		p.n = normalize(cross(b - a, c - a));
		p.d = dot(p.n, a);
		return p;
	}

	inline vec3 closestPtPointPlane(const vec3& q, const Plane& p)
	{
		float t = (dot(p.n, q) - p.d);
		return q - t*p.n;
	}


	inline bool intersectSegmentPlane(const vec3& a, const vec3& b, const Plane& p, float& t, vec3& q)
	{
		vec3 ab = b - a;

		t = (p.d - dot(p.n, a)) / dot(p.n, ab);

		if (t >= 0.0f && t <= 1.0f)
		{
			q = a + t * ab;
			return true;
		}

		return false;
	}

	inline Plane transformPlane(const Plane& p, const Transform& t)
	{
		Plane _p;
		_p.n = (t.q * Quaternion(p.n, 0.0f) * conjugate(t.q)).v;
		_p.d = dot(transformVec3(p.d * p.n, t), _p.n);
		return _p;
	}


	inline Plane transformPlane(const Plane& p, const vec3& t, const mat3x3& rot)
	{
		Plane _p;
		_p.n = rot * p.n;
		_p.d = dot(transformVec3(p.d * p.n, t, rot), _p.n);
		return _p;
	}


	inline Plane invTransformPlane(const Plane& p, Transform& t)
	{
		Plane _p;
		_p.n = rotate(p.n, conjugate(t.q));
		_p.d = dot(invTransformVec3(p.d * p.n, t), _p.n);
		return _p;
	}

	inline Plane invTransformPlane(const Plane& p, const vec3& t, const mat3x3& rot)
	{
		Plane _p;
		_p.n = transpose(rot) * p.n;
		_p.d = dot(invTransformVec3(p.d * p.n, t, rot), _p.n);
		return _p;
	}


	//Line


	inline Line lineFromAB(const vec3& a, const vec3& b)
	{
		Line l;
		l.o = a;
		l.d = normalize(b - a);
		return l;
	}

	//return squared distance
	float closestPtSegmentSegment(const vec3& p1, const vec3& q1, const vec3& p2, const vec3& q2,
		float &s, float &t, vec3& c1, vec3& c2);
	float closestPtSegmentRay(const vec3& a, const vec3& b, const vec3& o, const vec3& d,
		float &s, float &t, vec3& c1, vec3& c2);

	vec3 closestPtPointSegment(const vec3& p, const vec3& a, const vec3& b, float* t = 0);
	vec3 closestPtPointTriangle(const vec3&p, const vec3& a, const vec3& b, const vec3& c,
		float* u = 0, float* v = 0, float* w = 0);

	// dist

	inline float distPointPlane(const vec3& q, const Plane& p)
	{
		return dot(p.n, q) - p.d;
	}

	inline float distPointFatPlane(const vec3& q, const Plane& p, float epsilon)
	{
		float dist = distPointPlane(q, p);
		if (dist <= epsilon && dist >= -epsilon) dist = 0.0f;
		return dist;
	}

	inline float sqDistPointLine(const vec3& q, const Line& l)
	{
		vec3 ac = q - l.o;
		float e = dot(ac, l.d);
		return dot(ac, ac) - e*e;
	}

	inline float sqDistPointSegment(const vec3& p, const vec3& a, const vec3& b)
	{
		vec3 ab = b - a;
		vec3 ac = p - a;
		vec3 bc = p - b;

		float e = dot(ac, ab);

		if (e <= 0.0f) return dot(ac, ac);
		float f = dot(ab, ab);
		if (e >= f) return dot(bc, bc);
		return dot(ac, ac) - e*e / f;

	}

}