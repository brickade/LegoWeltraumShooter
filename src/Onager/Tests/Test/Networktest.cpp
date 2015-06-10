#include "NetworkTest.h"



void NetworkTest::start()
{
	m_world = new World(vec3(0, -10, 0));

	Material* material = m_world->createMaterial({ 1.0f, 0.0f, 1.0f });

	m_entities.push_back(addFloor(m_world, material, vec3(0, 0, 0)));

	
	BodyDescription descr;
	descr.angularMomentum = vec3(0, 0, 0);
	descr.linearMomentum = vec3(0, 0, 0);
	descr.transform.p = vec3(-m_numPlayers, 2, 0);
	descr.transform.q = Quaternion(vec3(0, 0, 0), 1);
	descr.type = BodyType::Dynamic;

	m_players = new Entity*[m_numPlayers];

	for (int i = 0; i < m_numPlayers; ++i)
	{
		Entity* entity = addBox(m_world, descr, material);
		m_entities.push_back(entity);
		m_players[i] = entity;
		descr.transform.p.x += 2.0f;
	}

	m_eye.p = vec3(0, 10, 0);
	m_eye.q = QuatFromAxisAngle(vec3(1, 0, 0), 0.5f*ong_PI);

}

void NetworkTest::addMovement(int movement, int idx)
{
	int x = ((movement & 0x01) ? 1 : 0) - ((movement & 0x02) ? 1 : 0);
	int y = ((movement & 0x04) ? 1 : 0) - ((movement & 0x08) ? 1 : 0);
	int z = ((movement & 0x10) ? 1 : 0) - ((movement & 0x20) ? 1 : 0);

	vec3 p = m_players[idx]->getBody()->getWorldCenter();
	p.y += 1;
	m_players[idx]->getBody()->applyImpulse(vec3(1.0f * x, 11.0f * y, 1.0f*z), p);
}
