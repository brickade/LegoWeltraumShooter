#include "include/TheBrick/BrickInstance.h"

#include "include/TheBrick/Brick.h"
#include "include/TheBrick/GameObject.h"
#include "include/TheBrick/Conversion.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBrickInstance::CBrickInstance(CBrick& a_pBrick, CGameObject& a_rGameObject, ong::World& a_pWorld, PuRe_Color a_pColor)
    {
        this->m_pBrick = &a_pBrick;
        this->m_pGameObject = &a_rGameObject;
        for (size_t i = 0; i < this->m_pBrick->GetColliderData().size(); i++)
        {
            ong::Collider* collider = a_pWorld.createCollider(this->m_pBrick->GetColliderData()[i]);
            collider->setUserData(this);
            this->m_pCollider.push_back(collider);
        }
        this->m_Color = a_pColor;
        a_pBrick.AddInstance(*this);
        a_rGameObject.AddBrickInstance(this, a_pWorld);
    }

    // **************************************************************************
    // **************************************************************************
    CBrickInstance::~CBrickInstance()
    {
        this->m_pGameObject->RemoveBrickInstance(*this);
        this->m_pBrick->DeleteInstance(*this);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickInstance::RotateAroundPivotOffset(PuRe_QuaternionF a_Quaternion)
    {
        //Get
        PuRe_Vector3F pivotOffset = this->m_pBrick->GetPivotOffset();
        ong::Transform currentTransform = this->GetTransform();
        ong::Transform newTransform;
        //Rotation
        newTransform.q = PuReToOng(a_Quaternion) * currentTransform.q;
        //Position
        PuRe_MatrixF rotTransform = PuRe_MatrixF::Translation(-pivotOffset) * a_Quaternion.GetMatrix() * PuRe_MatrixF::Translation(pivotOffset);
        newTransform.p = currentTransform.p + ong::vec3(0, 0, 0) * PuReToOng(rotTransform) - PuReToOng(pivotOffset * OngToPuRe(newTransform.q).GetMatrix());
        //Set
        this->SetTransform(newTransform);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::PosToBrickSpace(const PuRe_Vector3F& a_rWorldSpacePosition) const
    {
        return OngToPuRe(ong::invTransformVec3(PuReToOng(a_rWorldSpacePosition), ong::transformTransform(this->GetTransform(), this->GetGameObject()->GetTransform())));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::DirToBrickSpace(const PuRe_Vector3F& a_rWorldSpaceDir) const
    {
        return OngToPuRe(ong::rotate(ong::rotate(PuReToOng(a_rWorldSpaceDir), ong::conjugate(this->GetGameObject()->GetTransform().q)), ong::conjugate(this->GetTransform().q)));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::PosToWorldSpace(const PuRe_Vector3F& a_rBrickSpacePosition) const
    {
        return OngToPuRe(ong::transformVec3(PuReToOng(a_rBrickSpacePosition), ong::transformTransform(this->GetTransform(), this->GetGameObject()->GetTransform())));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::DirToWorldSpace(const PuRe_Vector3F& a_rBrickSpaceDir) const
    {
        return OngToPuRe(ong::rotate(ong::rotate(PuReToOng(a_rBrickSpaceDir), this->GetTransform().q), this->GetGameObject()->GetTransform().q));
    }

    // **************************************************************************
    // **************************************************************************
    SNub* CBrickInstance::GetNubAtWorldPos(const PuRe_Vector3F& a_WorldPos, float a_threshold) const
    {
        std::vector<SNub>& nubs = this->m_pBrick->GetNubs();
        PuRe_Vector3F brickPos = this->PosToBrickSpace(a_WorldPos);
        for (size_t i = 0; i < nubs.size(); i++)
        {
            if ((nubs[i].Position - brickPos).Length() < a_threshold)
            {
                return &nubs[i];
            }
        }
        return nullptr;
    }

	PuRe_Vector3F CBrickInstance::GetCenter()
	{
		ong::vec3 center(0,0,0);
		float mass = 0.0f;
		for (auto c : m_pCollider)
		{
			center += c->getMassData().m * c->getMassData().cm;
			mass += c->getMassData().m;
		}
		center = 1.0f / mass * center;

		return TheBrick::OngToPuRe(center);
	}


	void CBrickInstance::SetDestructionInstance(sba::SBrickDestruction* a_DestructionInstance)
	{
		m_pDestructionInstance = a_DestructionInstance;
	}

	sba::SBrickDestruction* CBrickInstance::GetDestructionInstance()
	{
		return m_pDestructionInstance;
	}
}