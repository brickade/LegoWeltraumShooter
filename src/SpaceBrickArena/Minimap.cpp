#include "include/Minimap.h"

namespace Game
{
    CMinimap::CMinimap(PuRe_IGraphics* a_pGraphics)
    {
        this->m_pMinimapSprite = new PuRe_Sprite(a_pGraphics, "../data/textures/minimap.png");
        this->m_pMinimapPlayer = new PuRe_Sprite(a_pGraphics, "../data/textures/minimap_player.png");
        int size = 36;
        PuReEngine::Core::SVertex* pData = new PuReEngine::Core::SVertex[size];
        PuRe_Box::GetVertices(pData);
        this->m_pBoxVertexBuffer = a_pGraphics->CreateVertexBuffer(size*sizeof(PuReEngine::Core::SVertex), sizeof(PuReEngine::Core::SVertex));
        PuReEngine::Core::SVertex* vdata = (PuReEngine::Core::SVertex*)this->m_pBoxVertexBuffer->Lock();
        memcpy(vdata, pData, size*sizeof(PuReEngine::Core::SVertex));
        this->m_pBoxVertexBuffer->Unlock();
        SAFE_DELETE(pData);
    }

    // **************************************************************************
    // **************************************************************************
    CMinimap::~CMinimap()
    {
        SAFE_DELETE(this->m_pBoxVertexBuffer);
        SAFE_DELETE(this->m_pMinimapPlayer);
        SAFE_DELETE(this->m_pMinimapSprite);
    }

    // **************************************************************************
    // **************************************************************************
    void CMinimap::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, PuRe_MatrixF a_CamRotation)
    {
        this->m_Position = a_Position;
        a_pMaterial->Apply();
        a_pCamera->Apply(a_pMaterial);
        a_pMaterial->SetMatrix(PuRe_MatrixF::Scale(PuRe_Vector3F(128.0f, 128.0f, 128.0f)), "Scale");
        a_pMaterial->SetMatrix(a_CamRotation, "Rotation");
        a_pMaterial->SetMatrix(PuRe_MatrixF::Translation(a_Position), "Translation");
        a_pGraphics->SetVertexBuffer(this->m_pBoxVertexBuffer);
        a_pMaterial->Apply();
        a_pGraphics->Draw(PuRe_Primitive::Linestrip, this->m_pBoxVertexBuffer->GetSize());
        //this->m_pMinimapSprite->Draw(a_pCamera, a_pMaterial, a_Position, PuRe_Vector3F(128.0f, 128.0f, 1.0f), PuRe_Vector3F());
    }

    // **************************************************************************
    // **************************************************************************
    void CMinimap::DrawPlayer(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, const PuRe_BoundingBox &a_Boundaries, PuRe_MatrixF a_CamRotation)
    {
        a_Position.X /= a_Boundaries.m_Size.X;
        a_Position.Y /= a_Boundaries.m_Size.Y;
        a_Position.Z /= a_Boundaries.m_Size.Z;

        a_Position *= 256.0f;

        a_Position = PuRe_Vector3F(a_Position.X, a_Position.Y, -a_Position.Z);
        a_Position -= PuRe_Vector3F(128.0f, 128.0f, -128.0f);
        a_Position *= a_CamRotation;

        a_Position = this->m_Position + a_Position;


        a_pMaterial->Apply();
        a_pCamera->Apply(a_pMaterial);
        a_pMaterial->SetMatrix(PuRe_MatrixF::Scale(PuRe_Vector3F(8.0f, 8.0f, 8.0f)), "Scale");
        a_pMaterial->SetMatrix(a_CamRotation, "Rotation");
        a_pMaterial->SetMatrix(PuRe_MatrixF::Translation(a_Position), "Translation");
        a_pGraphics->SetVertexBuffer(this->m_pBoxVertexBuffer);
        a_pMaterial->Apply();
        a_pGraphics->Draw(PuRe_Primitive::Linestrip, this->m_pBoxVertexBuffer->GetSize());

        //a_Position.X /= a_Boundaries.m_Size.X;
        //a_Position.Y /= a_Boundaries.m_Size.Y;
        //a_Position.Z /= a_Boundaries.m_Size.Z;
        //a_Position.X = this->m_Position.X - 64.0f;
        //a_Position.Y = this->m_Position.Y - 64.0f;
        //a_Position.Z = this->m_Position.Z - 64.0f;
        //this->m_pMinimapPlayer->Draw(a_pCamera, a_pMaterial, a_Position, PuRe_Vector3F(8.0f, 8.0f, 1.0f), PuRe_Vector3F());
    }

}