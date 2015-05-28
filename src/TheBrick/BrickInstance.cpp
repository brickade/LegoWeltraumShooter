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
        PuRe_Vector3F pivotOffset = this->m_pBrick->GetPivotOffset();
        ong::Transform currentTransform = this->GetTransform();
        PuRe_MatrixF transform = PuRe_MatrixF::Translation(-pivotOffset) * OngToPuRe(currentTransform.q).GetMatrix() * PuRe_MatrixF::Translation(pivotOffset) * PuRe_MatrixF::Translation(OngToPuRe(currentTransform.p));
        return a_rWorldSpacePosition * PuRe_MatrixF::Invert(transform);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::DirToBrickSpace(const PuRe_Vector3F& a_rWorldSpaceRotation) const
    {
        return a_rWorldSpaceRotation * OngToPuRe(ong::conjugate(this->GetTransform().q));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::PosToWorldSpace(const PuRe_Vector3F& a_rBrickSpacePosition) const
    {
        PuRe_Vector3F pivotOffset = this->m_pBrick->GetPivotOffset();
        ong::Transform currentTransform = this->GetTransform();
        PuRe_MatrixF transform = PuRe_MatrixF::Translation(-pivotOffset) * OngToPuRe(currentTransform.q).GetMatrix() * PuRe_MatrixF::Translation(pivotOffset) * PuRe_MatrixF::Translation(OngToPuRe(currentTransform.p));
        return a_rBrickSpacePosition * transform;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::DirToWorldSpace(const PuRe_Vector3F& a_rBrickSpaceRotation) const
    {
        return a_rBrickSpaceRotation * OngToPuRe(this->GetTransform().q);
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
    }
}