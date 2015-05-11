#pragma once

#include "Shapes.h"
#include <vector>
#include <Allocator.h>

namespace ong
{



	class Body;

	struct ProxyType
	{
		enum Type
		{
			Sphere,
			AABB,
			COUNT = AABB + 1
		};
	};

	struct Proxy
	{
		union
		{
			Sphere sphere;
			AABB aabb;
		};
		//Body* body;
		char type;
	};


	struct Pair
	{
		Body* A;
		Body* B;
	};

	struct ProxyID
	{
		Body* pBody;
		int level;
		int bucket;
		int idx;
	};

	class HGrid
	{
	public:
		static const int MAX_LEVELS = 20;
		static const int NUM_BUCKETS = 128; //todo test for good number
		static const float MIN_CELL_SIZE;
		static const float CELL_TO_CELL_RATIO;
		static const float SPHERE_TO_CELL_RATIO;
		
		HGrid();

		const ProxyID* addBody(Body* pBody);
		void removeBody(const ProxyID* pProxyID);
		
		void updateBody(const ProxyID* pProxyID);
		
		int generatePairs(Pair* pairs);

	private:

		struct Object
		{
			Sphere sphere;
			ProxyID* id;
		};


		int calculateBucketID(vec3 pos, int level);
		void removeFromBucket(const ProxyID* id);
		void removeFromLevel(const ProxyID* id);

		int m_occupiedLevelsMask;
		int m_objectsAtLevel[MAX_LEVELS];
		std::vector<Object> m_objectBucket[NUM_BUCKETS];
		int m_timeStamp[NUM_BUCKETS];
		int m_tick;


		Allocator<ProxyID> m_proxyIDAllocator;
	};

	inline int HGrid::calculateBucketID(vec3 pos, int level)
	{
		const int32 h1 = 1543;
		const int32 h2 = 3079;
		const int32 h3 = 6151;
		const int32 h4 = 12289;

		int n = pos.x*h1 + pos.y*h2 + pos.z * h3 + level * h4;
		n = n%NUM_BUCKETS;
		if (n < 0) n += NUM_BUCKETS;

		return n;
	}

	inline bool operator==(const Pair& lhs, const Pair& rhs)
	{
		return (lhs.A == rhs.A && lhs.B == rhs.B) || (lhs.A == rhs.B && lhs.B == rhs.A);
	}

}