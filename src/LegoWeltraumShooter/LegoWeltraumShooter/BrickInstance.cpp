#include "include/BrickInstance.h"
namespace Game
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
}