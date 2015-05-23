#include "StressTest.h"


void StressTest::init()
{
	m_world = new World(vec3(0.0f, -10.0f, 0.0f));

	m_Material = { 1.0f, 0.0f, 0.1f };

	
	BodyDescription bodyDescr;
	bodyDescr.angularMomentum = vec3(0, 0, 0);
	bodyDescr.linearMomentum = vec3(0, 0, 0);
	bodyDescr.transform.p = vec3(0, 0, 0);
	bodyDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
	bodyDescr.type = BodyType::Static;

	ShapeDescription shapeDescr;
	shapeDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
	
	ColliderDescription colliderDescr;
	colliderDescr.material = &m_Material;
	colliderDescr.transform.p = vec3(0, 0, 0);
	colliderDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
	colliderDescr.isSensor = false;


	//walls
	{
		float wallSize = 10;

		Body* wall;
		{
			shapeDescr.hullFromBox.e = vec3(1, wallSize, wallSize);
			shapeDescr.hullFromBox.c = vec3(-wallSize, 0, 0);
			wall = m_world->createBody(bodyDescr);
			colliderDescr.shape = m_world->createShape(shapeDescr);
			wall->addCollider(m_world->createCollider(colliderDescr));
			m_entities.push_back(new Entity(wall, vec3(1, 0, 0)));
		}
		{
			shapeDescr.hullFromBox.e = vec3(1, wallSize, wallSize);
			shapeDescr.hullFromBox.c = vec3(wallSize, 0, 0);
			wall = m_world->createBody(bodyDescr);
			colliderDescr.shape = m_world->createShape(shapeDescr);
			wall->addCollider(m_world->createCollider(colliderDescr));
			m_entities.push_back(new Entity(wall, vec3(1, 0, 0)));
		}
		{
			shapeDescr.hullFromBox.e = vec3(wallSize, wallSize, 1);
			shapeDescr.hullFromBox.c = vec3(0, 0, -wallSize);
			wall = m_world->createBody(bodyDescr);
			colliderDescr.shape = m_world->createShape(shapeDescr);
			wall->addCollider(m_world->createCollider(colliderDescr));
			m_entities.push_back(new Entity(wall, vec3(1, 0, 0)));
		}
		{
			shapeDescr.hullFromBox.e = vec3(wallSize, wallSize, 1);
			shapeDescr.hullFromBox.c = vec3(0, 0, wallSize);
			wall = m_world->createBody(bodyDescr);
			colliderDescr.shape = m_world->createShape(shapeDescr);
			wall->addCollider(m_world->createCollider(colliderDescr));
			m_entities.push_back(new Entity(wall, vec3(1, 0, 0)));
		}
		{
			shapeDescr.hullFromBox.e = vec3(wallSize, 1, wallSize);
			shapeDescr.hullFromBox.c = vec3(0, 0, 0);
			bodyDescr.transform.p.y = -wallSize;
			wall = m_world->createBody(bodyDescr);
			colliderDescr.shape = m_world->createShape(shapeDescr);

			wall->addCollider(m_world->createCollider(colliderDescr));
			m_entities.push_back(new Entity(wall, vec3(1, 0, 0)));
		}
	}
	//

	
	bodyDescr.type = BodyType::Dynamic;
	
	//shapeDescr.hullFromBox.c = vec3(0, 0, 0);
	//shapeDescr.hullFromBox.e = vec3(0.5, 0.5, 0.5);

	shapeDescr.shapeType = ShapeType::SPHERE;
	shapeDescr.sphere.c = vec3(0, 0, 0);
	shapeDescr.sphere.r = 2.0f;

	ShapePtr shape = m_world->createShape(shapeDescr);

	colliderDescr.shape = shape;

	srand(123);

	int num = 1;
	int spacing = 6;

	for (int x = -num*spacing; x < num*spacing; x += spacing)
	{
		for (int z = -num*spacing; z < num*spacing; z += spacing)
		{
			for (int y = 20; y < 80; y += spacing)
			{
				bodyDescr.transform.p = vec3(x + rand() / (float)RAND_MAX, y, z + rand() / (float)RAND_MAX);
				
				Body* body = m_world->createBody(bodyDescr);
				body->addCollider(m_world->createCollider(colliderDescr));
				m_entities.push_back(new Entity(body, vec3(0, 0, 1)));
			}
		}
	}
}