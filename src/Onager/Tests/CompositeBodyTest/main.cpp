
#include "World.h"
#include "Body.h"

using namespace ong;

int main()
{
	World world;



	vec3 box[8] =
	{
		vec3(-1, -1, 1),
		vec3(-1, -1, -1),
		vec3(1, -1, -1),
		vec3(1, -1, 1),

		vec3(-1, 1, 1),
		vec3(-1, 1, -1),
		vec3(1, 1, -1),
		vec3(1, 1, 1)
	};

	vec3 boxSmall[8] =
	{
		vec3(-0.5, -1, 1),
		vec3(-0.5, -1, -1),
		vec3(0.5, -1, -1),
		vec3(0.5, -1, 1),

		vec3(-0.5, 1, 1),
		vec3(-0.5, 1, -1),
		vec3(0.5, 1, -1),
		vec3(0.5, 1, 1)
	};

	Material m;
	m.density = 1.0f;



	BodyDescription bodyDescr;
	bodyDescr.type = BodyType::Dynamic;
	bodyDescr.transform = Transform(vec3(0.0f, 0.0f, 0.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	bodyDescr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
	bodyDescr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);

	Body* body1 = world.createBody(bodyDescr);
	Body* body2 = world.createBody(bodyDescr);


	ShapeDescription shapeDescr;
	shapeDescr.type = ShapeConstruction::HULL_FROM_POINTS;
	shapeDescr.hullFromPoints.points = box;
	shapeDescr.hullFromPoints.numPoints = 8;

	ShapePtr shape = world.createShape(shapeDescr);

	ColliderDescription colliderDescr;
	colliderDescr.material = &m;
	colliderDescr.transform.p = vec3(0.0f, 0.0f, 0.0f);
	colliderDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	colliderDescr.shape = shape;

	Collider* colliderBig = world.createCollider(colliderDescr);

	body1->addCollider(colliderBig);


	colliderDescr.transform.p.x = -0.5f;

	shapeDescr.hullFromPoints.points = boxSmall;
	shape = world.createShape(shapeDescr);

	colliderDescr.shape = shape;

	Collider* colliderSmall1 = world.createCollider(colliderDescr);

	colliderDescr.transform.p.x = 0.5f;
	Collider* colliderSmall2 = world.createCollider(colliderDescr);

	body2->addCollider(colliderSmall1);
	body2->addCollider(colliderSmall2);



	return 0;

	


}


