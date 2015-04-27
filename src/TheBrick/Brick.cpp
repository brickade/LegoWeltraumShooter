#include "include/TheBrick/Brick.h"
namespace TheBrick
{
    const float CBrick::SEGMENT_WIDTH = 0.39f;
    const float CBrick::SEGMENT_HEIGHT = 0.1f;

    // **************************************************************************
    // **************************************************************************
    CBrick::CBrick(PuRe_Model* a_pModel, PuRe_Vector3I a_segments, bool a_isPlate)
    {
        m_pModel = a_pModel;
        m_segments = a_segments;
        m_isPlate = a_isPlate;
    }

    // **************************************************************************
    // **************************************************************************
    CBrick::~CBrick()
    {
        SAFE_DELETE(this->m_pModel);
    }

    // **************************************************************************
    // **************************************************************************
    CBrick* CBrick::FromFile(PuRe_IGraphics* a_pGraphics, const char* a_pPath)
    {
        PuRe_IMaterial* material = a_pGraphics->LoadMaterial("../data/effects/editor/default");
        PuRe_Model* model = new PuRe_Model(a_pGraphics, material, a_pPath);
        PuRe_Vector3I segments = PuRe_Vector3I(1, 1, 1);
        bool plate = false;
        return new CBrick(model, segments, plate);
    }

    // **************************************************************************
    // **************************************************************************
    CBrickInstance* CBrick::CreateInstance()
    {
        return new CBrickInstance(this);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrick::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, PuRe_Vector3F a_position, PuRe_Vector3F a_Scale, PuRe_Vector3F a_rotation)
    {
        this->m_pModel->Draw(a_pCamera, PuRe_Primitive::Triangles, a_position, a_Scale, a_rotation, PuRe_Vector3F(0, 0, 0));
    }
}