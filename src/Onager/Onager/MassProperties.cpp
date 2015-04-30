#include "MassProperties.h"
#include "Shapes.h"
#include "VolumeIntegration.h"

namespace ong
{



	void calculateMassData(const ShapePtr shape, float density, MassData* data)
	{
		switch (shape.getType())
		{
		case ShapeType::HULL:
			calculateHullMassData(shape, density, data);
			break;
		case ShapeType::SPHERE:
			calculateSphereMassData(shape, density, data);
			break;
		case ShapeType::CAPSULE:
			calculateCapsuleMassData(shape, density, data);
			break;
		}
	}

	void calculateHullMassData(const Hull* hull, float density, MassData* data)
	{

		VolumeIntegrals I;
		computeVolumeIntegrals(hull, &I);

		float m = density * I.t0;

		vec3 r = 1.0f / I.t0 * I.t1;


		mat3x3 J;

		J[0][0] = density * (I.t2.y + I.t2.z);
		J[1][1] = density * (I.t2.z + I.t2.x);
		J[2][2] = density * (I.t2.x + I.t2.y);

		J[0][1] = J[1][0] = -density * I.tp.x;
		J[1][2] = J[2][1] = -density * I.tp.y;
		J[2][0] = J[0][2] = -density * I.tp.z;

		J[0][0] -= m * (r.y*r.y + r.z*r.z);
		J[1][1] -= m * (r.z*r.z + r.x*r.x);
		J[2][2] -= m * (r.x*r.x + r.y*r.y);

		J[0][1] = J[1][0] += m * r.x * r.y;
		J[1][2] = J[2][1] += m * r.y * r.z;
		J[2][0] = J[0][2] += m * r.z * r.x;

		data->cm = r;
		data->m = m;
		data->I = J;


	}

	void calculateSphereMassData(const Sphere* sphere, float density, MassData* data)
	{

		float m = density * 4.0f / 3.0f * ong_PI * sphere->r*sphere->r*sphere->r;
		float i = 2.0f / 5.0f * m * sphere->r*sphere->r;

		data->cm = sphere->c;
		data->m = m;
		data->I = mat3x3(
			vec3(i, 0.0f, 0.0f),
			vec3(0.0f, i, 0.0f),
			vec3(0.0f, 0.0f, i));

		data->I = data->I + m * (dot(data->cm, data->cm) * identity() - outerproduct(data->cm, data->cm));
	}

	void calculateCapsuleMassData(const Capsule* capsule, float density, MassData* data)
	{
		float h = length(capsule->c2 - capsule->c1);
		float r = capsule->r;

		float mCy = density * h * r*r * ong_PI;
		float mHs = density * 2.0f / 3.0f * r*r*r * ong_PI;
		float m = mCy + 2.0f*mHs;

		vec3 up = normalize(capsule->c2 - capsule->c1);
		vec3 l;
		if (abs(up.x) < abs(up.y) && abs(up.y) < abs(up.z))
			l = normalize(cross(vec3(1, 0, 0), up));
		else if (abs(up.y) < abs(up.z))
			l = normalize(cross(vec3(0, 1, 0), up));
		else
			l = normalize(cross(vec3(0, 0, 1), up));
		vec3 s = normalize(cross(up, l));

		mat3x3 rot(
			vec3(s.x, up.x, l.x),
			vec3(s.y, up.y, l.y),
			vec3(s.z, up.z, l.z));


		data->cm = 0.5f * (capsule->c1 + capsule->c2);
		data->m = m;
		data->I = identity();

		data->I[0][0] = data->I[2][2] = mCy*(h*h / 12.0f + r*r / 4.0f) + 2 * mHs*(2.0f / 5.0f*r*r + h*h / 2.0f + 3.0f / 8.0f*h*r);
		data->I[1][1] = mCy*(r*r / 2.0f) + 2.0f*mHs*(2.0f / 5.0f*r*r);

		data->I = rot * data->I * transpose(rot);
		data->I = data->I + m * (dot(data->cm, data->cm) * identity() - outerproduct(data->cm, data->cm));

	}

}