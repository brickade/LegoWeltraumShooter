#pragma once



namespace ong
{

	class Body;
	struct ContinuousState;


	float getTimeOfImpact(Body* a, Body* b, ContinuousState* cpS, float tmin);
}
