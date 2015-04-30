#pragma once

#include "myMath.h"

namespace ong
{



	class ShapePtr;
	struct Hull;
	struct Sphere;
	struct Capsule;

	struct MassData
	{
		mat3x3 I;
		vec3 cm;
		float m;
	};


	void calculateMassData(const ShapePtr shape, float density, MassData* data);
	void calculateHullMassData(const Hull* hull, float density, MassData* data);
	void calculateSphereMassData(const Sphere* sphere, float density, MassData* data);
	void calculateCapsuleMassData(const Capsule* capsule, float density, MassData* data);

}