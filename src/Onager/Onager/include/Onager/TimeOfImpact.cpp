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
		static const float MIN_INTERVAL = 0;

		//  
		//	first test moving bounding spheres to get approximate
		//	start and end  points
		//  then start with point in the middle and test for overlap
		//	if hit move half distance in the direction of the other body
	    //  else move half distance to the right.
		//  repeat until 3 hit or so to get an approximate
		//  time of impact
		//

		float t0 = tmin;
		float t1 = 1.0f;
		
		
		float ra = length(a->getAABB().e);
		float rb = length(b->getAABB().e);
		vec3 ca = a->getAABB().c;
		vec3 cb = b->getAABB().c;


		//Transform ta = a->getTransform();
		//Transform tb = b->getTransform();

		vec3 v = vec3(0,0,0);

		if (a->getContinuousPhysics())
			v += cpS[a->getCpIndex()].p1 - cpS[a->getCpIndex()].p0;
		if (b->getContinuousPhysics())
			v -= cpS[b->getCpIndex()].p1 - cpS[b->getCpIndex()].p0;
		
		float absV = length(v);

		//broad		
		{
			Sphere a = { ca, ra };
			Sphere b = { cb, rb };
			if (!overlapMovingSphereSphere(&a, &b, v, t0, t1) || t0 > 1.0f)
			{
				//no collision 
				return 1.0f;
			}
		}

		//narrow
		int numHits = 0;
		static const int MAX_HITS = 3;
		while (numHits < MAX_HITS)
		{
			float t = 0.5f *(t0 + t1);

			if (overlap(a, b, Transform(t*v, Quaternion(vec3(0, 0, 0), 1))))
			{
				++numHits;
				t1 = t;
			}
			else
			{
				if (dot(cb - (ca + t*v), v) > 0)
				{
					t0 = t;
				}
				else
				{
					t1 = t;
				}

			}
		}

		return t1;
	}
}