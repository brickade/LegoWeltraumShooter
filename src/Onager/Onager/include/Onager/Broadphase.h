#pragma once
#include "defines.h"
#include "Callbacks.h"
#include "Shapes.h"
#include <vector>
#include "Allocator.h"

namespace ong
{
	class Body;
	class Collider;
	struct RayQueryResult;


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
		static const int MAX_LEVELS = 100;
		static const int NUM_BUCKETS = 1024; //todo test for good number
		static const float MIN_CELL_SIZE;
		static const float CELL_TO_CELL_RATIO;
		static const float SPHERE_TO_CELL_RATIO;
		
		HGrid();

		const ProxyID* addBody(Body* pBody);
		void removeBody(const ProxyID* pProxyID);
		
		void updateBody(const ProxyID* pProxyID);
		
		int generatePairs(Pair* pairs);

		bool queryRay(const vec3& origin, const vec3& dir, RayQueryResult* hit, float tmax = FLT_MAX);
		bool queryCollider(const Collider* collider);
		bool queryCollider(Collider* collider, ColliderQueryCallBack callback);
		bool queryShape(ShapePtr shape, const Transform& transform);
		bool queryShape(ShapePtr shape, const Transform& transform, ShapeQueryCallBack callback, void* userData);

	private:

		struct Object
		{
			Sphere sphere;
			ProxyID* id;
			//debug
			int x, y, z;
		};


		int calculateBucketID(int x, int y, int z, int level);
		void removeFromBucket(const ProxyID* id);
		void removeFromLevel(const ProxyID* id);

		int m_occupiedLevelsMask;
		int m_objectsAtLevel[MAX_LEVELS];
		std::vector<Object> m_objectBucket[NUM_BUCKETS];
		int m_timeStamp[NUM_BUCKETS];
		int m_tick;

		vec3 m_minExtend;
		vec3 m_maxExtend;

		Allocator<ProxyID> m_proxyIDAllocator;
	};

	inline int HGrid::calculateBucketID(int x, int y, int z, int level)
	{
		const int32 h1 = 12582917;
		const int32 h2 = 25165843;
		const int32 h3 = 50331653;
		const int32 h4 = 100663319;

		int n = x*h1 + y*h2 + z * h3 + level * h4;

		//elf hash
		unsigned char* p = (unsigned char*)&n;
		int h = 0, g;
		for (int i = 0; i < sizeof(int); ++i)
		{
			h = (h << 4) + p[i];
			g = h & 0xf0000000L;
			if (g != 0)
			{
				h ^= g >> 24;
			}
			
			h &= ~g;
		}

		h = h%NUM_BUCKETS;
		if (h < 0) h += NUM_BUCKETS;


		h = h%NUM_BUCKETS;
		return h;
	}

	inline bool operator==(const Pair& lhs, const Pair& rhs)
	{
		return (lhs.A == rhs.A && lhs.B == rhs.B) || (lhs.A == rhs.B && lhs.B == rhs.A);
	}

}