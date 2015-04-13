#include "include/MainScene.h"
namespace Game
{
    CMainScene::CMainScene(PuRe_Application* a_pApplication)
    {
        this->m_pApplication = a_pApplication;
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
    {

        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //Camera
        this->m_pCamera = new PuRe_Camera(PuRe_Vector2F((float32)gdesc.ResolutionWidth, (float32)gdesc.ResolutionHeight), PuRe_Camera_Perspective);
        this->m_pPostCamera = new PuRe_Camera(PuRe_Vector2F((float32)gdesc.ResolutionWidth, (float32)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
        this->m_pCamera->SetFoV(45.0f);
        this->m_pCamera->Move(PuRe_Vector3F(5.0f, 5.0f, -10.0f));
        this->m_pCamera->Rotate(-20.0f, 20.0f, 0.0f);
        this->m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/default/default");
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
        this->m_pModel = new PuRe_Model(a_pGraphics, this->m_pMaterial, "../data/models/squirtle/Squirtle.obj");
        this->m_pRenderTarget = a_pGraphics->CreateRendertarget(this->m_pPostMaterial);
        this->m_pSkyDome = new PuRe_Skydome(a_pGraphics, this->m_pMaterial, "../data/textures/space.png");
        this->rot = 0.0f;
        this->textureID = 0;

    }

    // **************************************************************************
    // **************************************************************************
    bool CMainScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC))
            return true;

        if (a_pInput->KeyPressed(a_pInput->Left))
        {
            this->textureID--;
            if (this->textureID < 0)
                this->textureID = 2;
        }

        else if (a_pInput->KeyPressed(a_pInput->Right))
        {
            this->textureID++;
            if (this->textureID > 2)
                this->textureID = 0;
        }
        //Seconds for frame independent movement
        float32 Seconds = a_pTimer->GetElapsedSeconds();
        this->rot += Seconds*1.0f;

        PuRe_Vector3F CameraMove;
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        if (a_pInput->MouseIsPressed(a_pInput->LeftClick))
        {

            PuRe_Vector2F speed = (a_pInput->GetMousePosition() - this->MouseClickPosition);
            speed.Normalize();
            speed *= 100.0f;
            PuRe_Vector3F cameraLook = PuRe_Vector3F();
            if (speed.X != 0.0f)
                cameraLook.X += speed.X*Seconds;
            if (speed.Y != 0.0f)
                cameraLook.Y += speed.Y*Seconds;
            this->m_pCamera->Rotate(cameraLook.X, cameraLook.Y, cameraLook.Z);
        }
        float32 speed = 10.0f*Seconds;
        this->MouseClickPosition = a_pInput->GetMousePosition();
        //Handle Movement
        if (a_pInput->KeyIsPressed(a_pInput->W))
            CameraMove.Z += speed;
        else if (a_pInput->KeyIsPressed(a_pInput->S))
            CameraMove.Z -= speed;
        if (a_pInput->KeyIsPressed(a_pInput->D))
            CameraMove.X += speed;
        else if (a_pInput->KeyIsPressed(a_pInput->A))
            CameraMove.X -= speed;

        this->m_pCamera->Move(CameraMove);
        for (int32 i = 0; i < 4; i++)
        {
            if (a_pInput->GamepadPressed(a_pInput->Pad_A, i))
                printf("A pressed by %i\n", i);
        }


        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_Color clear = PuRe_Color(0.0f, 0.4f, 1.0f);
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        this->m_pRenderTarget->Apply(clear);

        for (int32 i = 0; i<4; i++)
        {
            float32 x = (2.5f*(float32)(i / 2)) + 2.0f;
            float32 y = 2.5f*(float32)(i % 2);
            this->m_pModel->Draw(this->m_pCamera, PuRe_Primitive::Triangles, PuRe_Vector3F(x, y, 0.0f), PuRe_Vector3F(1.0f, 1.0f, 1.0f), PuRe_Vector3F(0.0f, this->rot, 0.0f), PuRe_Vector3F(0.0f, 0.0f, 0.0f));
        }
        this->m_pSkyDome->Draw(this->m_pCamera, PuRe_Vector3F(0.0f, this->rot / 1000.0f, 0.0f));

        a_pGraphics->Begin(clear);
        this->m_pPostMaterial->Apply();
        this->m_pPostMaterial->SetFloat((float32)textureID, "textureID");
        this->m_pPostMaterial->SetVector3(PuRe_Vector3F(0.1f, 0.1f, 0.1f), "ambient");
        this->m_pRenderTarget->Draw(this->m_pPostCamera, PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f), PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f));

        a_pGraphics->End();
    }

    // **************************************************************************
    // **************************************************************************
    void CMainScene::Exit()
    {
        SAFE_DELETE(this->m_pSkyDome);
        SAFE_DELETE(this->m_pRenderTarget);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pMaterial);
        SAFE_DELETE(this->m_pPostCamera);
        SAFE_DELETE(this->m_pCamera);
        SAFE_DELETE(this->m_pModel);
    }
}