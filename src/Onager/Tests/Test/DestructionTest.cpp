#include "DestructionTest.h"



class Ship : public Entity
{
	struct Brick
	{
		Brick* stud[4][2][2];
		Collider* collider;
	};

	Brick* m_base;
public:

	Ship(World* world, Body* body, vec3 color)
		: Entity(body, color)
	{
		ShapeDescription sDescr;
		sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		sDescr.hullFromBox.c = vec3(0, 0, 0);
		sDescr.hullFromBox.e = vec3(4, 1, 2);

		ColliderDescription cDescr;
		cDescr.isSensor = false;
		cDescr.material = world->createMaterial({ 1, 1, 1 });
		cDescr.shape = world->createShape(sDescr);
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

		Collider* collider = world->createCollider(cDescr);

		m_body->addCollider(collider);
		
		m_base = new Brick;
		m_base->collider = collider;
		m_base->collider->setUserData(m_base);
		
		cDescr.transform.p = vec3(2, 1, 0);
		collider = world->createCollider(cDescr);

		Brick* brick = new Brick;
		brick->collider = collider;
		brick->collider->setUserData(brick);


		m_base->stud[2][0][1] = brick;
		m_base->stud[2][1][1] = brick;
		m_base->stud[3][0][1] = brick;
		m_base->stud[3][1][1] = brick;

		brick->stud[0][0][0] = m_base;
		brick->stud[0][1][0] = m_base;
		brick->stud[1][0][0] = m_base;
		brick->stud[1][1][0] = m_base;
	}
};



void DestructionTest::init()
{
	m_world = new World();

	BodyDescription bDescr;
	bDescr.type = BodyType::Dynamic;
	bDescr.linearMomentum = vec3(0, 0, 0);
	bDescr.angularMomentum = vec3(0, 0, 0);
	bDescr.transform.p = vec3(0, 0, -5);
	bDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

	
}


void DestructionTest::update(float dt)
{

}