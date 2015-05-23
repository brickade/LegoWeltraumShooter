#pragma once

#include "test.h"

class RestitutionTest : public Test
{
public:
	void init() override
	{
		m_world = new World((vec3(0.0f, -10.0f, 0.0f)));
		Material m;
		m.density = 10.0f;
		m.friction = 1.0f;
		m.restitution = 0.0f;


		Material* m0 = m_world->createMaterial(m);
		m.restitution = 0.25f;
		Material* m1 = m_world->createMaterial(m);
		m.restitution = 0.5f;
		Material* m2 = m_world->createMaterial(m);
		m.restitution = 0.75f;
		Material* m3 = m_world->createMaterial(m);
		m.restitution = 1.00f;
		Material* m4 = m_world->createMaterial(m);

		m_entities.push_back(addFloor(m_world, m0, vec3(0,0,0)));

		BodyDescription descr;
		descr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);
		descr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
		descr.type = BodyType::Dynamic;

		descr.transform.q = QuatFromAxisAngle(vec3(1.0, 0.0f, 0.0f), 0.0f);
		descr.transform.p = vec3(0.0f, 5.0f, -5.0f);

		ShapeDescription sDescr;
		sDescr.shapeType = ShapeType::SPHERE;
		sDescr.sphere.c = vec3(0, 0, 0);
		sDescr.sphere.r = 0.2f;

		ColliderDescription cDescr;
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0),1);
		cDescr.shape = m_world->createShape(sDescr);
		cDescr.material = m0;
		cDescr.isSensor = false;
	
		Body* body = m_world->createBody(descr);
		body->addCollider(m_world->createCollider(cDescr));

		m_entities.push_back(new Entity(body, vec3(1,0,0)));
		descr.transform.p.z += 3.0f;
		cDescr.material = m1;
		body = m_world->createBody(descr);
		body->addCollider(m_world->createCollider(cDescr));

		m_entities.push_back(new Entity(body, vec3(1, 0, 0)));

		descr.transform.p.z += 3.0f;
		cDescr.material = m2;
		body = m_world->createBody(descr);
		body->addCollider(m_world->createCollider(cDescr));

		m_entities.push_back(new Entity(body, vec3(1, 0, 0)));

		descr.transform.p.z += 3.0f;
		cDescr.material = m3;
		body = m_world->createBody(descr);
		body->addCollider(m_world->createCollider(cDescr));

		m_entities.push_back(new Entity(body, vec3(1, 0, 0)));

		descr.transform.p.z += 3.0f;
		cDescr.material = m4;
		body = m_world->createBody(descr);
		body->addCollider(m_world->createCollider(cDescr));

		m_entities.push_back(new Entity(body, vec3(1, 0, 0)));

	}
};