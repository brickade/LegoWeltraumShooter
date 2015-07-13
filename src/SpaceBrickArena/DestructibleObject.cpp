#include "include\DestructibleObject.h"
#include "include\Space.h"

namespace sba
{
	CDestructibleObject::CDestructibleObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc)
		:CGameObject(a_rWorld, a_pBodyDesc)
	{
	}


	void CDestructibleObject::SetCenterBrick(SBrickDestruction* a_pCenterBrick)
	{
		m_pCenterBrick = a_pCenterBrick;
	}
	
	SBrickDestruction* CDestructibleObject::getCenterBrick()
	{
		return m_pCenterBrick;
	}


	void CDestructibleObject::ImpulseResponse(ong::Collider* thisCollider, ong::Contact* contact)
	{
		if (contact->manifold.numPoints == 0)
			return;

		TheBrick::CBrickInstance* brick = (TheBrick::CBrickInstance*)thisCollider->getUserData();
		TheBrick::CBrickInstance* other = (TheBrick::CBrickInstance*)(thisCollider == contact->colliderA ? contact->colliderB : contact->colliderA)->getUserData();

		int dir = contact->colliderA == thisCollider ? -1 : 1;

		ong::vec3 impulse = ong::vec3(0,0,0);
		ong::vec3 point = ong::vec3(0,0,0); 
		for (int i = 0; i < contact->manifold.numPoints; ++i)
		{
			impulse += dir * contact->accImpulseN[i] * contact->manifold.normal;
			point += contact->manifold.points[i].position;
		}

		point = 1.0f / contact->manifold.numPoints * point;

		switch (other->GetGameObject()->m_Type)
		{
		case TheBrick::EGameObjectType::Bullet:
			impulse = 10.0f * impulse;
			break;
		case TheBrick::EGameObjectType::Ship:
			impulse = 10.0f * impulse;
			break;
		}

		sba_Space->DestructionManager->AddImpulse(brick, point, impulse);
    }

    void CDestructibleObject::Build()
    {
        sba_Space->DestructionManager->BuildDestruction(this, m_pBricks.data(), m_pBricks.size());

        for (auto pBrick : m_pBricks)
        {
            for (auto pCollider : pBrick->m_pCollider)
            {
                ong::ColliderCallbacks cb = pCollider->getColliderCallbacks();
                cb.postSolve = ImpulseResponse;
                pCollider->setCallbacks(cb);

            }
        }
    }

	void CDestructibleObject::Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld) 
	{
		CGameObject::Deserialize(a_pSerializer, a_rBricks, a_pWorld);
	}



}