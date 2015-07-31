#pragma once

#include "test.h"


class ForceTest : public Test
{
public:
	virtual void init()
	{
		m_world = new World(vec3(0,-10,0));

		ShapeDescription shapeDescr;

		shapeDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		shapeDescr.hullFromBox.c = ong::vec3(0,0,0);
		shapeDescr.hullFromBox.e = ong::vec3(0.2, 0.2, 1.0);


		ColliderDescription cDescr;
		cDescr.material = m_world->createMaterial(Material{ 1.0, 0.0, 1.0 });
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
		cDescr.isSensor = false;

		cDescr.shape = m_world->createShape(shapeDescr);

		Collider* testA = m_world->createCollider(cDescr);

		BodyDescription bDescr;
		bDescr.angularMomentum = vec3(0.0, 0.0, 0);
		bDescr.linearMomentum = vec3(0, 0, 0);
		bDescr.transform.p = vec3(0, 0, 0);
		bDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

		m_pBody = m_world->createBody(bDescr);
		m_pBody->addCollider(testA);
		m_entities.push_back(new Entity(m_pBody, vec3(1, 0, 0)));

		m_entities.push_back(addFloor(m_world, cDescr.material, vec3(0, -2, 0)));
	}

	virtual void update(float dt) override
	{
		int x, y;
		if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_RIGHT))
		{
			int w, h;
			SDL_GetWindowSize(m_window, &w, &h);

			float _x = x / (float)w - 0.5f;
			float _y = y / (float)h - 0.5f;

			vec3 dir;

			float aspect = 800.0f / 600.0f;

			float fovY = 60 * ong_PI / 180.0f;
			float fovX = fovY * aspect;


			dir.z = 0.1f;
			dir.y = -tan(_y*fovY) * 0.1f;
			dir.x = tan(_x*fovX) * 0.1f;

			dir = normalize(rotate(dir, m_eye.q));

			RayQueryResult result = { 0 };
			if (m_world->queryRay(m_eye.p, dir, &result))
			{
				vec3 force = 10.0f * normalize(result.point - transformVec3(m_pBody->getLocalCenter() + vec3(0,0,0), m_pBody->getTransform()));
				printf("force: %f|%f|%f\n", force.x, force.y, force.z);

				m_pBody->applyRelativeForce(rotate(force, conjugate(m_pBody->getOrientation())), vec3(0, 0, 1), dt);
			}
		}
	}

private:

	Body* m_pBody;
};

