#ifndef _DESTRUCTIBLE_OBJECT_H_
#define _DESTRUCTIBLE_OBJECT_H_

#include "TheBrick\GameObject.h"


namespace sba
{

	struct SBrickDestruction;

	class CDestructibleObject : public TheBrick::CGameObject
	{
	private:
		SBrickDestruction* m_pCenterBrick;
		CGameObject*  m_pDestructionParent;
		int m_DestructionTime;

	public:

		static void ImpulseResponse(ong::Collider* thisCollider, ong::Contact* contact);

		CDestructibleObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc, CGameObject* a_pDestructionParent = 0, int a_DestructionTime = -1);

		void SetCenterBrick(SBrickDestruction* a_pCenterBrick);
		SBrickDestruction* getCenterBrick();

		CDestructibleObject* GetDestructible() override
		{
			return this;
		}

		bool CheckForParent(TheBrick::CGameObject* a_pOther, int a_DestructionCounter);

        void Build();

		void Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld) override;
	};

}



#endif