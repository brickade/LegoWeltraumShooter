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
        PuRe_Vector2F size = PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight);
        this->m_pUICamera = new PuRe_Camera(size, PuRe_Camera_Orthogonal);
        this->m_pUICamera->setNearFar(PuRe_Vector2F(0.01f,1000.0f));
        this->m_pCamera = new CGameCamera(size, PuRe_Camera_Perspective);
        this->m_pCamera->Initialize();
        this->m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/default/default");
        this->m_pFontMaterial = a_pGraphics->LoadMaterial("../data/effects/font/default");
        this->m_pUIMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/UI/default");
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/Post/default");
        this->m_pSkyMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/skybox/default");
        this->m_pPointLightMaterial = a_pGraphics->LoadMaterial("../data/effects/GameEffects/PointLight/default");
        this->m_pModel = new PuRe_Model(a_pGraphics, "../data/models/brick1.obj");
        this->m_pSkyBox = new PuRe_SkyBox(a_pGraphics, "../data/textures/cube/");
        this->m_pPointLight = new PuRe_PointLight(a_pGraphics);
        this->m_pRenderer = new PuRe_Renderer(a_pGraphics,PuRe_Vector2I(size.X,size.Y));
        this->m_pMinimap = new CMinimap(a_pGraphics);
        this->m_pFont = new PuRe_Font(a_pGraphics, "../data/textures/font.png");
        this->m_pNetwork = new CNetworkHandler();

        this->m_MapBoundaries = PuRe_BoundingBox(PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(100.0f, 100.0f, 100.0f));

        this->gameStart = false;
        this->m_NetworkState = 0;

        //this->m_pPlayerShip = new TheBrick::CSpaceship();
        //this->m_pPlayerShip->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
        //ong::vec3 start(50.0f, 50.0f, 50.0f);
        //for (int i = 0; i < 10; i++)
        //{
        //    TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(BrickBozz::Instance()->World, start + ong::vec3((i % 2)*5.0f,(i % 2) * 5.0f,i*2.0f));
        //    asteroid->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
        //    this->m_Asteroids.push_back(asteroid);
        //}

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
                this->textureID = 3;
        }

        else if (a_pInput->KeyPressed(a_pInput->Right))
        {
            this->textureID++;
            if (this->textureID > 3)
                this->textureID = 0;
        }
        if (this->gameStart)
        {
            this->m_pPlayerShip->HandleInput(a_pInput, a_pTimer->GetElapsedSeconds(), this->m_Bullets, BrickBozz::Instance()->BrickManager);
            PuRe_Vector3F playerpos = TheBrick::OngToPuRe(this->m_pPlayerShip->m_pBody->getTransform().p);

            //Move player inside of map if hes outside
            float mapEnd = this->m_MapBoundaries.m_Position.X + this->m_MapBoundaries.m_Size.X;
            if (playerpos.X > mapEnd)
                playerpos.X = playerpos.X - mapEnd;
            else if (playerpos.X < this->m_MapBoundaries.m_Position.X)
                playerpos.X = mapEnd + playerpos.X;

            mapEnd = this->m_MapBoundaries.m_Position.Y + this->m_MapBoundaries.m_Size.Y;
            if (playerpos.Y > mapEnd)
                playerpos.Y = playerpos.Y - mapEnd;
            else if (playerpos.Y < this->m_MapBoundaries.m_Position.Y)
                playerpos.Y = mapEnd + playerpos.Y;

            mapEnd = this->m_MapBoundaries.m_Position.Z + this->m_MapBoundaries.m_Size.Z;
            if (playerpos.Z > mapEnd)
                playerpos.Z = playerpos.Z - mapEnd;
            else if (playerpos.Z < this->m_MapBoundaries.m_Position.Z)
                playerpos.Z = mapEnd + playerpos.Z;

            this->m_pPlayerShip->m_pBody->setPosition(TheBrick::PuReToOng(playerpos));

            this->m_pCamera->Update(this->m_pPlayerShip, a_pInput, a_pTimer);
            this->m_pPlayerShip->Update(a_pTimer->GetElapsedSeconds());
        }
        else
        {
            this->m_pNetwork->Update(a_pInput);
            if (a_pInput->KeyPressed(a_pInput->F3))
            {

                this->m_pPlayerShip = new TheBrick::CSpaceship();
                this->m_pPlayerShip->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
                ong::vec3 start(50.0f, 50.0f, 50.0f);
                for (int i = 0; i < 10; i++)
                {
                    TheBrick::CAsteroid* asteroid = new TheBrick::CAsteroid(BrickBozz::Instance()->World, start + ong::vec3((i % 2)*5.0f,(i % 2) * 5.0f,i*2.0f));
                    asteroid->Deserialize(nullptr, BrickBozz::Instance()->BrickManager, BrickBozz::Instance()->World);
                    this->m_Asteroids.push_back(asteroid);
                }
                this->gameStart = true;
            }
        }

        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
        {
            this->m_Bullets[i]->Update(a_pTimer->GetElapsedSeconds());

            if (this->m_Bullets[i]->m_lifeTime > 5.0f)
            {
                SAFE_DELETE(this->m_Bullets[i]);
                this->m_Bullets.erase(this->m_Bullets.begin() + i);
            }
        }
        printf("Fps: %s\n", std::to_string(a_pTimer->GetFPS()).c_str());


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
        Screen.m_Size = PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight);
        a_pGraphics->Begin(Screen);
        this->m_pSkyBox->Draw(this->m_pCamera, this->m_pSkyMaterial);
        if (this->gameStart)
        {
            this->m_pPlayerShip->Draw(a_pGraphics, this->m_pCamera);
            for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
                this->m_Asteroids[i]->Draw(a_pGraphics, this->m_pCamera);
        }
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            this->m_Bullets[i]->Draw(a_pGraphics, this->m_pCamera);

        PuRe_Vector3F minipos = PuRe_Vector3F(Screen.m_Size.X - 150.0f, Screen.m_Size.Y - 150.0f, 128.0f);

        PuRe_Vector3F rot = this->m_pCamera->GetRotation();
        rot *= PuRe_DegToRad;
        PuRe_MatrixF rotation = PuRe_QuaternionF(rot).GetMatrix();

        this->m_pMinimap->Draw(a_pGraphics, this->m_pUICamera, this->m_pUIMaterial, minipos, rotation);
        if (this->gameStart)
        {
            PuRe_Vector3F playerpos = TheBrick::OngToPuRe(this->m_pPlayerShip->m_pBody->getTransform().p);
            this->m_pMinimap->DrawPlayer(a_pGraphics, this->m_pUICamera, this->m_pUIMaterial, playerpos, this->m_MapBoundaries, rotation);
        }

        int nstate = this->m_pNetwork->GetState();
        if (nstate == 0)
            this->m_pFont->Draw(this->m_pUICamera, this->m_pFontMaterial, "Press << 0 >> to Host and << 1 >> to Join", PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_Vector3F(32.0f, 32.0f, 0.0f), PuRe_MatrixF::Identity(), 32.0f);
        else if (nstate == 1)
            this->m_pFont->Draw(this->m_pUICamera, this->m_pFontMaterial, ("IP: " + this->m_pNetwork->m_IP).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_Vector3F(32.0f, 32.0f, 0.0f), PuRe_MatrixF::Identity(), 32.0f);
        else if (nstate == 2)
            this->m_pFont->Draw(this->m_pUICamera, this->m_pFontMaterial, ("Port: " + this->m_pNetwork->m_Port).c_str(), PuRe_Vector3F(10.0f, gdesc.ResolutionHeight - 32.0f, 0.0f), PuRe_Vector3F(32.0f, 32.0f, 0.0f), PuRe_MatrixF::Identity(), 32.0f);


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
        SAFE_DELETE(this->m_pFontMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pSkyMaterial);
        SAFE_DELETE(this->m_pUIMaterial);
        SAFE_DELETE(this->m_pMaterial);
        // DELETE OBJECTS
        for (unsigned int i = 0; i < this->m_Bullets.size(); i++)
            SAFE_DELETE(this->m_Bullets[i]);
        for (unsigned int i = 0; i < this->m_Asteroids.size(); i++)
            SAFE_DELETE(this->m_Asteroids[i]);
        SAFE_DELETE(this->m_pPlayerShip);
        SAFE_DELETE(this->m_pSkyBox);
        SAFE_DELETE(this->m_pPointLight);
        // DELETE CAMERAS
        SAFE_DELETE(this->m_pCamera);
        SAFE_DELETE(this->m_pUICamera);
        // DELETE MODELS
        SAFE_DELETE(this->m_pModel);
        // DELETE RENDERER
        SAFE_DELETE(this->m_pRenderer);
        SAFE_DELETE(this->m_pMinimap);
        SAFE_DELETE(this->m_pFont);
    }
}