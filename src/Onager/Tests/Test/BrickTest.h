#pragma once

#include "test.h"


class BrickTest : public Test
{
public:
	virtual void init()
	{
		m_world = new World();
		
		ShapeDescription shapeDescr;

		shapeDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		shapeDescr.hullFromBox.c = ong::vec3(-0.39, 0.479, 2.339);
		shapeDescr.hullFromBox.e = ong::vec3(-0.39, 0.479, 2* 2.339);

		
		ColliderDescription cDescr;
		cDescr.material = m_world->createMaterial({ 1.0, 1.0, 1.0 });
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
		cDescr.isSensor = false;

		cDescr.shape = m_world->createShape(shapeDescr);
		
		Collider* testA = m_world->createCollider(cDescr);

		shapeDescr.hullFromBox.c = ong::vec3(0, 0, 0);
		cDescr.shape = m_world->createShape(shapeDescr);
		cDescr.transform.p = ong::vec3(-0.39, 0.479, 2.339);

		
		Collider* testB = m_world->createCollider(cDescr);

		//shapeDescr.hullFromBox.c = ong::vec3(0, 0, 0);

		BodyDescription bDescr;
		bDescr.angularMomentum = vec3(0.01, 0.01, 200);
		bDescr.linearMomentum = vec3(0, 0, 0);
		bDescr.transform.p = vec3(0, 0, 0);
		bDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

		Body* bodyA = m_world->createBody(bDescr);

		//bDescr.transform.p.x = -5;


		//Body* bodyB = m_world->createBody(bDescr);


		//bodyA->addCollider(testA);

		////ColliderData data = bodyA->getCollider()->getData();

		//bodyB->addCollider(testB);


		//m_entities.push_back(new Entity(bodyA, vec3(1, 0, 0)));
		//m_entities.push_back(new Entity(bodyB, vec3(0, 1, 0)));


		float angle = 45;
		cDescr.transform.p.x = cos(angle* ong_PI / 180.0f) * 0.4f*shapeDescr.hullFromBox.e.z;
		cDescr.transform.p.z = sin(angle* ong_PI / 180.0f)* 0.4f*shapeDescr.hullFromBox.e.z;
		cDescr.transform.p.y = 0;
		cDescr.transform.q = QuatFromAxisAngle(vec3(0, 1, 0), angle* ong_PI / 180.0f);

		bodyA->addCollider(m_world->createCollider(cDescr));

		cDescr.transform.p.x = -cos(-angle* ong_PI / 180.0f)* 0.4f*shapeDescr.hullFromBox.e.z;
		cDescr.transform.q = QuatFromAxisAngle(vec3(0, 1, 0), -angle* ong_PI / 180.0f);

		bodyA->addCollider(m_world->createCollider(cDescr));

		m_entities.push_back(new Entity(bodyA, vec3(1, 0, 0)));

		m_eye.p = vec3(0, 10, 0);
		m_eye.q = Quaternion(vec3(0, 0, 0),1);

	}

private:
};