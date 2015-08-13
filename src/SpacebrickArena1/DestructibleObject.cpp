#include "include\DestructibleObject.h"
#include "include\Space.h"

namespace sba
{
	CDestructibleObject::CDestructibleObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc, CGameObject* a_pDestructionParent, int a_DestructionTime)
		:CGameObject(a_rWorld, a_pBodyDesc),
		m_pDestructionParent(a_pDestructionParent),
		m_DestructionTime(a_DestructionTime)
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
		
		sba::CDestructibleObject* destrObjA = brick->GetGameObject()->GetDestructible();
		sba::CDestructibleObject* destrObjB = other->GetGameObject()->GetDestructible();


		//prevent mass self destruction
		int destructionCounter = sba_Space->DestructionManager->GetDestructionCounter();
		if ((destrObjA && destrObjB) && (!destrObjA->CheckForParent(destrObjB, destructionCounter) || !destrObjB->CheckForParent(destrObjA, destructionCounter)))
		{
			printf("destruction prevented! \n");
			return;
		}


		int dir = contact->colliderA == thisCollider ? -1 : 1;

		ong::vec3 impulse = ong::vec3(0,0,0);
		ong::vec3 point = ong::vec3(0,0,0); 
		for (int i = 0; i < contact->manifold.numPoints; ++i)
		{
			impulse += dir * contact->accImpulseN[i] * contact->manifold.normal;
			point += contact->manifold.points[i].position;
		}

		point = 1.0f / contact->manifold.numPoints * point;

        CBullet* bullet = (CBullet*)other->GetGameObject();
		switch (other->GetGameObject()->m_Type)
		{
		case TheBrick::EGameObjectType::Bullet:
            if (bullet->m_ID != TheBrick::Laser)
			    impulse = 10.0f * impulse;
            else
                impulse = 0.001f * impulse;
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

	bool CDestructibleObject::CheckForParent(TheBrick::CGameObject* a_pOther, int a_DestructionCounter)
	{
		if (a_pOther == m_pDestructionParent)
		{
			printf("destrTime: %d, destrCounter: %d\n", m_DestructionTime, a_DestructionCounter);
		}
		return !(a_pOther == m_pDestructionParent && m_DestructionTime > a_DestructionCounter - CDestructionManager::DESTRUCTION_COOLDOWN);
	}
}