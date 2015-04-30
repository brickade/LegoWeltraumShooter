#pragma once

#include "myMath.h"

namespace ong
{




	struct FaceQuery
	{
		int index;
		float separation;
	};

	struct EdgeQuery
	{
		int index1;
		int index2;
		float separation;
	};

	struct Hull;
	struct ContactManifold;
	struct Feature;

	void SAT(const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, ContactManifold* manifold, Feature* feature = nullptr);

	void queryFaceDirections(const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, FaceQuery* out);
	void queryEdgeDirections(const Hull* hull1, const Transform* t1, const Hull* hull2, const Transform* t2, EdgeQuery* out);


}