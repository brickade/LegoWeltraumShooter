#include "Shapes.h"
#include <float.h>
#include <algorithm>
#include "Settings.h"

namespace ong
{


	static const int MAX_GJK_ITERATIONS = 20;

	float sqDistPointAABB(const vec3& p, const AABB& aabb)
	{
		float sqDist = 0.0f;

		for (int i = 0; i < 3; ++i)
		{
			float v = p[i];

			float min = aabb.c[i] - aabb.e[i];
			float max = aabb.c[i] + aabb.e[i];

			if (v < min) sqDist += (min - v) * (min - v);
			if (v > max) sqDist += (v - max) * (v - max);
		}

		return sqDist;
	}


	AABB calculateAABB(ShapePtr shape, const Transform& transform)
	{
		switch (shape.getType())
		{
		case ShapeType::HULL:
			return calculateAABB(shape.toHull(), transform);
		case ShapeType::SPHERE:
			return calculateAABB(shape.toSphere(), transform);
		case ShapeType::CAPSULE:
			return calculateAABB(shape.toCapsule(), transform);
		default:
			return { vec3(0, 0, 0), vec3(0, 0, 0) };

		}
	}
		
	AABB calculateAABB(const Hull* hull, const Transform& transform)
	{
		AABB aabb;

		mat3x3 rot = toRotMat(transform.q);

		vec3 min = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		vec3 max = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int i = 0; i < hull->numVertices; ++i)
		{
			vec3 v = rot * hull->pVertices[i];

			for (int i = 0; i < 3; ++i)
			{
				if (v[i] < min[i]) min[i] = v[i];
				if (v[i] > max[i]) max[i] = v[i];
			}
		}

