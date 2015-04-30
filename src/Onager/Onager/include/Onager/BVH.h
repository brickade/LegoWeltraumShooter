#pragma once

#include "Shapes.h"
#include "defines.h"


namespace ong
{

	class Collider;

	struct NodeType
	{
		enum Type
		{
			LEAF, BRANCH
		};
	};

	struct BVTree
	{
		NodeType::Type type;

		AABB aabb;

		union
		{
			struct
			{
				uint16 left;
				uint16 right;
			};
			Collider* collider;
		};
	};

	// tree should be allocated with at least (numCollider + (numCollider-1))
	void constructBVTree(Collider* collider, int numCollider, BVTree* tree);
}