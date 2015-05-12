#include "Broadphase.h"
#include <algorithm>
#include "Body.h"


namespace ong
{


	const float HGrid::CELL_TO_CELL_RATIO = 2.0f;
	const float HGrid::MIN_CELL_SIZE = 1.0f;
	const float HGrid::SPHERE_TO_CELL_RATIO = 0.25f;

	static inline bool overlapSphereSphere(Proxy* sphereProxy, Proxy* aabbProxy)
	{
		return overlap(&sphereProxy->sphere, &aabbProxy->sphere);
	}

	static inline bool overlapAABBAABB(Proxy* aabbProxy1, Proxy* aabbProxy2)
	{
		return overlap(&aabbProxy1->aabb, &aabbProxy2->aabb);
	}

	static inline bool overlapSphereAABB(Proxy* sphereProxy, Proxy* aabbProxy)
	{
		return overlap(&sphereProxy->sphere, &aabbProxy->aabb);
	}



	HGrid::HGrid()
		: m_proxyIDAllocator(128 / sizeof(ProxyID)),
		m_tick(0),
		m_occupiedLevelsMask(0)
	{
		m_timeStamp;
		memset(m_timeStamp, 0, sizeof(int) * NUM_BUCKETS);


	}

	const ProxyID* HGrid::addBody(Body* pBody)
	{
		AABB aabb = pBody->getAABB();
		ProxyID* id = m_proxyIDAllocator();

		Object object;
		object.id = id;

		object.sphere.c = aabb.c;
		object.sphere.r = 0;

		object.sphere.r = length(aabb.e);


		float size = MIN_CELL_SIZE, diameter = 2.0f * object.sphere.r;
		for (id->level = 0; size* SPHERE_TO_CELL_RATIO < diameter; ++id->level)
			size *= CELL_TO_CELL_RATIO;

		assert(id->level < MAX_LEVELS);

		int x = (int)object.sphere.c.x / size;
		int y = (int)object.sphere.c.y / size;
		int z = (int)object.sphere.c.z / size;

		object.x = x;
		object.y = y;
		object.z = z;

		id->bucket = calculateBucketID(x,y,z, id->level);
		id->idx = m_objectBucket[id->bucket].size();
		id->pBody = pBody;


		m_objectBucket[id->bucket].push_back(object);

		m_objectsAtLevel[id->level]++;
		m_occupiedLevelsMask |= (1 << id->level);

		return id;
	}
	
	void HGrid::removeBody(const ProxyID* pProxyID)
	{
		ProxyID* id = m_objectBucket[pProxyID->bucket][pProxyID->idx].id;
		removeFromLevel(pProxyID);
		removeFromBucket(pProxyID);

		m_proxyIDAllocator.sDelete(id);
	}

	void HGrid::updateBody(const ProxyID* pProxyID)
	{
		AABB aabb = pProxyID->pBody->getAABB();

		Object object = m_objectBucket[pProxyID->bucket][pProxyID->idx];

		object.sphere.c = aabb.c;
		object.sphere.r = 0;

		object.sphere.r = length(aabb.e);

		int level = 0;
		float size = MIN_CELL_SIZE, diameter = 2.0f * object.sphere.r;
		for (level = 0; size* SPHERE_TO_CELL_RATIO < diameter; ++level)
			size *= CELL_TO_CELL_RATIO;

		assert(level < MAX_LEVELS);


		ProxyID* id = object.id;
		
		assert(id == pProxyID);

		if (level != id->level)
		{
			removeFromLevel(pProxyID);
			id->level = level;

			m_objectsAtLevel[id->level]++;
			m_occupiedLevelsMask |= (1 << id->level);
		}

		int x = (int)object.sphere.c.x / size;
		int y = (int)object.sphere.c.y / size;
		int z = (int)object.sphere.c.z / size;

		object.x = x;
		object.y = y;
		object.z = z;

		int bucket = calculateBucketID(x,y,z, level);
		if (bucket != pProxyID->bucket)
		{
			removeFromBucket(pProxyID);

			id->bucket = bucket;
			id->idx = m_objectBucket[id->bucket].size();
			m_objectBucket[id->bucket].push_back(object);
		}

	}


	//todo get rid of dulpicates
	// maybe smarter checking
	// first check own level like
	// a list, should hopefully get rid
	// of all duplicates

