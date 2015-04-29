#include "include/TheBrick/BrickInstance.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBrickInstance::CBrickInstance(CBrick* a_pBrick)
    {
        this->m_pBrick = a_pBrick;
        this->m_position = PuRe_Vector3F(0, 0, 0);
        this->m_rotation = 0;
        this->m_scale = 1;
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
        this->m_pBrick->Draw(a_pGraphics, a_pCamera, this->m_position, PuRe_Vector3F(this->m_scale, this->m_scale, this->m_scale), PuRe_Vector3F(0, this->m_rotation, 0));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_List<SNub>* CBrickInstance::GetWorldSpaceNubs()
    {
        PuRe_List<SNub>* nubs = new PuRe_List<SNub>(*this->m_pBrick->GetNubs()); //Copy nubs
        for (int i = 0; i < nubs->size(); i++)
        {
            (*nubs)[i].Position += this->m_position;
            //(*nubs)[i].Orientation += this->m_rotation; Need consens of orientation vs rotation vs euler vs stuff: need transform component w position, rotation & scale
        }
        return nubs;
    }
}