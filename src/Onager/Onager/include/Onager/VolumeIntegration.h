#pragma once


#include "myMath.h"

namespace ong
{




	//
	//explanation see
	//http://www.cs.upc.edu/~virtual/SGI/docs/3.%20Further%20Reading/Fast%20and%20accurate%20computation%20of%20polyhedral%20mass%20properties.pdf
	//


	struct Hull;

	struct VolumeIntegrals
	{
		float t0;
		vec3 t1;
		vec3 t2;
		vec3 tp;
	};




	void computeVolumeIntegrals(const Hull* hull, VolumeIntegrals* integrals);

}