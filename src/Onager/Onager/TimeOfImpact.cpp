#include "TimeOfImpact.h"
#include "myMath.h"
#include "Shapes.h"
#include "World.h"
#include "Body.h"

namespace ong
{

	//currently only works with single sphere colliders
	float _getTimeOfImpact(Body* a, Body* b, ContinuousState* cpS, float tmin)
	{
		//raycast
		float t1 = 1.0f;


		Transform ta = a->getTransform();
		Transform tb = b->getTransform();

		vec3 v = vec3(0, 0, 0);

		if (a->getContinuousPhysics())
		{
			assert("continuos collision objects must only contain a single sphere collider!\n" && a->getCollider()->getShape().getType() == ShapeType::SPHERE && a->getNumCollider() == 1);
			assert(cpS[a->getCpIndex()].t == tmin);
			v += cpS[a->getCpIndex()].p1 - cpS[a->getCpIndex()].p0;
		}
		if (b->getContinuousPhysics())
		{
			assert("continuos collision objects must only contain a single sphere collider!\n" && b->getCollider()->getShape().getType() == ShapeType::SPHERE && b->getNumCollider() == 1);
			assert(cpS[b->getCpIndex()].t == tmin);
			v -= cpS[b->getCpIndex()].p1 - cpS[b->getCpIndex()].p0;
		}


		float absV = length(v);

		// check for inital overlap
		if (overlap(a, b, Transform(vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1))))
		{
			return tmin;
		}


		if (a->getContinuousPhysics() && b->getContinuousPhysics())
		{
			//check for collision moving sphere
			Sphere sA { a->getPosition(), a->getCollider()->getShape().toSphere()->r };
			Sphere sB{ b->getPosition(), b->getCollider()->getShape().toSphere()->r };
			float t = 0.0f;
			overlapMovingSphereSphere(&sA, &sB, v, t, t1);
			t1 = t;
		}
		else
		{
			//raycast from position continuous to standard object
			Body* continuous = 0;
			Body* standard = 0;

			if (a->getContinuousPhysics())
			{
				continuous = a;
				standard = b;
			}
			else if (b->getContinuousPhysics())
			{
				continuous = b;
				standard = a;
			}
			else
			{
				return 0.0f;
			}


			RayQueryResult result = { 0 };
			vec3 center = continuous->getPosition() + continuous->getCollider()->getShape().toSphere()->r * normalize(v);
			if (standard->queryRay(center, v, &result, 1.0f))
			{
				t1 = result.t;
			}
		}

		if (t1 > 1.0f)
			return 1.0f;

		//scale to timeline
		t1 = t1 * (1.0f - tmin) + tmin;

		return t1;
	}



	//todo maybe use different end condition
	float getTimeOfImpact(Body* a, Body* b, ContinuousState* cpS, float tmin)
	{
		//  
		//	first test moving bounding spheres to get approximate
		//	start and end  points
		//  then start with point in the middle and test for overlap
		//	if hit move half distance in the direction of the other body
		//  else move half distance to the right.
		//  repeat until 3 hit or so to get an approximate
		//  time of impact
		//

		float t0 = 0;
		float t1 = 1.0f;


		float ra = length(a->getAABB().e);
		float rb = length(b->getAABB().e);
		vec3 ca = a->getAABB().c;
		vec3 cb = b->getAABB().c;


		Transform ta = a->getTransform();
		Transform tb = b->getTransform();

		vec3 v = vec3(0, 0, 0);

		if (a->getContinuousPhysics())
		{
			assert(cpS[a->getCpIndex()].t == tmin);
			v += cpS[a->getCpIndex()].p1 - cpS[a->getCpIndex()].p0;
		}
		if (b->getContinuousPhysics())
		{
			assert(cpS[b->getCpIndex()].t == tmin);
			v -= cpS[b->getCpIndex()].p1 - cpS[b->getCpIndex()].p0;
		}


		float absV = length(v);

		// check for inital overlap
		if (overlap(a, b, Transform(vec3(0, 0, 0), Quaternion(vec3(0, 0, 0), 1))))
		{
			return tmin;
		}

		//zero velocity
		if (absV < FLT_EPSILON)
		{
			return 1.0f;
		}

		//broad		
		{
			Sphere a = { ca, ra };
			Sphere b = { cb, rb };
			overlapMovingSphereSphere(&a, &b, v, t0, t1);

			if (t0 > 1.0f)//no collision
				return 1.0f;

		}

		//narrow


		static const int MAX_HITS = 7;
		static const float MIN_INTERVAL = 0.0001f;
		int numHits = 1;
		float maxT = t1;


		// substep till first hit
		while ((t1 - t0) > MIN_INTERVAL)
		{
			float t = t0 + ra / absV;
			if (overlap(a, b, Transform(t*v, Quaternion(vec3(0, 0, 0), 1))))
			{
				t1 = t;
				numHits = 1;
				break;
			}
			t0 = t;
		}

		if (numHits == 0)
			return 1.0f;


		while (numHits < MAX_HITS && (t1 - t0) > MIN_INTERVAL)
		{
			float t = 0.5f * (t0 + t1);

			if (overlap(a, b, Transform(t*v, Quaternion(vec3(0, 0, 0), 1))))
			{
				++numHits;
				t1 = t;
			}
			else
			{
				t0 = t;
				//vec3 pa = ca + t*v;
				//float d = dot(b->closestPoint(pa) - pa, v);
				//if (d > 0)
				//{
				//	t0 = t;
				//}
				//else if (d >= -100.0f*FLT_EPSILON && d <= 100.0f*FLT_EPSILON)
				//{
				//	break;
				//}
				//else
				//{
				//	t1 = t;
				//}
			}
		}

		if (t1 >= maxT)
			return 1.0f; //no collision
		
		if (t0 == 0.0f)
			return tmin;

		//scale to timeline
		t1 = t1 * (1.0f - tmin) + tmin;

		return t1;
	}
}