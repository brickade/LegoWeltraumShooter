#pragma once
#include "myMath.h"

namespace ong
{
	struct PositionState
	{
		vec3 p;
		Quaternion q;
	};

	struct VelocityState
	{
		vec3 v;
		vec3 w;
	};

	struct MomentumState
	{
		vec3 l;
		vec3 a;
	};


	struct MassState
	{
		mat3x3 localInvI;
		mat3x3 invI;
		float invM;
	};

	struct WorldContext
	{
		PositionState* r;

		VelocityState* v;

		MomentumState* p;

		MassState* m;

	};
}