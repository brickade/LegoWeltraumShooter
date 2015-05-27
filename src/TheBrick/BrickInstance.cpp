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
            this->m_pCollider.push_back(a_pWorld.createCollider(this->m_pBrick->GetColliderData()[i]));
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
}