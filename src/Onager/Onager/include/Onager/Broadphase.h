#pragma once

#include "Shapes.h"

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
		Body* body;
		char type;
	};


	struct Pair
	{
		Body* A;
		Body* B;
	};

	int generatePairs(Proxy* proxies, int numProxies, Pair* pairs);


	inline bool operator==(const Pair& lhs, const Pair& rhs)
	{
		return (lhs.A == rhs.A && lhs.B == rhs.B) || (lhs.A == rhs.B && lhs.B == rhs.A);
	}

}