#pragma once


#include "myMath.h"

namespace ong
{
	struct Hull;


	void quickHull(vec3* points, int numPoints, Hull* hull);
}