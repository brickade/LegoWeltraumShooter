#include "HierarchieTest.h"
#include <stdlib.h>
#include <time.h>
#include "Draw.h"




void HierarchieTest::init()
{
	m_world = new World();

	BodyDescription bodyDescr;
	bodyDescr.type = BodyType::Static;
	bodyDescr.transform.p = vec3(0.0f, 0.0f, 0.0f);
	bodyDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	bodyDescr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
	bodyDescr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);

	m_body = m_world->createBody(bodyDescr);

	m_entities.push_back(new Entity(m_body, vec3(1, 0, 0)));

	m_eye.p = vec3(0, 0, 0);

	srand((uint32)time(NULL));

	m_Material.density = 1.0f;
	m_Material.friction = 0.0f;
	m_Material.restitution = 0.0f;


	float width = 1;
	float height = 1;
	float depth = 1;

	vec3 box[8] =
	{
		vec3(-width, -height, depth),
		vec3(-width, -height, -depth),
		vec3(width, -height, -depth),
		vec3(width, -height, depth),

		vec3(-width, height, depth),
		vec3(-width, height, -depth),
		vec3(width, height, -depth),
		vec3(width, height, depth)
	};

	Transform t;
	t.p.x = 0;
	t.p.y =  0;
	t.p.z = 0;

	t.q = QuatFromAxisAngle(vec3(1, 0, 0), 0);

	//ColliderDescription colliderDescr;
	//colliderDescr.material = &m_Material;
	//colliderDescr.type = ShapeType::SPHERE;
	//colliderDescr.transform = t;
	//colliderDescr.sphere.c = vec3(0,0,0);
	//colliderDescr.sphere.r = 1.0f;

	
	ShapeDescription shapeDescr;
	shapeDescr.shapeType = ShapeType::CAPSULE;
	shapeDescr.capsule.c1 = vec3(0, -1, 0);
	shapeDescr.capsule.c2 = vec3(0, 1, 0);
	shapeDescr.capsule.r = 1.0f;


	ShapePtr capsule = m_world->createShape(shapeDescr);

	ColliderDescription colliderDescr;
	colliderDescr.material = &m_Material;
	colliderDescr.transform = t;
	colliderDescr.shape = capsule;
	colliderDescr.isSensor = false;

	m_collider = m_world->createCollider(colliderDescr);

}


bool HierarchieTest::procEvent(SDL_Event event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
	{
		char* keys = 0;
		Transform* t = &m_collider->getTransform();




		if (event.key.keysym.mod & KMOD_CTRL)
		{

			float angle = 0.5f * ong_PI * 0.1f;

			switch (event.key.keysym.scancode)
			{
			case SDL_SCANCODE_KP_8:
				t->q = QuatFromAxisAngle(vec3(0, 0, 1), -angle) * t->q;
				return true;
			case SDL_SCANCODE_KP_2:
				t->q = QuatFromAxisAngle(vec3(0, 0, 1), angle) * t->q;
				return true;
			case SDL_SCANCODE_KP_4:
				t->q = QuatFromAxisAngle(vec3(1, 0, 0), -angle) * t->q;
				return true;
			case SDL_SCANCODE_KP_6:
				t->q = QuatFromAxisAngle(vec3(1, 0, 0), angle) * t->q;
				return true;
			case SDL_SCANCODE_KP_7:
				t->q = QuatFromAxisAngle(vec3(0, 1, 0), -angle) * t->q;
				return true;
			case SDL_SCANCODE_KP_9:
				t->q = QuatFromAxisAngle(vec3(0, 1, 0), angle) * t->q;
				return true;
			}
		}
		else
		{
			switch (event.key.keysym.scancode)
			{
			case SDL_SCANCODE_KP_8:
				t->p.z += 0.1f;
				return true;
			case SDL_SCANCODE_KP_2:
				t->p.z -= 0.1f;
				return true;
			case SDL_SCANCODE_KP_4:
				t->p.x -= 0.1f;
				return true;
			case SDL_SCANCODE_KP_6:
				t->p.x += 0.1f;
				return true;
			case SDL_SCANCODE_KP_7:
				t->p.y -= 0.1f;
				return true;
			case SDL_SCANCODE_KP_9:
				t->p.y += 0.1f;
				return true;
			case SDL_SCANCODE_SPACE:
			{
				bool query = m_body->queryCollider(m_collider);

				printf("%s\n", query ? "true" : "false");
				return true;
			}

			}
		}

		break;
		
	}
	}

	return false;
}

