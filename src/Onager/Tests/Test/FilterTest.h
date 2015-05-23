#pragma once

#include "test.h"
#include "Collider.h"

class FilterTest : public Test
{
public:
	void init() override
	{
		m_world = new World((vec3(0.0f, -10.0f, 0.0f)));
		
		
		Material m;
		m.density = 10.0f;
		m.friction = 1.0f;
		m.restitution = 0.0f;


		Material* material = m_world->createMaterial(m);

		ColliderData sensorData;
		{
			ShapeDescription sDescr;
			sDescr.shapeType = ShapeType::SPHERE;
			sDescr.sphere.c = vec3(0, 0, 0);
			sDescr.sphere.r = 2.0f;

			ColliderDescription cDescr;
			cDescr.transform.p = vec3(0, 0, 0);
			cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
			cDescr.material = material;
			cDescr.isSensor = true;
			cDescr.shape = m_world->createShape(sDescr);
			Collider* c = m_world->createCollider(cDescr);
			sensorData = c->getData();
			m_world->destroyCollider(c);
		}
		ColliderCallbacks callbacks;

		BodyDescription descr;
		descr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);
		descr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
		descr.type = BodyType::Dynamic;

		descr.transform.q = QuatFromAxisAngle(vec3(1.0, 0.0f, 0.0f), 0.0f);
		descr.transform.p = vec3(0.0f, 0.0f, 0.0f);

		ShapeDescription sDescr;
		sDescr.shapeType = ShapeType::SPHERE;
		sDescr.sphere.c = vec3(0, 0, 0);
		sDescr.sphere.r = 1.0f;

		ColliderDescription cDescr;
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
		cDescr.shape = m_world->createShape(sDescr);
		cDescr.material = material;
		cDescr.isSensor = false;

		Body* body = m_world->createBody(descr);
		Collider* collider = m_world->createCollider(cDescr);
		collider->setCollisionFilter(0x0);
		body->addCollider(collider);
		
		collider = m_world->createCollider(sensorData);
		callbacks.beginContact = [](Collider*, Contact*){printf("begin1\n"); };
		callbacks.endContact = [](Collider*, Contact*){printf("end1\n"); };
		collider->setCallbacks(callbacks);
		body->addCollider(collider);

		m_entities.push_back(new Entity(body, vec3(1, 0, 0)));


	/*	descr.transform.p.z = 3.0f;
		collider = m_world->createCollider(cDescr);
		collider->setCollisionFilter(0x2);
		body = m_world->createBody(descr);
		body->addCollider(collider);

		collider = m_world->createCollider(sensorData);
		callbacks.beginContact = [](Collider*, Contact*){printf("begin2\n"); };
		callbacks.endContact = [](Collider*, Contact*){printf("end2\n"); };
		collider->setCallbacks(callbacks);
		body->addCollider(collider);

		m_entities.push_back(new Entity(body, vec3(0, 1, 0)));

		descr.transform.p.z = -3.0f;
		collider = m_world->createCollider(cDescr);
		collider->setCollisionFilter(0x1 | 0x2);
		body = m_world->createBody(descr);
		body->addCollider(collider);

		collider = m_world->createCollider(sensorData);
		callbacks.beginContact = [](Collider*, Contact*){printf("begin3\n"); };
		callbacks.endContact = [](Collider*, Contact*){printf("end3\n"); };
		collider->setCallbacks(callbacks);
		body->addCollider(collider);

		m_entities.push_back(new Entity(body, vec3(1, 1, 0)));
*/
		sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		sDescr.hullFromBox.c = vec3(0, 0, 0);
		sDescr.hullFromBox.e = vec3(7, 0.5, 7);

		cDescr.shape = m_world->createShape(sDescr);
	
		descr.type = BodyType::Static;
		descr.transform.p = vec3(0, -4, -2);
		descr.transform.q = QuatFromAxisAngle(vec3(1, 0, 0), 0.1f*ong_PI);

		body = m_world->createBody(descr);
		collider = m_world->createCollider(cDescr);
		collider->setCollisionGroup(0x1);
		body->addCollider(collider);
		m_entities.push_back(new Entity(body, vec3(1, 0, 0)));

		descr.transform.p = vec3(0, -12, 2);
		descr.transform.q = QuatFromAxisAngle(vec3(1, 0, 0), -0.1f*ong_PI);
		body = m_world->createBody(descr);
		collider = m_world->createCollider(cDescr);
		collider->setCollisionGroup(0x2);
		body->addCollider(collider);
		m_entities.push_back(new Entity(body, vec3(0, 1, 0)));

	}
};