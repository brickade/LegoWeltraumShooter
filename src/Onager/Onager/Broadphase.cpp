#include "Broadphase.h"
#include "Body.h"
#include <algorithm>
#include <stack>
#include "Profiler.h"


namespace ong
{


	const float HGrid::CELL_TO_CELL_RATIO = 2.0f;
	const float HGrid::MIN_CELL_SIZE = 1.0f;
	const float HGrid::SPHERE_TO_CELL_RATIO = 0.25f;


	HGrid::HGrid()
		: m_proxyIDAllocator(128 / sizeof(ProxyID)),
		m_tick(0),
		m_occupiedLevelsMask(0),
		m_minExtend(0, 0, 0),
		m_maxExtend(0,0,0)
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

		for (int i = 0; i < 3; ++i)
		{
			if ((object.sphere.c[i] + object.sphere.r) > m_maxExtend[i])
				m_maxExtend[i] = object.sphere.c[i] + object.sphere.r;
			else if ((object.sphere.c[i] - object.sphere.r) < m_minExtend[i])
				m_minExtend[i] = object.sphere.c[i] + object.sphere.r;
		}


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


		for (int i = 0; i < 3; ++i)
		{
			if ((object.sphere.c[i] + object.sphere.r) > m_maxExtend[i])
				m_maxExtend[i] = object.sphere.c[i] + object.sphere.r;
			else if ((object.sphere.c[i] - object.sphere.r) < m_minExtend[i])
				m_minExtend[i] = object.sphere.c[i] + object.sphere.r;
		}

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



