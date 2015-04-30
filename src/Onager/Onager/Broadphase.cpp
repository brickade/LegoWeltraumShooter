#include "Broadphase.h"
#include <algorithm>
#include "Body.h"


namespace ong
{



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
		return overlap(&sphereProxy->aabb, &aabbProxy->aabb);
	}


	// returns num Pairs
	int generatePairs(Proxy* proxies, int numProxies, Pair* pairs)
	{
		typedef bool(*overlapProxyFunc)(Proxy* a, Proxy* b);
		static const overlapProxyFunc overlapFuncMatrix[ProxyType::COUNT][ProxyType::COUNT]
		{
			{overlapSphereSphere, overlapSphereAABB	},
			{ nullptr, overlapAABBAABB }
		};


		int numPairs = 0;
		for (int i = 0; i < numProxies - 1; ++i)
		{
			for (int j = i + 1; j < numProxies; ++j)
			{
				Proxy* a = proxies + i;;
				Proxy* b = proxies + j;

				if (a->body->getType() == BodyType::Static && b->body->getType() == BodyType::Static)
					continue;

				if (a->type > b->type)
					std::swap(a, b);

				if (overlapFuncMatrix[a->type][b->type](a, b))
				{
					Pair newPair{ a->body, b->body };
					bool duplicate = false;
					for (int i = 0; i < numPairs; ++i)
					{
						if (pairs[i] == newPair)
						{
							duplicate = true;
							break;
						}
					}

					if (!duplicate)
						pairs[numPairs++] = Pair{ a->body, b->body };
				}
			}
		}

		return numPairs;
	}

}