		aabb.e = 0.5f * (max - min);
		aabb.c = transform.p + min + aabb.e;
		return aabb;
	}

	AABB calculateAABB(const Sphere* sphere, const Transform& transform)
	{
		AABB aabb;
		aabb.c = transformVec3(sphere->c, transform);
		aabb.e = sphere->r * vec3(1, 1, 1);

		return aabb;
	}

	AABB calculateAABB(const Capsule* capsule, const Transform& transform)
	{
		AABB aabb;
		aabb.c = 0.5f * (capsule->c1 + capsule->c2) + transform.p;

		for (int i = 0; i < 3; ++i)
		{
			vec3 c1 = transformVec3(capsule->c1, transform);
			vec3 c2 = transformVec3(capsule->c2, transform);

			aabb.e[i] = ong_MAX(ong_MAX(ong_MAX(abs(c1[i] + capsule->r), abs(c1[i] - capsule->r)),
				abs(c2[i] + capsule->r)),
				abs(c2[i] - capsule->r)) - aabb.c[i];
		}
		return aabb;
	}


	vec3 getHullSupport(const vec3& dir, const Hull* hull, int* idx)
	{
		int maxIndex = -1;
		float maxProjection = -FLT_MAX;

		for (int i = 0; i < hull->numVertices; ++i)
		{
			float projection = dot(dir, hull->pVertices[i]);
			if (projection > maxProjection)
				maxIndex = i, maxProjection = projection;
		}

		if (idx)
			*idx = maxIndex;
		return hull->pVertices[maxIndex];
	}

	vec3 getSegmentSupport(const vec3& dir, const vec3& a, const vec3& b, int* idx)
	{
		float projA = dot(dir, a);
		float projB = dot(dir, b);

		if (projA > projB)
		{
			if (idx) *idx = 0;
			return a;
		}
		else
		{
			if (idx) *idx = 1;
			return b;
		}

	}

	struct SimplexVertex
	{
		vec3 p;
		vec3 pA;
		vec3 pB;
		int idxA;
		int idxB;
		float a;
	};

	struct Simplex
	{
		SimplexVertex v[4];
		int size;
	};

	vec3 pointOfMinimumNormAndCulling(const vec3& p, Simplex* simplex)
	{
		assert(simplex->size > 0);

		if (simplex->size == 1)
		{
			return simplex->v[0].p;
		}
		else if (simplex->size == 2)
		{
			float t;
			vec3 _p = closestPtPointSegment(p, simplex->v[0].p, simplex->v[1].p, &t);

			simplex->v[0].a = 1.0f - t;
			simplex->v[1].a = t;

			if (t == 0.0f)
			{
				simplex->size = 1;
			}
			else if (t == 1.0f)
			{
				simplex->v[0] = simplex->v[1];
				simplex->size = 1;
			}

			return _p;
		}
		else if (simplex->size == 3)
		{
			float u, v, w;
			vec3 _p = closestPtPointTriangle(p, simplex->v[0].p, simplex->v[1].p, simplex->v[2].p, &u, &v, &w);

			simplex->v[0].a = u;
			simplex->v[1].a = v;
			simplex->v[2].a = w;

			if (u == 1.0f)
			{
				simplex->size = 1;
			}
			else if (v == 1.0f)
			{
				simplex->v[0] = simplex->v[1];
				simplex->size = 1;
			}
			else if (w == 1.0f)
			{
				simplex->v[0] = simplex->v[2];
				simplex->size = 1;
			}
			else if (w == 0.0f)
			{
				simplex->size = 2;
			}
			else if (v == 0.0f)
			{
				simplex->v[1] = simplex->v[2];
				simplex->size = 2;
			}
			else if (u == 0.0f)
			{
				simplex->v[0] = simplex->v[1];
				simplex->v[1] = simplex->v[2];
				simplex->size = 2;
			}
			return _p;
		}
		else
		{
			vec3 AP = p - simplex->v[0].p;

			vec3 AB = simplex->v[1].p - simplex->v[0].p;
			vec3 AC = simplex->v[2].p - simplex->v[0].p;
			vec3 AD = simplex->v[3].p - simplex->v[0].p;

			float APAB = dot(AP, AB), APAC = dot(AP, AC), APAD = dot(AP, AD);


			simplex->size = 1;

			// vertex a
			if (APAB <= 0.0f && APAC <= 0.0f && APAD <= 0.0f)
			{
				simplex->v[0].a = 0.0f;
				return simplex->v[0].p;
			}

			vec3 BP = p - simplex->v[1].p;

			vec3 BC = simplex->v[2].p - simplex->v[1].p;
			vec3 BD = simplex->v[3].p - simplex->v[1].p;

			float BPBA = dot(BP, -AB), BPBC = dot(BP, BC), BPBD = dot(BP, BD);

			// vertex b

			if (BPBA <= 0.0f && BPBC <= 0.0f && BPBD <= 0.0f)
			{
				simplex->v[0] = simplex->v[1];
				simplex->v[0].a = 0.0f;
				return simplex->v[0].p;
			}

			vec3 CP = p - simplex->v[2].p;

			vec3 CD = simplex->v[3].p - simplex->v[2].p;

			float CPCA = dot(CP, -AC), CPCB = dot(CP, -BC), CPCD = dot(CP, CD);

			// vertex c
			if (CPCA <= 0.0f && CPCB <= 0.0f && CPCD <= 0.0f)
			{
				simplex->v[0] = simplex->v[2];
				simplex->v[0].a = 0.0f;
				return simplex->v[0].p;
			}

			vec3 DP = p - simplex->v[3].p;

			float DPDA = dot(DP, -AD), DPDB = dot(DP, -BD), DPDC = dot(DP, -CD);

			// vertex d
			if (DPDA <= 0.0f && DPDB <= 0.0f && DPDC <= 0.0f)
			{
				simplex->v[0] = simplex->v[3];
				simplex->v[0].a = 0.0f;
				return simplex->v[0].p;
			}

			simplex->size = 2;

			vec3 nABC = cross(AB, AC);
			vec3 nADB = cross(AD, AB);

			//edge ab
			if (APAB >= 0.0f && BPBA >= 0.0f &&
				dot(AP, cross(AB, nABC)) >= 0.0f && dot(AP, cross(nADB, AB)) >= 0.0f)
			{
				float t = APAB / dot(AB, AB);
				simplex->v[0].a = 1.0f - t;
				simplex->v[1].a = t;

				return (1.0f - t)*simplex->v[0].p + t * simplex->v[1].p;;
			}

			vec3 nACD = cross(AC, AD);

			// edge ac
			if (APAC >= 0.0f && CPCA >= 0.0f &&
				dot(AP, cross(AC, nACD)) >= 0.0f && dot(AP, cross(nABC, AC)) >= 0.0f)
			{
				simplex->v[1] = simplex->v[2];

				float t = APAC / dot(AC, AC);
				simplex->v[0].a = 1.0f - t;
				simplex->v[1].a = t;

				return (1.0f - t)*simplex->v[0].p + t * simplex->v[1].p;
			}

			// edge ad
			if (APAD >= 0.0f && DPDA >= 0.0f &&
				dot(AP, cross(AD, nADB)) >= 0.0f && dot(AP, cross(nACD, AD)) >= 0.0f)
			{
				simplex->v[1] = simplex->v[3];

				float t = APAD / dot(AD, AD);
				simplex->v[0].a = 1.0f - t;
				simplex->v[1].a = t;

				return (1.0f - t)*simplex->v[0].p + t * simplex->v[1].p;
			}

			vec3 nBDC = cross(BD, BC);

			// edge bc
			if (BPBC >= 0.0f && CPCB >= 0.0f &&
				dot(BP, cross(BC, nABC)) >= 0.0f && dot(BP, cross(nBDC, BC)) >= 0.0f)
			{
				simplex->v[0] = simplex->v[1];
				simplex->v[1] = simplex->v[2];

				float t = BPBC / dot(BC, BC);
				simplex->v[0].a = 1.0f - t;
				simplex->v[1].a = t;

				return (1.0f - t)*simplex->v[0].p + t * simplex->v[1].p;
			}

			// edge bd
			if (BPBD >= 0.0f && DPDB >= 0.0f &&
				dot(BP, cross(BD, nBDC)) >= 0.0f && dot(BP, cross(nADB, BD)) >= 0.0f)
			{
				simplex->v[0] = simplex->v[1];
				simplex->v[1] = simplex->v[3];

				float t = BPBD / dot(BD, BD);
				simplex->v[0].a = 1.0f - t;
				simplex->v[1].a = t;

				return (1.0f - t)*simplex->v[0].p + t * simplex->v[1].p;
			}

			// edge cd
			if (CPCD >= 0.0f && DPDC >= 0.0f &&
				dot(CP, cross(CD, nACD)) >= 0.0f && dot(CP, cross(nBDC, CD)) >= 0.0f)
			{
				simplex->v[0] = simplex->v[2];
				simplex->v[1] = simplex->v[3];

				float t = CPCD / dot(CD, CD);
				simplex->v[0].a = 1.0f - t;
				simplex->v[1].a = t;

				return (1.0f - t)*simplex->v[0].p + t * simplex->v[1].p;
			}


			simplex->size = 4;
			float bestDistance = FLT_MAX;
			vec3 _p = p;
			vec3* n = nullptr;
			float u = 0, v = 0, w = 0;

			// face ABC
			//if (dot(AP, nABC) > 0)
			if (dot(AP, nABC) * dot(AD, nABC) <= 0)
			{
				simplex->size = 3;

				float va = dot(nABC, cross(-BP, -CP));
				float vb = dot(nABC, cross(-CP, -AP));
				float vc = dot(nABC, cross(-AP, -BP));
				float vg = va + vb + vc;


				vec3 __p = va / vg * simplex->v[0].p + vb / vg * simplex->v[1].p + vc / vg * simplex->v[2].p;

				if (lengthSq(p - __p) < bestDistance && va > 0.0f && vb > 0.0f && vc > 0.0f)
				{
					_p = __p, bestDistance = lengthSq(p - __p), n = &nABC;
					u = va / vg, v = vb / vg, w = vc / vg;
				}

			}

			// face ADB
			//if (dot(AP, nADB) > 0)
			if (dot(AP, nADB) * dot(AC, nADB) <= 0)
			{
				simplex->size = 3;

				float va = dot(nADB, cross(-DP, -BP));
				float vd = dot(nADB, cross(-BP, -AP));
				float vb = dot(nADB, cross(-AP, -DP));
				float vg = va + vd + vb;

				vec3 __p = va / vg * simplex->v[0].p + vd / vg * simplex->v[3].p + vb / vg * simplex->v[1].p;

				if (lengthSq(p - __p) < bestDistance && va > 0.0f && vd > 0.0f && vb > 0.0f)
				{
					_p = __p, bestDistance = lengthSq(p - __p), n = &nADB;
					u = va / vg, v = vd / vg, w = vb / vg;
				}
			}


			// face ACD
			//if (dot(AP, nACD) > 0)
			if (dot(AP, nACD) * dot(AB, nACD) <= 0)
			{
				simplex->size = 3;

				float va = dot(nACD, cross(-CP, -DP));
				float vc = dot(nACD, cross(-DP, -AP));
				float vd = dot(nACD, cross(-AP, -CP));
				float vg = va + vc + vd;

				vec3 __p = va / vg * simplex->v[0].p + vc / vg * simplex->v[2].p + vd / vg * simplex->v[3].p;

				if (lengthSq(p - __p) < bestDistance && va > 0.0f && vc > 0.0f && vd > 0.0f)
				{
					_p = __p, bestDistance = lengthSq(p - __p), n = &nACD;
					u = va / vg, v = vc / vg, w = vd / vg;
				}
			}

			// face BDC
			//if (dot(BP, nBDC) > 0)
			if (dot(BP, nBDC) * dot(-AB, nBDC) <= 0)
			{
				simplex->size = 3;

				float vb = dot(nBDC, cross(-DP, -CP));
				float vd = dot(nBDC, cross(-CP, -BP));
				float vc = dot(nBDC, cross(-BP, -DP));
				float vg = vb + vd + vc;

				vec3 __p = vb / vg * simplex->v[1].p + vd / vg * simplex->v[3].p + vc / vg * simplex->v[2].p;

				if (lengthSq(p - __p) < bestDistance && vb > 0.0f && vd > 0.0f && vc > 0.0f)
				{
					_p = __p, bestDistance = lengthSq(p - __p), n = &nBDC;
					u = vb / vg, v = vd / vg, w = vc / vg;
				}
			}

			if (n == &nADB)
			{
				simplex->v[2] = simplex->v[1];
				simplex->v[1] = simplex->v[3];
			}
			else if (n == &nACD)
			{
				simplex->v[1] = simplex->v[2];
				simplex->v[2] = simplex->v[3];
			}
			else if (n == &nBDC)
			{
				simplex->v[0] = simplex->v[1];
				simplex->v[1] = simplex->v[3];
			}


			simplex->v[0].a = u;
			simplex->v[1].a = v;
			simplex->v[2].a = w;

			return _p;
		}
	}

	vec3 closestPointOnHull(const vec3& p, const Hull* hull, float epsilon)
	{


		Simplex simplex;

		float hullEpsilon = hull->epsilon / FLT_EPSILON * epsilon;

		//starting point
		{
			vec3 d = p - hull->centroid;
			int i;
			simplex.v[0].p = getHullSupport(d, hull, &i);
			simplex.v[0].idxA = i;
			simplex.size = 1;
		}

		vec3 last_p = p;

		for (int i = 0; i < MAX_GJK_ITERATIONS; ++i)
		{
			vec3 _p = pointOfMinimumNormAndCulling(p, &simplex);

			if (_p == p || simplex.size == 4)
				return p;

			if (lengthSq(_p - last_p) <= lengthSq(_p) * epsilon)
				return _p;


			vec3 d = p - _p;
			if (lengthSq(d) < hullEpsilon)
				return _p;


			int idx;
			vec3 support = getHullSupport(d, hull, &idx);



			if (abs(dot(d, support) - dot(d, _p)) <= 10.0f * hullEpsilon)
				return _p;

			for (int i = 0; i < simplex.size; ++i)
			{
				if (idx == simplex.v[i].idxA)
					return _p;
			}

			simplex.v[simplex.size].p = support;
			simplex.v[simplex.size].idxA = idx;
			simplex.size++;

			last_p = _p;

		}


		return p;
	}


	// todo proper failure state
	float closestPtSegmentHull(const vec3& a, const vec3& b, const Hull* hull, vec3& cSegment, vec3& cHull, float epsilon)
	{
		Simplex simplex;

		float hullEpsilon = hull->epsilon / FLT_EPSILON * epsilon;

		//starting point
		{
			vec3 d = hull->centroid - a;
			simplex.v[0].pA = getSegmentSupport(d, a, b, &simplex.v[0].idxA);
			simplex.v[0].pB = getHullSupport(-d, hull, &simplex.v[0].idxB);
			simplex.v[0].p = simplex.v[0].pA - simplex.v[0].pB;

			simplex.size = 1;
		}

		const vec3 o = vec3(0.0f, 0.0f, 0.0f);

		vec3 last_p = o;


		for (int i = 0; i < MAX_GJK_ITERATIONS; ++i)
		{
			vec3 _p = pointOfMinimumNormAndCulling(o, &simplex);

			if (_p == o || simplex.size == 4)
				break;

			if (lengthSq(_p - last_p) <= lengthSq(_p) * epsilon)
				break;


			vec3 d = -_p;
			if (lengthSq(d) < hullEpsilon)
				break;


			int idxA;
			vec3 supportA = getSegmentSupport(d, a, b, &idxA);
			int idxB;
			vec3 supportB = getHullSupport(-d, hull, &idxB);
			vec3 support = supportA - supportB;



			if (abs(dot(d, support) - dot(d, _p)) <= 10.0f * hullEpsilon)
				break;


			bool valid = true;;
			for (int i = 0; i < simplex.size; ++i)
			{
				if (idxA == simplex.v[i].idxA && idxB == simplex.v[i].idxB)
				{
					valid = false;
					break;
				}
			}
			if (!valid) break;


			simplex.v[simplex.size].p = support;
			simplex.v[simplex.size].idxA = idxA;
			simplex.v[simplex.size].idxB = idxB;
			simplex.v[simplex.size].pA = supportA;
			simplex.v[simplex.size].pB = supportB;
			simplex.size++;

			last_p = _p;

		}

		assert(simplex.size > 0);

		switch (simplex.size)
		{
		case 1:
		{
			cSegment = simplex.v[0].pA;
			cHull = simplex.v[0].pB;
			float dist = lengthSq(simplex.v[0].p);
			return dist;
		}
		case 2:
		{
			cSegment = simplex.v[0].a * simplex.v[0].pA + simplex.v[1].a * simplex.v[1].pA;
			cHull = simplex.v[0].a * simplex.v[0].pB + simplex.v[1].a * simplex.v[1].pB;
			float dist = lengthSq(simplex.v[0].a * simplex.v[0].p + simplex.v[1].a * simplex.v[1].p);
			return dist;
		}
		case 3:
		{
			float u = simplex.v[0].a;
			float v = simplex.v[1].a;
			float w = simplex.v[2].a;
			cSegment = u * simplex.v[0].pA + v * simplex.v[1].pA + w * simplex.v[2].pA;
			cHull = u * simplex.v[0].pB + v * simplex.v[1].pB + w * simplex.v[2].pB;
			float dist = lengthSq(u * simplex.v[0].p + v * simplex.v[1].p + w * simplex.v[2].p);
			return dist;
		}
		case 4:
			return 0.0f;
		default:
			return 0.0f;
		}
	}

	bool overlap(Sphere* sphere, AABB* aabb)
	{
		float sqDist = sqDistPointAABB(sphere->c, *aabb);

		return sqDist <= sphere->r * sphere->r;
	}

	bool overlap(Sphere* sphereA, Sphere* sphereB)
	{
		float r = sphereA->r + sphereB->r;
		return lengthSq(sphereB->c - sphereA->c) < r*r;
	}

	bool overlap(const AABB* aabbA, const AABB* aabbB)
	{
		if (abs(aabbA->c[0] - aabbB->c[0]) > (aabbA->e[0] + aabbB->e[0])) return false;
		if (abs(aabbA->c[1] - aabbB->c[1]) > (aabbA->e[1] + aabbB->e[1])) return false;
		if (abs(aabbA->c[2] - aabbB->c[2]) > (aabbA->e[2] + aabbB->e[2])) return false;

		return true;
	}


	bool overlap(const ShapePtr shapeA, const ShapePtr shapeB, const Transform& ta, const Transform& tb)
	{

		typedef bool(*overlapFunc)(const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb);

		static const overlapFunc overlapMat[ShapeType::COUNT][ShapeType::COUNT] =
		{
			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(a.toSphere(), b.toSphere(), ta, tb); },
			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(a.toSphere(), b.toCapsule(), ta, tb); },
			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(a.toSphere(), b.toHull(), ta, tb); },

			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(b.toSphere(), a.toCapsule(), tb, ta); },
			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(a.toCapsule(), b.toCapsule(), ta, tb); },
			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(a.toCapsule(), b.toHull(), ta, tb); },

			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(b.toSphere(), a.toHull(), tb, ta); },
			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(b.toCapsule(), a.toHull(), tb, ta); },
			[](const ShapePtr a, const ShapePtr b, const Transform& ta, const Transform& tb){return overlap(a.toHull(), b.toHull(), ta, tb); }
		};

		return (overlapMat[shapeA.getType()][shapeB.getType()](shapeA, shapeB, ta, tb));
	}




	bool overlap(const AABB& a, const AABB& b, const vec3& t, const mat3x3& rot)
	{
		vec3 _t = rot*(b.c) + t - a.c;

		// a planes
		for (int i = 0; i < 3; ++i)
		{
			float rA = a.e[i];
			float rB = b.e[0] * abs(rot[i][0]) + b.e[1] * abs(rot[i][1]) + b.e[2] * abs(rot[i][2]);
			if (abs(_t[i]) > rA + rB) return false;
		}

		// b planes
		for (int i = 0; i < 3; ++i)
		{
			float rA = a.e[0] * abs(rot[0][i]) + a.e[1] * abs(rot[1][i]) + a.e[2] * abs(rot[2][i]);
			float rB = b.e[i];
			if (abs(_t[0] * rot[0][i] + _t[1] * rot[1][i] + _t[2] * rot[2][i]) > rA + rB) return false;
		}

		return true;

	}




	// narrow

	bool overlap(const Hull* hullA, const Hull* hullB, const Transform& t1, const Transform& t2)
	{
		float aEpsilon = hullA->epsilon / FLT_EPSILON * ong_OVERLAP_EPSILON;
		float bEpsilon = hullB->epsilon / FLT_EPSILON * ong_OVERLAP_EPSILON;

		//planes A
		Transform tB = invTransformTransform(t1, t2);
		for (int i = 0; i < hullA->numFaces; ++i)
		{
			Plane p = transformPlane(hullA->pPlanes[i], tB);

			vec3 support = getHullSupport(-p.n, hullB);
			float dist = distPointFatPlane(support, p, aEpsilon);

			if (dist >= 0.0f)
				return false;
		}

		// planes B
		Transform tA = invTransformTransform(t2, t1);
		for (int i = 0; i < hullB->numFaces; ++i)
		{
			Plane p = transformPlane(hullB->pPlanes[i], tA);

			vec3 support = getHullSupport(-p.n, hullA);
			float dist = distPointFatPlane(support, p, bEpsilon);

			if (dist >= 0.0f)
				return false;
		}

		//edges
		for (int i1 = 0; i1 < hullA->numEdges; i1 += 2)
		{
			HalfEdge* e1 = hullA->pEdges + i1;
			HalfEdge* t1 = hullA->pEdges + e1->twin;

			vec3 p1 = hullA->pVertices[e1->tail];
			vec3 q1 = hullA->pVertices[t1->tail];
			vec3 E1 = q1 - p1;

			for (int i2 = 0; i2 < hullB->numEdges; i2 += 2)
			{
				HalfEdge* e2 = hullB->pEdges + i2;
				HalfEdge* t2 = hullB->pEdges + e2->twin;

				vec3 p2 = transformVec3(hullB->pVertices[e2->tail], tA);
				vec3 q2 = transformVec3(hullB->pVertices[t2->tail], tA);
				vec3 E2 = q2 - p2;


				//check for minkowski face

				vec3 u1 = hullA->pPlanes[e1->face].n;
				vec3 v1 = hullA->pPlanes[t1->face].n;
				vec3 u2 = rotate(hullB->pPlanes[e2->face].n, tA.q);
				vec3 v2 = rotate(hullB->pPlanes[t2->face].n, tA.q);

				float cba = dot(-u1, -E1);
				float dba = dot(-v1, -E1);
				float adc = dot(u1, -E2);
				float bdc = dot(v1, -E2);

				if (cba*dba >= 0.0f || adc*bdc >= 0.0f || cba*bdc <= 0.0f)
					continue;

				// project
				vec3 n = cross(E1, E2);

				float epsilon = ong_MAX(aEpsilon, bEpsilon);

				if (lengthSq(n) < epsilon)
				{
					vec3 m = cross(E1, p2 - p1);
					n = cross(E1, m);
					if (lengthSq(n) < epsilon)
						continue;
				}
				n = normalize(n);

				if (dot(n, p1 - hullA->centroid) < 0.0f)
					n = -n;

				float dist = dot(n, p2 - p1);

				if (dist > -epsilon && dist < epsilon)
					dist = 0.0f;

				if (dist >= 0.0f)
					return false;

			}
		}

		return true;
	}

	bool overlap(const Sphere* sphereA, const Sphere* sphereB, const Transform& t1, const Transform& t2)
	{


		float r = sphereA->r + sphereB->r;
		float epsilon = (3 * r) * ong_OVERLAP_EPSILON;

		Transform t = invTransformTransform(t2, t1);

		vec3 ca = sphereA->c;
		vec3 cb = transformVec3(sphereB->c, t);

		if (ca == cb)
			return true;

		return lengthSq(cb - ca) - r*r < -(epsilon*epsilon);
	}



	bool overlap(const Capsule* capsuleA, const Capsule* capsuleB, const Transform& ta, const Transform& tb)
	{
		float r = capsuleA->r + capsuleB->r;

		Transform t = invTransformTransform(tb, ta);

		vec3 p1 = capsuleA->c1;
		vec3 q1 = capsuleA->c2;

		vec3 p2 = transformVec3(capsuleB->c1, t);
		vec3 q2 = transformVec3(capsuleB->c2, t);

		float S, T;
		vec3 c1, c2;
		float distSq = closestPtSegmentSegment(p1, q1, p2, q2, S, T, c1, c2);

		float epsilon = (ong_MAX(length(q1 - p1), length(q2 - p2) + 3 * ong_MAX(capsuleA->r, capsuleB->r))) * ong_OVERLAP_EPSILON;

		return (distSq - r*r < -(epsilon*epsilon));
	}

	bool overlap(const Sphere* sphereA, const Capsule* capsuleB, const Transform& t1, const Transform& t2)
	{
		Transform t = invTransformTransform(t1, t2);

		float r = sphereA->r + capsuleB->r;

		vec3 c = transformVec3(sphereA->c, t);

		float distSq = sqDistPointSegment(c, capsuleB->c1, capsuleB->c2);

		float epsilon = (length(capsuleB->c2 - capsuleB->c1) + 3 * ong_MAX(sphereA->r, capsuleB->r)) * ong_OVERLAP_EPSILON;

		return (distSq - r*r < -(epsilon*epsilon));
	}



	bool overlap(const Sphere* sphereA, const Hull* hullB, const Transform& t1, const Transform& t2)
	{
		float epsilon = (3 * sphereA->r) * ong_OVERLAP_EPSILON + (hullB->epsilon/FLT_EPSILON * ong_OVERLAP_EPSILON);

		Transform t = invTransformTransform(t1, t2);

		vec3 c = transformVec3(sphereA->c, t);
		vec3 p = closestPointOnHull(c, hullB, ong_OVERLAP_EPSILON);
		
		if (c == p)
			return true;

		return lengthSq(p - c) - sphereA->r*sphereA->r < -10.0f*epsilon;
	}

	bool overlap(const Capsule* capsuleA, const Hull* hullB, const Transform& t1, const Transform& t2)
	{
		float epsilon = ong_MAX((hullB->epsilon/FLT_EPSILON * ong_OVERLAP_EPSILON), (3 * capsuleA->r + length(capsuleA->c2 - capsuleA->c1)) * ong_OVERLAP_EPSILON);

		Transform t = invTransformTransform(t1, t2);

		vec3 c1 = transformVec3(capsuleA->c1, t);
		vec3 c2 = transformVec3(capsuleA->c2, t);

		vec3 s, h;
		float distSq = closestPtSegmentHull(c1, c2, hullB, s, h, ong_OVERLAP_EPSILON);

		return distSq - capsuleA->r * capsuleA->r < -10.0f * epsilon;
	}



	void mergeAABBAABB(AABB* a, AABB* b)
	{
		vec3 min;
		vec3 max;

		min.x = ong_MIN(a->c.x - a->e.x, b->c.x - b->e.x);
		min.y = ong_MIN(a->c.y - a->e.y, b->c.y - b->e.y);
		min.z = ong_MIN(a->c.z - a->e.z, b->c.z - b->e.z);

		max.x = ong_MAX(a->c.x + a->e.x, b->c.x + b->e.x);
		max.y = ong_MAX(a->c.y + a->e.y, b->c.y + b->e.y);
		max.z = ong_MAX(a->c.z + a->e.z, b->c.z + b->e.z);

		a->e = 0.5f * (max - min);
		a->c = min + a->e;
	}

	AABB transformAABB(const AABB* a, const Transform* t)
	{
		mat3x3 rot = toRotMat(t->q);

		AABB result;

		for (int i = 0; i < 3; ++i)
		{
			result.c[i] = t->p[i];
			result.e[i] = 0.0f;

			for (int j = 0; j < 3; ++j)
			{
				result.c[i] += rot[i][j] * a->c[j];
				result.e[i] += abs(rot[i][j]) * a->e[j];
			}

		}

		return result;
	}




	bool intersectRayAABB(const vec3& origin, const vec3& dir, const AABB& aabb, float& tmin, vec3& p)
	{
		tmin = 0.0f;
		float tmax = FLT_MAX;

		for (int i = 0; i < 3; ++i)
		{

			if (abs(dir[i]) < FLT_EPSILON)
			{
				if (origin[i] < aabb.c[i] - aabb.e[i] || origin[i] > aabb.c[i] + aabb.e[i])
				{
					return false;
				}
			}
			else
			{
				float ood = 1.0f / dir[i];
				float t1 = (aabb.c[i] - aabb.e[i] - origin[i]) * ood;
				float t2 = (aabb.c[i] + aabb.e[i] - origin[i]) * ood;

				if (t1 > t2) std::swap(t1, t2);

				tmin = ong_MAX(tmin, t1);
				tmax = ong_MIN(tmax, t2);
				if (tmin > tmax) return false;
			}


		}

		p = origin + tmin * dir;
		return true;
	}

	bool intersectRayHull(const vec3& origin, const vec3& dir, const Hull* hull, float& tmin, vec3& p, vec3& n)
	{
		tmin = 0.0f;
		float tmax = FLT_MAX;

		for (int i = 0; i < hull->numFaces; ++i)
		{
			Plane& p = hull->pPlanes[i];

			float denom = dot(p.n, dir);
            float dist = distPointPlane(origin, p);

			// ray parallel to face
			if (abs(denom) < FLT_EPSILON)
			{
				if (dist > 0.0f)
					return false;
			}
			else
			{
				float t = -dist / denom;
				if (denom < 0.0f)
				{
					if (t > tmin)
					{
						n = p.n;
						tmin = t;
					}
				}
				else
				{
					if (t < tmax) tmax = t;
				}
			}

			if (tmin > tmax)
                return false;

		}

		p = origin + tmin * dir;
		return true;
	}

	bool intersectRaySphere(const vec3& origin, const vec3& dir, const Sphere* sphere, float& tmin, vec3& p, vec3& n)
	{
		vec3 m = origin - sphere->c;
		float b = dot(m, dir);
		float c = dot(m, m) - sphere->r * sphere->r;

		if (c > 0.0f && b > 0.0f) return false;

		float discr = b*b - c;
		if (discr < 0.0f) return false;

		tmin = -b - sqrt(discr);

		if (tmin < 0.0f) tmin = 0.0f;
		p = origin + tmin * dir;
		n = p - sphere->c;

		return true;
	}

	bool intersectRayCapsule(const vec3& origin, const vec3& dir, const Capsule* capsule, float& tmin, vec3& p, vec3& n)
	{

		vec3 _c, r;
		float s, t;
		closestPtSegmentRay(capsule->c1, capsule->c2, origin, dir, s, t, _c, r);

		vec3 m = origin - _c;
		float b = dot(m, dir);
		float c = dot(m, m) - capsule->r * capsule->r;

		if (c > 0.0f && b > 0.0f) return false;

		float discr = b*b - c;
		if (discr < 0.0f) return false;


		tmin = -b - sqrt(discr);

		if (tmin < 0.0f) tmin = 0.0f;
		p = origin + tmin * dir;
		n = p - _c;

		return true;
	}

}