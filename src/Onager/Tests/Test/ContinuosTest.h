
#pragma once

#include "test.h"


class ContinuousTest : public Test
{
public:
	virtual void init()
	{
		m_world = new World();

		ShapeDescription shapeDescr;

		shapeDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		shapeDescr.hullFromBox.c = ong::vec3(0, 0, 0);
		shapeDescr.hullFromBox.e = ong::vec3(1, 5, 20);

		ShapePtr box = m_world->createShape(shapeDescr);

		shapeDescr.shapeType = ShapeType::SPHERE;
		shapeDescr.sphere.c = ong::vec3(0, 0, 0);
		shapeDescr.sphere.r = 0.5f;

		ShapePtr sphere = m_world->createShape(shapeDescr);


		ColliderDescription cDescr;
		cDescr.material = m_world->createMaterial({ 1.0, 1.0, 0.0 });
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
		cDescr.isSensor = false;

		cDescr.shape = box;

		BodyDescription bDescr;
		bDescr.type = BodyType::Static;
		bDescr.angularMomentum = vec3(0.0, 0.0, 0);
		bDescr.linearMomentum = vec3(0, 0, 0);
		bDescr.transform.p = vec3(-5, 0, 0);
		bDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
		bDescr.continuousPhysics = false;

		Body* body = m_world->createBody(bDescr);
		body->addCollider(m_world->createCollider(cDescr));
		m_entities.push_back(new Entity(body, vec3(0, 1, 0)));

		bDescr.transform.p.x = 5;
		body = m_world->createBody(bDescr);
		body->addCollider(m_world->createCollider(cDescr));
		m_entities.push_back(new Entity(body, vec3(0, 1, 0)));

		bDescr.type = BodyType::Dynamic;
		bDescr.linearMomentum = vec3(000.0f, 0.0f, 500.0f);
		bDescr.transform.p = vec3(-0.25f, 0.0f, -8.0f);
		bDescr.continuousPhysics = true;

		body = m_world->createBody(bDescr);
		cDescr.shape = sphere;
		body->addCollider(m_world->createCollider(cDescr));
		m_entities.push_back(new Entity(body, vec3(1, 0, 0)));

		bDescr.linearMomentum = vec3(000.0f, 0.0f, -500.0f);
		bDescr.transform.p = vec3(0.24f, 0.0f, 8.0f);
		body = m_world->createBody(bDescr);
		cDescr.shape = sphere;
		body->addCollider(m_world->createCollider(cDescr));
		m_entities.push_back(new Entity(body, vec3(0, 0, 1)));

		m_eye.p = vec3(0, 40, 0);
		m_eye.q = QuatFromAxisAngle(vec3(1, 0, 0), 0.5f*ong_PI);
	}

private:
};