#include "BVH.h"


#include "Collider.h"
#include <algorithm>
#include <stack>


namespace ong
{
	typedef std::stack<int> Stack;

	struct BVDynTree
	{
		NodeType::Type type;

		AABB aabb;
		union
		{
			struct
			{
				BVDynTree* left;
				BVDynTree* right;
			};
			Collider* collider;
		};
	};



	// todo non recursive
	void buildBranch(BVDynTree* node, BVDynTree* leafs, int numLeafs, BVDynTree** head)
	{
		node->type = NodeType::BRANCH;
		node->aabb = { vec3(0, 0, 0), vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX) };
		node->left = nullptr;
		node->right = nullptr;

		// create aabb
		for (int i = 0; i < numLeafs; ++i)
		{
			mergeAABBAABB(&node->aabb, &leafs[i].aabb);
		}



		int axis = (node->aabb.e.x > node->aabb.e.y && node->aabb.e.x > node->aabb.e.z) ? 0 :
			node->aabb.e.y > node->aabb.e.z ? 1 : 2;


		// seperate leafs
		int i = 0, mid = numLeafs;
		while (i < mid)
		{
			if (leafs[i].aabb.c[axis] < node->aabb.c[axis])
				++i;
			else
				std::swap(leafs[i], leafs[--mid]);
		}

		// if all leafs lie on one side seperate them in their middle
		if (mid == 0 || mid == numLeafs)
			mid = numLeafs / 2;

		// build new branches
		if (mid > 1)
		{
			node->left = (*head)++;
			buildBranch(node->left, leafs, mid, head);
		}
		else
		{
			node->left = leafs;
		}

		if (numLeafs - mid > 1)
		{
			node->right = (*head)++;
			buildBranch(node->right, leafs + mid, numLeafs - mid, head);
		}
		else
		{
			node->right = leafs + mid;
		}
	}

	//todo non recursiv

	static const int STACK_SIZE = 100;
	int preorderOutput(BVTree* sT, BVDynTree* dT, int i, Stack& parentStack)
	{
		sT[i].type = dT->type;
		sT[i].aabb = dT->aabb;

		if (sT[i].type == NodeType::LEAF)
		{
			sT[i].collider = dT->collider;
			return i;
		}

		if (dT->right)
		{
			parentStack.push(i);
		}



		if (dT->left)
		{
			sT[i].left = i + 1;
			i = preorderOutput(sT, dT->left, i + 1, parentStack);
		}
		else
		{
			sT[i].left = -1;
		}

		if (dT->right)
		{
			int p = parentStack.top();
			parentStack.pop();

			sT[p].right = i + 1;
			i = preorderOutput(sT, dT->right, i + 1, parentStack);
		}
		else
		{
			sT[i].right = -1;
		}


		return i;
	}

	void constructStaticBVTree(BVTree* staticTree, BVDynTree* dynamicTree)
	{
		Stack parentStack;

		preorderOutput(staticTree, dynamicTree, 0, parentStack);
	}

	void constructBVTree(Collider* collider, int numCollider, BVTree* tree)
	{
		BVDynTree* dynTree = new BVDynTree[numCollider + (numCollider - 1)];

		Collider* c = collider;
		for (int i = 0; i < numCollider; ++i)
		{
			assert(c != nullptr);

			dynTree[i].type = NodeType::LEAF;
			dynTree[i].aabb = c->getAABB();
			dynTree[i].collider = c;


			c = c->getNext();
		}

		BVDynTree* head = dynTree + numCollider + 1;

		buildBranch(head - 1, dynTree, numCollider, &head);

		constructStaticBVTree(tree, dynTree + numCollider);

		delete[] dynTree;
	}

}