	// returns num Pairs
	int HGrid::generatePairs(Pair* pairs)
	{

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

									if (overlap(&a->getAABB(), &b->getAABB()))
									{
										if (a->getType() == BodyType::Static && b->getType() == BodyType::Static)
											continue;

										pairs[numPairs++] = Pair{ a, b };

									}

								}


							}

						}
					}

				}
			}
		}


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




	bool HGrid::queryRay(const vec3& origin, const vec3& dir, RayQueryResult* hit, float tmax)
	{

		m_tick++;

		int i[MAX_LEVELS];
		int j[MAX_LEVELS];
		int k[MAX_LEVELS];

		int iEnd[MAX_LEVELS];
		int jEnd[MAX_LEVELS];
		int kEnd[MAX_LEVELS];

			
		vec3 t[MAX_LEVELS];
		vec3 d[MAX_LEVELS];

		int di = ((dir.x > 0) ? 1 : (dir.x < 0) ? -1 : 0);
		int dj = ((dir.y > 0) ? 1 : (dir.y < 0) ? -1 : 0);
		int dk = ((dir.z > 0) ? 1 : (dir.z < 0) ? -1 : 0);


		float maxSize = MIN_CELL_SIZE;
		int occupiedLevelsMask = m_occupiedLevelsMask;
		int maxLevel = 0;
		for (maxLevel = 0; maxLevel < MAX_LEVELS; ++maxLevel, occupiedLevelsMask >>= 1)
		{
			if (occupiedLevelsMask == 0)
				break;
			maxSize *= CELL_TO_CELL_RATIO;
		}


		vec3 end = origin + maxSize * dir;

		float size= MIN_CELL_SIZE;
		for (int level = 0; level < maxLevel; ++level)
		{
		
			i[level] = (int)floorf(origin.x / size);
			j[level] = (int)floorf(origin.y / size);
			k[level] = (int)floorf(origin.z / size);

			iEnd[level] = (int)floorf(end.x / size);
			jEnd[level] = (int)floorf(end.y / size);
			kEnd[level] = (int)floorf(end.z / size);

			float minx = size * floorf(origin.x / size);
			float miny = size * floorf(origin.y / size);
			float minz = size * floorf(origin.z / size);

			float maxx = minx + size;
			float maxy = miny + size;
			float maxz = minz + size;

			t[level].x = ((dir.x < 0) ? (origin.x - minx) : (maxx - origin.x)) / abs(dir.x);
			t[level].y = ((dir.y < 0) ? (origin.y - miny) : (maxy - origin.y)) / abs(dir.y);
			t[level].z = ((dir.z < 0) ? (origin.z - miny) : (maxy - origin.z)) / abs(dir.z);

			d[level].x = size / abs(dir.x);
			d[level].y = size / abs(dir.y);
			d[level].z = size / abs(dir.z);

			size *= CELL_TO_CELL_RATIO;
		}


		iEnd[maxLevel] = (int)floorf((di >= 0 ? m_maxExtend.x : m_minExtend.x) / maxSize);
		jEnd[maxLevel] = (int)floorf((dj >= 0 ? m_maxExtend.y : m_minExtend.y) / maxSize);
		kEnd[maxLevel] = (int)floorf((dk >= 0 ? m_maxExtend.z : m_minExtend.z) / maxSize);

		RayQueryResult minResult;
		minResult.collider = 0;
		minResult.t = FLT_MAX;



		for (;;)
		{


			float ooSize = 1.0f / maxSize;
		   
			for (int l = maxLevel - 1; l >= 0; --l)
			{
				ooSize *= CELL_TO_CELL_RATIO;

				if (m_objectsAtLevel[l] == 0)
					continue;



				for (;;)
				{


					vec3 boundary; 
					boundary.x = origin.x[l]

					int x0 = (int)floorf(boundary.x - size*SPHERE_TO_CELL_RATIO) * ooSize;
					int y0 = (int)floorf(boundary.y - size*SPHERE_TO_CELL_RATIO) * ooSize;
					int z0 = (int)floorf(boundary.z - size*SPHERE_TO_CELL_RATIO) * ooSize;
																				 
					int x1 = (int)floorf(boundary.x + size*SPHERE_TO_CELL_RATIO) * ooSize;
					int y1 = (int)floorf(boundary.y + size*SPHERE_TO_CELL_RATIO) * ooSize;
					int z1 = (int)floorf(boundary.z + size*SPHERE_TO_CELL_RATIO) * ooSize;

					for (int x = x0; x <= x1; ++x)
					{
						for (int y = y0; y <= y1; ++y)
						{
							for (int z = z0; z <= z1; ++z)
							{

								//int bucket = calculateBucketID(i[l] + x, j[l] + y, k[l] + z, l);
								int bucket = calculateBucketID(x, y, z, l);
								if (m_timeStamp[bucket] == m_tick)
									continue;
								m_timeStamp[bucket] = m_tick;

								for (Object& obj : m_objectBucket[bucket])
								{
									float tmin;
									vec3 p;
									if (intersectRayAABB(origin, dir, obj.id->pBody->getAABB(), tmin, p) && tmin < tmax)
									{
										RayQueryResult result = { 0 };
										if (obj.id->pBody->queryRay(origin, dir, &result, tmax))
										{
											if (result.t < minResult.t)
												minResult = result;
										}
									}
								}

							}
						}
					}

					if (t[l].x <= t[l].y && t[l].x <= t[l].z)
					{

						t[l].x += d[l].x;
						i[l] += di;

						if ((int)floorf(i[l]/(maxSize*ooSize)) != i[maxLevel])
							break;
					}
					else if (t[l].y <= t[l].z)
					{

						t[l].y += d[l].y;
						j[l] += dj;

						if ((int)floorf(j[l] / (maxSize*ooSize)) != j[maxLevel])
							break;
					}
					else
					{
						t[l].z += d[l].z;
						k[l] += dk;

						if ((int)floorf(k[l] / (maxSize*ooSize)) != k[maxLevel])
							break;
					}
				}

				//iEnd[l] += (int)floorf((maxSize * dir.x) * ooSize);
				//jEnd[l] += (int)floorf((maxSize * dir.y) * ooSize);
				//kEnd[l] += (int)floorf((maxSize * dir.z) * ooSize);
			}

			if (minResult.collider != 0)
			{
				*hit = minResult;
				return true;
			}

			if (t[maxLevel].x <= t[maxLevel].y && t[maxLevel].x <= t[maxLevel].z)
			{
				if (i[maxLevel] == iEnd[maxLevel])
					return false;
				t[maxLevel].x += d[maxLevel].x;
				i[maxLevel] += di;

			}
			else if (t[maxLevel].y <= t[maxLevel].z)
			{
				if (j[maxLevel] == jEnd[maxLevel])
					return false;
				t[maxLevel].y += d[maxLevel].y;
				j[maxLevel] += dj;

			}
			else
			{
				if (k[maxLevel] == kEnd[maxLevel]) 
					return false;
				t[maxLevel].z += d[maxLevel].z;
				k[maxLevel] += dk;

			}

		}
	}
}