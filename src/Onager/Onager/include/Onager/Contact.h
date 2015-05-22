#pragma once

#include "myMath.h"
#include <vector>


namespace ong
{



	class Collider;
	class Body;

	const int MAX_CONTACT_POINTS = 4;

	struct ContactPoint
	{
		vec3 position;
		float penetration;
	};

	struct ContactManifold
	{
		int numPoints;
		ContactPoint points[MAX_CONTACT_POINTS];
		vec3 normal;
	};

	struct Feature
	{
		enum Type
		{
			HULL_EDGE,
			HULL_FACE,
			NONE
		} type;

		union
		{
			struct
			{
				int edge1;
				int edge2;
			} hullEdge;

			struct
			{
				int face1;
				int face2;
			} hullFace;
		};

	};

	inline bool operator==(const Feature& lhs, const Feature& rhs)
	{
		if (lhs.type != rhs.type)
			return false;

		switch (lhs.type)
		{
		case Feature::HULL_EDGE: return lhs.hullEdge.edge1 == rhs.hullEdge.edge1 && lhs.hullEdge.edge2 == rhs.hullEdge.edge2;
		case Feature::HULL_FACE: return lhs.hullFace.face1 == rhs.hullFace.face1 && lhs.hullFace.face2 == rhs.hullFace.face2;
		case Feature::NONE: return true;
		default: return false;
		}
	}




	struct Contact
	{
		Collider* colliderA;
		Collider* colliderB;

		Feature feature;

		vec3 tangent;
		vec3 biTangent;

		float accImpulseN[MAX_CONTACT_POINTS]; // normal impulse
		float accImpulseT[MAX_CONTACT_POINTS]; // tangent impulse
		float accImpulseBT[MAX_CONTACT_POINTS]; // bitangent impulse

		float massN[MAX_CONTACT_POINTS]; // mass along normal
		float massT[MAX_CONTACT_POINTS]; // mass along tangent
		float massBT[MAX_CONTACT_POINTS]; // mass along bitangent

		vec3 rA[MAX_CONTACT_POINTS]; 
		vec3 rB[MAX_CONTACT_POINTS]; 

		float friction;
		float e; // restitution
		ContactManifold manifold;

		int tick; //last update
	};


	struct ContactIter
	{
		Body* other;

		int dir;

		float impulse[MAX_CONTACT_POINTS];

		Contact* contact;
		ContactIter* next;
		ContactIter* prev;
	};




	void optimizeContactPoints(const std::vector<ContactPoint>& in, ContactManifold* manifold);


}