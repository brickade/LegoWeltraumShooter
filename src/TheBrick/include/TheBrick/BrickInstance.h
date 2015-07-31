#ifndef _BRICKINSTANCE_H_
#define _BRICKINSTANCE_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>
#include <Onager/myMath.h>
#include <Onager/World.h>
#include "Nub.h"


namespace sba
{
	struct SBrickDestruction;
}

namespace TheBrick
{
    //Forward declaration
    class CGameObject;
    class CBrick;
	
    class CBrickInstance
    {
    public:
        CBrick* m_pBrick;
        PuRe_List<ong::Collider*> m_pCollider;
        PuRe_Color m_Color;
        void* m_UserData;

    private:

        CGameObject* m_pGameObject;
		sba::SBrickDestruction* m_pDestructionInstance;


    public:
        const ong::Transform& GetTransform() const
        {
            assert(!this->m_pCollider.empty());
            return this->m_pCollider[0]->getTransform();
        }

        void SetTransform(const ong::Transform& a_rTransform)
        {
            assert(!this->m_pCollider.empty());
            for (size_t i = 0; i < this->m_pCollider.size(); i++)
            {
                this->m_pCollider[i]->setTransform(a_rTransform);
            }
        }

        CGameObject* GetGameObject() const
        {
            return this->m_pGameObject;
        }

        void SetGameObject(CGameObject* a_pGameObject)
        {
            this->m_pGameObject = a_pGameObject;
        }

    public:
        CBrickInstance(CBrick& a_pBrick, CGameObject& a_rGameObject, ong::World& a_pWorld, PuRe_Color a_pColor = PuRe_Color(0.7f, 0.7f, 0.7f, 1.0f), bool a_isSensor=false);
        ~CBrickInstance();

        void RotateAroundPivotOffset(PuRe_QuaternionF a_Quaternion);
        PuRe_Vector3F PosToBrickSpace(const PuRe_Vector3F& a_rWorldSpacePosition) const;
        PuRe_Vector3F DirToBrickSpace(const PuRe_Vector3F& a_rWorldSpaceDir) const;
        PuRe_Vector3F PosToWorldSpace(const PuRe_Vector3F& a_rBrickSpacePosition) const;
        PuRe_Vector3F DirToWorldSpace(const PuRe_Vector3F& a_rBrickSpaceDir) const;

        SNub* GetNubAtWorldPos(const PuRe_Vector3F& a_WorldPos, float a_threshold) const;

		// returns center of mass in brick space
		PuRe_Vector3F GetCenter();

		void SetDestructionInstance(sba::SBrickDestruction* a_DestructionInstance);
		sba::SBrickDestruction* GetDestructionInstance();
    };
}

#endif /* _BRICKINSTANCE_H_ */