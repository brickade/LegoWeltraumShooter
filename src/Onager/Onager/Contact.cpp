#include "Contact.h"
#include "defines.h"
#include <vector>

namespace ong
{



	void optimizeContactPoints(const std::vector<ContactPoint>& in, ContactManifold* manifold)
	{
		assert(in.size() > 4);

		manifold->numPoints = 4;

		// todo smart start point
		manifold->points[0] = in[0];

		int maxI = -1;
		float max = 0.0f;
		for (uint32 i = 0; i < in.size(); ++i)
		{
			float dist = lengthSq(in[i].position - manifold->points[0].position);
			if (dist > max)
				maxI = i, max = dist;
		}

		manifold->points[1] = in[maxI];

		maxI = -1;
		max = 0.0f;
		for (uint32 i = 0; i < in.size(); ++i)
		{
			vec3 CA = manifold->points[0].position - in[i].position;
			vec3 CB = manifold->points[1].position - in[i].position;

			float area = 0.5f * dot(cross(CA, CB), manifold->normal);

			if (abs(area) > abs(max))
				maxI = i, max = area;
		}


		if (maxI == -1)
		{
			manifold->numPoints = 2;
			return;
		}

		if (max < 0.0f)
			std::swap(manifold->points[0], manifold->points[1]);

		manifold->points[2] = in[maxI];


		maxI = -1;
		max = 0.0f;
		for (uint32 i = 0; i < in.size(); ++i)
		{
			for (int j = 0; j < 3; j++)
			{
				vec3 CA = manifold->points[j].position - in[i].position;
				vec3 CB = manifold->points[(j + 1) % 3].position - in[i].position;

				float area = -0.5f * dot(cross(CA, CB), manifold->normal);

				if (area > max)
					maxI = i, max = area;
			}
		}

		if (maxI == -1)
		{
			manifold->numPoints = 3;
			return;
		}

		manifold->points[3] = in[maxI];

	}

}