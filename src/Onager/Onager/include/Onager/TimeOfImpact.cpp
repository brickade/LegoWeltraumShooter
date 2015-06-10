#include "TimeOfImpact.h"
#include "myMath.h"
#include "Shapes.h"
#include "World.h"
#include "Body.h"

namespace ong
{

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

		vec3 v = vec3(0,0,0);

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
		if (overlap(a, b, Transform(vec3(0,0,0), Quaternion(vec3(0, 0, 0), 1))))
		{
			return tmin;
		}

		//broad		
		{
			Sphere a = { ca, ra };
			Sphere b = { cb, rb };
			overlapMovingSphereSphere(&a, &b, v, t0, t1);

			if (t0 > 1.0f)//no collision
				return false;

		}

		//narrow


		int numHits = 0;
		static const int MAX_HITS = 3;
		static const float MIN_INTERVAL = 0.01f;
		while (numHits < MAX_HITS && (t1 -t0) > MIN_INTERVAL)
		{
			float t = 0.5f *(t0 + t1);

			if (overlap(a, b, Transform(t*v, Quaternion(vec3(0, 0, 0), 1))))
			{
				++numHits;
				t1 = t;
			}
			else
			{
				vec3 pa = ca + t*v;
				if (dot(b->closestPoint(pa) - pa, v) > 0)
				{
					t0 = t;
				}
				else
				{
					t1 = t;
				}

			}
		}


		//scale to timeline
		t1 = t1 * (1.0f - tmin) + tmin;

		return t1;
	}
}