#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/Conversion.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBrickInstance::CBrickInstance(CBrick* a_pBrick, ong::World* a_pWorld)
    {
        this->m_pBrick = a_pBrick;
        for (size_t i = 0; i < this->m_pBrick->m_pColliderData.size(); i++)
        {
            this->m_pCollider.push_back(a_pWorld->createCollider(this->m_pBrick->m_pColliderData[i]));
        }
    }

    // **************************************************************************
    // **************************************************************************
    CBrickInstance::~CBrickInstance()
    {
        
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickInstance::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        this->m_pBrick->Draw(a_pGraphics, a_pCamera, TheBrick::OngToPuRe(this->m_Transform.p), PuRe_Vector3F::One(), TheBrick::OngToPuRe(this->m_Transform.q.v));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_List<SNub>* CBrickInstance::GetWorldSpaceNubs()
    {
        PuRe_List<SNub>* nubs = new PuRe_List<SNub>(*this->m_pBrick->GetNubs()); //Copy nubs
        for (unsigned int i = 0; i < nubs->size(); i++)
        {
            SNub* n = &(*nubs)[i];
            n->Position += OngToPuRe(this->m_Transform.p);
            n->Direction *= OngToPuRe(this->m_Transform.q);
            //TODO Die Position der Nubs muss noch um den Pivot des Bricks rotiert werden
        }
        return nubs;
    }
}