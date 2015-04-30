#include "geomMath.h"

namespace ong
{


	float closestPtSegmentSegment(const vec3& p1, const vec3& q1, const vec3& p2, const vec3& q2,
		float &s, float &t, vec3& c1, vec3& c2)
	{
		vec3 d1 = q1 - p1;
		vec3 d2 = q2 - p2;
		vec3 r = p1 - p2;
		float a = dot(d1, d1);
		float e = dot(d2, d2);
		float c = dot(d1, r);
		float f = dot(d2, r);

		float b = dot(d1, d2);
		float denom = a*e - b*b;

		if (denom != 0.0f)
			s = ong_clamp((b*f - c*e) / denom, 0.0f, 1.0f);
		else
			s = 0.0f;

		t = (b*s + f) / e;

		if (t < 0.0f)
		{
			t = 0.0f;
			s = ong_clamp(-c / a, 0.0f, 1.0f);
		}
		else if (t > 1.0f)
		{
			t = 1.0f;
			s = ong_clamp((b - c) / a, 0.0f, 1.0f);
		}

		c1 = p1 + s * d1;
		c2 = p2 + t * d2;

		return dot(c1 - c2, c1 - c2);
	}


	float closestPtSegmentRay(const vec3& p, const vec3& q, const vec3& o, const vec3& d,
		float &s, float &t, vec3& c1, vec3& c2)
	{
		vec3 d1 = q - p;
		vec3 r = p - o;
		float a = dot(d1, d1);
		float c = dot(d1, r);
		float f = dot(d, r);

		float b = dot(d1, d);
		float denom = a - b*b;

		if (denom != 0.0f)
			s = ong_clamp((b*f - c) / denom, 0.0f, 1.0f);
		else
			s = 0.0f;

		t = (b*s + f);

		if (t < 0.0f)
		{
			t = 0.0f;
			s = ong_clamp(-c / a, 0.0f, 1.0f);
		}


		c1 = p + s * d1;
		c2 = o + t * d;

		return dot(c1 - c2, c1 - c2);
	}



	vec3 closestPtPointSegment(const vec3& p, const vec3& a, const vec3& b, float* t)
	{
		vec3 ab = b - a;
		float _t = dot(p - a, ab) / dot(ab, ab);
		if (t) *t = _t;

		if (_t < 0.0f)
			return a;

		else if (_t > 1.0f)
			return b;

		return a + _t * ab;
	}

	vec3 closestPtPointTriangle(const vec3&p, const vec3& a, const vec3& b, const vec3& c,
		float* _u, float* _v, float* _w)
	{

		if (_u) *_u = 0.0f;
		if (_v) *_v = 0.0f;
		if (_w) *_w = 0.0f;

		vec3 ab = b - a;
		vec3 ac = c - a;
		vec3 bc = c - b;

		float snom = dot(p - a, ab);
		float sdenom = dot(p - b, -ab);

		float tnom = dot(p - a, ac);
		float tdenom = dot(p - b, -ac);


		if (snom <= 0.0f && tnom <= 0.0f)
		{
			if (_u) *_u = 1.0f;
			return a;
		}

		float unom = dot(p - b, bc);
		float udenom = dot(p - c, -bc);

		if (sdenom <= 0.0f && unom <= 0.0f)
		{
			if (_v) *_v = 1.0f;
			return b;
		}

		if (tdenom <= 0.0f && udenom <= 0.0f)
		{
			if (_v) *_w = 1.0f;
			return c;
		}


		vec3 n = cross(-ab, -ac);
		float vc = dot(n, cross(a - p, b - p));

		if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
		{
			if (_u) *_u = 1.0f - snom / (snom + sdenom);
			if (_v) *_v = snom / (snom + sdenom);

			return  a + snom / (snom + sdenom)*ab;
		}

		float va = dot(n, cross(b - p, c - p));

		if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
		{
			if (_v) *_v = 1.0f - unom / (unom + udenom);
			if (_w) *_w = unom / (unom + udenom);

			return  b + unom / (unom + udenom)*bc;
		}

		float vb = dot(n, cross(c - p, a - p));

		if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
		{
			if (_u) *_u = 1.0f - tnom / (tnom + tdenom);
			if (_w) *_w = tnom / (tnom + tdenom);

			return a + tnom / (tnom + tdenom)*ac;
		}

		float u = va / (va + vb + vc);
		float v = vb / (va + vb + vc);
		float w = 1.0f - u - v;

		if (_u) *_u = u;
		if (_v) *_v = v;
		if (_w) *_w = w;

		return u*a + v*b + w*c;

	}

}