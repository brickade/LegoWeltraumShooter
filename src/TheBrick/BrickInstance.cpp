#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/Conversion.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBrickInstance::CBrickInstance(CBrick& a_pBrick, ong::World& a_pWorld, PuRe_Color a_pColor)
    {
        this->m_pBrick = &a_pBrick;
        for (size_t i = 0; i < this->m_pBrick->GetColliderData().size(); i++)
        {
            this->m_pCollider.push_back(a_pWorld.createCollider(this->m_pBrick->GetColliderData()[i]));
        }
        this->m_Color = a_pColor;
        a_pBrick.AddInstance(*this);
    }

    // **************************************************************************
    // **************************************************************************
    CBrickInstance::~CBrickInstance()
    {
        this->m_pBrick->DeleteInstance(*this);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_Vector3F CBrickInstance::TransformToBrickSpace(PuRe_Vector3F a_WorldSpacePos)
    {
        return OngToPuRe(ong::rotate(PuReToOng(a_WorldSpacePos) - this->GetTransform().p, ong::conjugate(this->GetTransform().q)));
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickInstance::RotateAroundPivotOffset(PuRe_QuaternionF a_Quaternion)
    {
        PuRe_MatrixF rot = PuRe_MatrixF::Translation(-this->m_pBrick->GetPivotOffset()) * a_Quaternion.GetMatrix() * PuRe_MatrixF::Translation(this->m_pBrick->GetPivotOffset());
        this->GetTransform().p = this->GetTransform().p * PuReToOng(rot);
        this->GetTransform().q = PuReToOng(a_Quaternion) * this->GetTransform().q;;
    }
}