void HierarchieTest::update(float dt)
{

	if (SDL_GetKeyboardState(0)[SDL_SCANCODE_KP_PLUS])
	{
		float width = fmodf((float)rand(), 10.0f) + 1;
		float height = fmodf((float)rand(), 10.0f) + 1;
		float depth = fmodf((float)rand(), 10.0f) + 1;


		vec3 box[8] =
		{
			vec3(-width, -height, depth),
			vec3(-width, -height, -depth),
			vec3(width, -height, -depth),
			vec3(width, -height, depth),

			vec3(-width, height, depth),
			vec3(-width, height, -depth),
			vec3(width, height, -depth),
			vec3(width, height, depth)
		};



		Transform t;
		t.p.x = fmodf((float)rand(), 20.0f) - 10.0f;
		t.p.y = fmodf((float)rand(), 20.0f) - 10.0f;
		t.p.z = fmodf((float)rand(), 20.0f) - 10.0f;
		
		//t.q = QuatFromAxisAngle(vec3(rand(), rand(), rand()), rand());
		t.q = QuatFromAxisAngle(vec3(1, 0, 0), 0);

		ColliderDescription colliderDescr;
		colliderDescr.material = &m_Material;
		colliderDescr.transform = t;
		colliderDescr.isSensor = false;
		
		int shapeChoice = rand() % 3;
		ShapeDescription shapeDescr;
		if (shapeChoice == 0)
		{
			shapeDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
			shapeDescr.hullFromBox.c = vec3(0, 0, 0);
			shapeDescr.hullFromBox.e = vec3(width, height, depth);
		}
		else if (shapeChoice == 1)
		{
			shapeDescr.type = ShapeType::SPHERE;
			shapeDescr.sphere.c = vec3(0,0,0);
			shapeDescr.sphere.r = width;
		}
		else if (shapeChoice == 2)
		{
			shapeDescr.type = ShapeType::CAPSULE;
			shapeDescr.capsule.c1 = vec3(width-1, height-1, depth-1);
			shapeDescr.capsule.c2 = vec3(-width+1, -height+1, -depth+1);
			shapeDescr.capsule.r = width;
		}

		ShapePtr shape = m_world->createShape(shapeDescr);
		colliderDescr.shape = shape;
		Collider* c = m_world->createCollider(colliderDescr);

		m_body->addCollider(c);

		printf("%d\n", m_body->getNumCollider());
	}

	int x, y;
	if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		int w, h;
		SDL_GetWindowSize(m_window, &w, &h);

		float _x = x/(float)w - 0.5f;
		float _y = y /(float)h - 0.5f;

		vec3 dir;
		
		float aspect = 800.0f / 600.0f;

		float fovY = 60 * ong_PI / 180.0f;
		float fovX = fovY * aspect;


		dir.z = 0.1f;
		dir.y = -tan(_y*fovY) * 0.1f;
		dir.x = tan(_x*fovX) * 0.1f;

		dir = normalize(rotate(dir, m_eye.q));

		RayQueryResult result = { 0 };
		m_body->queryRay(m_eye.p, dir, &result);
		Collider* c = result.collider;
		if (c)
			m_world->destroyCollider(c);
	}

}

void HierarchieTest::render()
{
	glUniform3f(m_colorLocation, 0, 1, 0);

	glPushMatrix();

	vec3 p = m_collider->getTransform().p;
	glTranslatef(p.x, p.y, p.z);

	mat3x3 q = transpose(toRotMat(m_collider->getTransform().q));

	float rot[16] =
	{
		q[0][0], q[0][1], q[0][2], 0.0f,
		q[1][0], q[1][1], q[1][2], 0.0f,
		q[2][0], q[2][1], q[2][2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	glMultMatrixf(rot);

	drawCollider(m_collider);

#if 1
	glUniform3f(m_colorLocation, 1, 1, 1);
	drawBox(m_collider->getAABB().c, m_collider->getAABB().e);
#endif

	glPopMatrix();
}