#include "TimeOfImpact.h"
#include "myMath.h"
#include "Shapes.h"
#include "Body.h"

namespace ong
{

	float getTimeOfImpact(Body* a, Body* b)
	{

		int t0 = 0.0f;
		int t1 = 1.0f;
		
		AABB aabb = a->getAABB();

		vec3 v = a->getLinearVelocity();
		
		if (b->getContinuousPhysics())
			v -= b->getLinearVelocity();

		while (true)
		{		   
			//make aabb
			AABB aabb0 = aabb, aabb1 = aabb;
			aabb0.c += t0*v;
			aabb1.c += t1*v;
			mergeAABBAABB(&aabb0, &aabb1);

			//test aabb

			if (overlap(&aabb0, &b->getAABB()))
			{

			}

		}


		return t1;
	}
}