#include "include/GameScene.h"
namespace Game
{
    CGameScene::CGameScene(PuRe_Application* a_pApplication, int a_playerIdx)
    {
        this->m_pApplication = a_pApplication;
        this->m_playerIdx = a_playerIdx;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //Camera
        this->m_pCamera = new CGameCamera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective);
        this->m_pCamera->Initialize();
        this->m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/default/default");
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/Post/default");
        this->m_pSkyMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/skybox/default");
        this->m_pPointLightMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/PointLight/default");
        this->m_pModel = new PuRe_Model(a_pGraphics, "../data/models/brick1.obj");
        this->m_pSkyBox = new PuRe_SkyBox(a_pGraphics, "../data/textures/cube/");
        this->m_pPointLight = new PuRe_PointLight(a_pGraphics, this->m_pPointLightMaterial);
        this->m_pRenderer = new PuRe_Renderer(a_pGraphics);

        this->m_pPlayerShip = new TheBrick::CSpaceship();
        this->m_pPlayerShip->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
        this->m_pAsteroid = new TheBrick::CAsteroid();
        this->m_pAsteroid->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);

        this->rot = 0.0f;
        this->textureID = 0;
        this->physicsTimer = 0.0f;

    }

    // **************************************************************************
    // **************************************************************************
    bool CGameScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC)
            || a_pInput->GamepadPressed(a_pInput->Pad_Back, this->m_playerIdx)
            || a_pInput->KeyPressed(a_pInput->F1)
            || a_pInput->GamepadPressed(a_pInput->Pad_Start, this->m_playerIdx))
        {
            return true;
        }

        this->physicsTimer += a_pTimer->GetElapsedSeconds();
        float constval = 1.0f / 60.0f;
        while (this->physicsTimer > constval)
        {
            BrickBozz::Instance()->World->step(constval);
            this->physicsTimer -= constval;
        }


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

        this->rot += a_pTimer->GetElapsedSeconds()*1.0f;

        this->m_pPlayerShip->HandleInput(this->m_pCamera, a_pInput, a_pTimer->GetElapsedSeconds());

        this->m_pCamera->Update(this->m_pPlayerShip, a_pInput, a_pTimer);

        if (a_pInput->GamepadPressed(a_pInput->Pad_A, 0))
        {
            ong::Body* b = this->m_pPlayerShip->m_pBody;
            PuRe_Vector3F forward = TheBrick::OngToPuRe(ong::rotate(ong::vec3(0,0,1), b->getOrientation()));
            this->m_Bullets.push_back(new TheBrick::CBullet(BrickBozz::Instance()->BrickManager, TheBrick::OngToPuRe(this->m_pPlayerShip->m_Transform.p) + PuRe_Vector3F(-0.5f,-0.5f,0.0f) + forward*10.0f, forward*50.0f, BrickBozz::Instance()->World));
        }
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
        {
            this->m_Bullets[i]->Update(a_pTimer->GetElapsedSeconds());
            if (this->m_Bullets[i]->m_lifeTime > 10.0f)
                this->m_Bullets.erase(this->m_Bullets.begin()+i);
        }


        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_Color clear = PuRe_Color(0.0f, 0.4f, 1.0f);
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        PuRe_Vector3F pos = this->m_pCamera->GetPosition();

        a_pGraphics->Clear(clear);

        PuRe_BoundingBox Screen;
        Screen.m_Position = PuRe_Vector2F(0.0f, 0.0f);
        Screen.m_Size = PuRe_Vector2F(gdesc.ResolutionWidth, gdesc.ResolutionHeight);
        a_pGraphics->Begin(Screen);
        this->m_pSkyBox->Draw(this->m_pCamera, this->m_pSkyMaterial);
        this->m_pPlayerShip->Draw(a_pGraphics, this->m_pCamera);
        this->m_pAsteroid->Draw(a_pGraphics, this->m_pCamera);
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
        {
            this->m_Bullets[i]->Draw(a_pGraphics, this->m_pCamera);
        }
        a_pGraphics->End();


        /*   this->m_pRenderer->Draw(this->m_pModel, PuRe_Primitive::Triangles, this->m_pMaterial, PuRe_Vector3F(0.0f,-1.0f,15.0f),PuRe_Vector3F(),PuRe_Vector3F(),PuRe_Vector3F(10.0f,10.0f,10.0f));
           this->m_pRenderer->Draw(this->m_pModel, PuRe_Primitive::Triangles, this->m_pMaterial, pos + this->m_pCamera->GetForward()*10.0f);
           this->m_pRenderer->Draw(this->m_pPointLight,this->m_pPointLightMaterial,pos,PuRe_Vector3F(1.0f,0.0f,0.0f),1.0f,0.01f,0.01f);
           this->m_pRenderer->Draw(this->m_pSkyBox,this->m_pSkyMaterial);
           this->m_pRenderer->Begin(clear);
           this->m_pRenderer->Render(this->m_pCamera,this->m_pPostMaterial);
           this->m_pRenderer->End();*/

    }

    // **************************************************************************
    // **************************************************************************
    void CGameScene::Exit()
    {
        // DELETE MATERIALS
        SAFE_DELETE(this->m_pPointLightMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pSkyMaterial);
        SAFE_DELETE(this->m_pMaterial);
        // DELETE OBJECTS
        SAFE_DELETE(this->m_pAsteroid);
        SAFE_DELETE(this->m_pPlayerShip);
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pPointLight);
        // DELETE CAMERAS
        SAFE_DELETE(this->m_pCamera);
        // DELETE MODELS
        SAFE_DELETE(this->m_pModel);
        // DELETE RENDERER
        SAFE_DELETE(this->m_pRenderer);
    }
}