	// returns num Pairs
	int HGrid::generatePairs(Pair* pairs)
	{
		//Debug
		int numTests = 0;
		int numLevels = 0;

		int pairTick = ++m_tick;


		typedef bool(*overlapProxyFunc)(Proxy* a, Proxy* b);
		static const overlapProxyFunc overlapFuncMatrix[ProxyType::COUNT][ProxyType::COUNT]
		{
			{overlapSphereSphere, overlapSphereAABB	},
			{ nullptr, overlapAABBAABB }
		};

		int numPairs = 0;

		for (int bucket = 0; bucket < NUM_BUCKETS; ++bucket)
		{

			for (int j = 0; j < m_objectBucket[bucket].size(); ++j)
			{
				Object &obj = m_objectBucket[bucket][j];

				float size = MIN_CELL_SIZE;
				int startLevel = 0;

				int occupiedLevelsMask = m_occupiedLevelsMask;
				vec3 pos = obj.sphere.c;
				float radius = obj.sphere.r;

				for (; size * SPHERE_TO_CELL_RATIO < 2.0f * obj.sphere.r; ++startLevel)
				{
					size *= CELL_TO_CELL_RATIO;
					occupiedLevelsMask >>= 1;
				}

				m_tick++;

				for (int level = startLevel; level < MAX_LEVELS;
					size *= CELL_TO_CELL_RATIO, occupiedLevelsMask >>= 1, ++level)
				{
					if (occupiedLevelsMask == 0)
						break;

					if ((occupiedLevelsMask & 1) == 0) 
						continue;

					//debug
					numLevels++;

					float ooSize = 1.0f / size;
					
					int x1, y1, z1, x2, y2, z2;

					float delta = radius + size * SPHERE_TO_CELL_RATIO + FLT_EPSILON * size;
					x1 = (int)floorf((pos.x - delta) * ooSize);
					y1 = (int)floorf((pos.y - delta) * ooSize);
					z1 = (int)floorf((pos.z - delta) * ooSize);

					x2 = (int)ceilf((pos.x + delta) * ooSize);
					y2 = (int)ceilf((pos.y + delta) * ooSize);
					z2 = (int)ceilf((pos.z + delta) * ooSize);



					for (int x = x1; x <= x2; ++x)
					{
						for (int y = y1; y <= y2; ++y)
						{
							for (int z = z1; z <= z2; ++z)
							{
								int bucket2 = calculateBucketID(x, y, z, level);

								if (m_timeStamp[bucket2] == m_tick)
									continue;
								m_timeStamp[bucket2] = m_tick;

								int startIndex = bucket == bucket2 ? j + 1 : 0;

								for (int k = startIndex; k < m_objectBucket[bucket2].size(); ++k)
								{
									Object& obj2 = m_objectBucket[bucket2][k];

									//skip if objects are the same
									//or if an object of an higher level is checked against an object with an lower level
									//or if an bucket with an higher index is checked against a bucket with a lower index if they are on the same level
									//or if buckets are the same but index of object a is higher than index of object b
									if (obj.id->level > obj2.id->level || (obj.id->level == obj2.id->level && bucket > bucket2))
										continue;


									Body* a = obj.id->pBody;
									Body* b = obj2.id->pBody;

									//debug
									numTests++;
									if (overlap(&a->getAABB(), &b->getAABB()))
									{
										if (a->getType() == BodyType::Static && b->getType() == BodyType::Static)
											continue;
	   
										//Pair newPair{ a, b };
										//bool duplicate = false;
										//for (int i = 0; i < numPairs; ++i)
										//{
										//	if (pairs[i] == newPair)
										//	{
										//		duplicate = true;
										//		break;
										//	}
										//}

										//if (!duplicate)
										//	pairs[numPairs++] = Pair{ a, b};
										pairs[numPairs++] = Pair{ a, b };

									}

								}


							}

						}
					}

				}
			}
		}

		//debug
		printf("numTests: %d numLevels: %d\n", numTests, numLevels);

		return numPairs;
	}


	void HGrid::removeFromBucket(const ProxyID* id)
	{
		if (m_objectBucket[id->bucket].size() > 1)
		{
			m_objectBucket[id->bucket][id->idx] = m_objectBucket[id->bucket].back();
			ProxyID* pOtherProxy = m_objectBucket[id->bucket][id->idx].id;
			pOtherProxy->idx = id->idx;
			m_objectBucket[id->bucket].pop_back();
		}
		else
		{
			m_objectBucket[id->bucket].clear();
		}
	}

	void HGrid::removeFromLevel(const ProxyID* id)
	{
		if (--m_objectsAtLevel[id->level] == 0)
			m_occupiedLevelsMask &= ~(1 << id->level);
	}
}