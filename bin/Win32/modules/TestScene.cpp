#include "include/TestScene.h"
namespace Game
{

    CTestScene::CTestScene(PuRe_Application* a_pApplication)
    {
        this->m_pApplication = a_pApplication;
    }
    // **************************************************************************
    // **************************************************************************
    void CTestScene::ReceiveData()
    {
        char Buffer[256];
        memset(Buffer, 0, 256);
        while (this->m_SocketWork)
        {
            if (this->m_pSocket->Receive(Buffer, 256, &to) != -1)
            {
                std::string text = Buffer;
                if (text == "Test")
                    printf("Test Received!\n");
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CTestScene::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_SoundPlayer* a_pSoundPlayer)
    {

        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //Camera
        this->m_pCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective);
        //this->m_pCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth / 4, (float)gdesc.ResolutionHeight / 4), PuRe_Camera_Perspective);
        this->m_pCamera2 = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Perspective);
        this->m_pPostCamera = new PuRe_Camera(PuRe_Vector2F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight), PuRe_Camera_Orthogonal);
        this->m_pCamera->SetFoV(45.0f);
        this->m_pCamera2->SetFoV(45.0f);
        this->m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/default/default");
        this->m_pPointLightMaterial = a_pGraphics->LoadMaterial("../data/effects/PointLight/default");
        this->m_pDirectionalMaterial = a_pGraphics->LoadMaterial("../data/effects/DirectionalLight/default");
        this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
        this->m_pInstancedMaterial = a_pGraphics->LoadMaterial("../data/effects/brick/default");
        this->m_pSkyBoxMaterial = a_pGraphics->LoadMaterial("../data/effects/skybox/default");
        this->m_pSSAOMaterial = a_pGraphics->LoadMaterial("../data/effects/SSAO/default");
        this->m_pFontMaterial = a_pGraphics->LoadMaterial("../data/effects/font/default");
        this->m_pModel = new PuRe_Model(a_pGraphics, "../data/models/Brick1x1.obj");
        //this->m_pRenderTarget = a_pGraphics->CreateRendertarget();
        this->m_pPointLight = new PuRe_PointLight(a_pGraphics);
        this->m_pDirectionalLight = new PuRe_DirectionalLight(a_pGraphics);
        this->rot = 0.0f;
        this->textureID = 0;
        //this->m_pSkyBox = new PuRe_SkyBox(a_pGraphics, "../data/textures/cube/");
        this->m_pRenderer = new PuRe_Renderer(a_pGraphics,PuRe_Vector2I(gdesc.ResolutionWidth,gdesc.ResolutionHeight));
        this->m_pRenderer->SetSSAO(this->m_pSSAOMaterial, "../data/textures/ssao.jpg");
        this->m_pSprite = new PuRe_Sprite(a_pGraphics,"../data/textures/dirt.png");
        this->m_pFont = new PuRe_Font(a_pGraphics, "../data/textures/font.png");

    }

    // **************************************************************************
    // **************************************************************************
    bool CTestScene::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        this->m_pTimer = a_pTimer;
        //Handle ESC Button
        if (a_pInput->KeyPressed(a_pInput->ESC) && a_pInput->GetCursorLock())
            a_pInput->UnLockCursor();
        else if (a_pInput->KeyPressed(a_pInput->ESC))
            return true;

        if (a_pInput->KeyPressed(a_pInput->F1))
        {
            SAFE_DELETE(this->m_pMaterial);
            SAFE_DELETE(this->m_pFontMaterial);
            SAFE_DELETE(this->m_pPointLightMaterial);
            SAFE_DELETE(this->m_pDirectionalMaterial);
            SAFE_DELETE(this->m_pPostMaterial);
            SAFE_DELETE(this->m_pInstancedMaterial);
            SAFE_DELETE(this->m_pSkyBoxMaterial);
            SAFE_DELETE(this->m_pSSAOMaterial);
            this->m_pFontMaterial = a_pGraphics->LoadMaterial("../data/effects/font/default");
            this->m_pMaterial = a_pGraphics->LoadMaterial("../data/effects/default/default");
            this->m_pPointLightMaterial = a_pGraphics->LoadMaterial("../data/effects/PointLight/default");
            this->m_pDirectionalMaterial = a_pGraphics->LoadMaterial("../data/effects/DirectionalLight/default");
            this->m_pPostMaterial = a_pGraphics->LoadMaterial("../data/effects/Post/default");
            this->m_pInstancedMaterial = a_pGraphics->LoadMaterial("../data/effects/brick/default");
            this->m_pSkyBoxMaterial = a_pGraphics->LoadMaterial("../data/effects/skybox/default");
            this->m_pSSAOMaterial = a_pGraphics->LoadMaterial("../data/effects/SSAO/default");
        }
        if (a_pInput->KeyPressed(a_pInput->F2))
        {
            this->m_pSocket = new PuRe_Socket("127.0.0.1", 13370, true);
            this->m_SocketWork = true;
            to = this->m_pSocket->GetAddress();
            host = true;
            std::thread receiveThread(&CTestScene::ReceiveData, this);
            receiveThread.detach();
        }
        if (a_pInput->KeyPressed(a_pInput->F3))
        {
            this->m_pSocket = new PuRe_Socket("127.0.0.1", 13370, false);
            this->m_SocketWork = true;
            to = this->m_pSocket->GetAddress();
            host = false;
            std::thread receiveThread(&CTestScene::ReceiveData, this);
            receiveThread.detach();
        }
        if (a_pInput->KeyPressed(a_pInput->F4))
        {
            const char* buffer = "Test";
            if (this->m_pSocket != NULL)
                this->m_pSocket->Send(to, buffer, strlen(buffer));
        }

        if (a_pInput->KeyPressed(a_pInput->Left))
        {
            this->textureID--;
            if (this->textureID < 0)
                this->textureID = 4;
        }

        else if (a_pInput->KeyPressed(a_pInput->Right))
        {
            this->textureID++;
            if (this->textureID > 4)
                this->textureID = 0;
        }
        //Seconds for frame independent movement
        float Seconds = a_pTimer->GetElapsedSeconds();
        this->rot += Seconds*1.0f;

        PuRe_Vector3F CameraMove;
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();
        if (a_pInput->MouseIsPressed(a_pInput->LeftClick))
        {

            PuRe_Vector2F speed = a_pInput->GetRelativeMousePosition();
            speed *= 10.0f;
            PuRe_Vector3F cameraLook = PuRe_Vector3F();
            if (speed.X != 0.0f)
                cameraLook.X += speed.X*Seconds;
            if (speed.Y != 0.0f)
                cameraLook.Y += speed.Y*Seconds;
            this->m_pCamera->Rotate(cameraLook.Y, cameraLook.X, cameraLook.Z);
        }
        if (a_pInput->MouseIsPressed(a_pInput->RightClick))
        {

            PuRe_Vector2F speed = a_pInput->GetRelativeMousePosition();
            speed *= 10.0f;
            PuRe_Vector3F cameraLook = PuRe_Vector3F();
            if (speed.X != 0.0f)
                cameraLook.X += speed.X*Seconds;
            if (speed.Y != 0.0f)
                cameraLook.Y += speed.Y*Seconds;
            this->m_pCamera2->Rotate(cameraLook.Y, cameraLook.X, cameraLook.Z);
        }
        float speed = 10.0f*Seconds;
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
        CameraMove = PuRe_Vector3F();

        if (a_pInput->KeyIsPressed(a_pInput->I))
            CameraMove.Z += speed;
        else if (a_pInput->KeyIsPressed(a_pInput->K))
            CameraMove.Z -= speed;
        if (a_pInput->KeyIsPressed(a_pInput->L))
            CameraMove.X += speed;
        else if (a_pInput->KeyIsPressed(a_pInput->J))
            CameraMove.X -= speed;

        this->m_pCamera2->Move(CameraMove);
        for (int i = 0; i < 4; i++)
        {
            if (a_pInput->GamepadPressed(a_pInput->Pad_A, i))
                printf("A pressed by %i\n", i);
        }
        if (a_pInput->KeyIsPressed(a_pInput->F10))
            printf("FPS: %i\n", a_pTimer->GetFPS());

        float sec = a_pTimer->GetTotalElapsedSeconds();
        sec = a_pTimer->GetTotalElapsedSeconds() - sec;

        return false;
    }

    // **************************************************************************
    // **************************************************************************
    void CTestScene::Render(PuRe_IGraphics* a_pGraphics)
    {
        PuRe_Color PuRe_CLEAR = PuRe_Color(0.0f, 0.4f, 1.0f, 1.0f);
        PuRe_GraphicsDescription gdesc = a_pGraphics->GetDescription();

        //a_pGraphics->Clear(PuRe_CLEAR);
        //PuRe_CLEAR = PuRe_Color(0.0f, 0.0f, 0.0f, 0.0f);
        //for (int g=0;g<2;g++)
        //{
        //    PuRe_Camera* activeCam;
        //    if (g == 0)
        //        activeCam = this->m_pCamera;
        //    else
        //        activeCam = this->m_pCamera2;
        //    //////////////// DRAW GEOMETRY ////////////////////
        //    this->m_pRenderTarget->ApplyGeometryPass(PuRe_CLEAR);


        //    int instances = 10000;
        //    bool* in = new bool[instances];
        //    int drawInstances = 0;
        //    for (int i = 0; i < instances; i++)
        //    {
        //        PuRe_Vector3F trans = PuRe_Vector3F((i % 10)*4.0f, ((int)(i / 150))*4.0f, (float)(((int)((i / 10)*4.0f)) % 50));
        //        PuRe_BoundingBox box = this->m_pModel->GetBoundingBox();
        //        box.m_Position += trans;
        //        box.m_Size += trans;
        //        in[i] = activeCam->BoxInFrustum(&box);
        //        if (in[i])
        //            drawInstances++;
        //    }
        //    if (drawInstances != 0)
        //    {
        //        printf("DRAW: %i\n",drawInstances);
        //        this->m_pInstanceBuffer = a_pGraphics->CreateInstanceBuffer(drawInstances, sizeof(PuReEngine::Core::SInstance));
        //        PuReEngine::Core::SInstance* inst = (PuReEngine::Core::SInstance*)this->m_pInstanceBuffer->Lock();
        //        int count = 0;
        //        for (int i = 0; i < instances; i++)
        //        {
        //            if (in[i])
        //            {
        //                PuRe_MatrixF rot = PuRe_QuaternionF(PuRe_Vector3F(0.0f, this->rot, 0.0f)).GetMatrix();
        //                PuRe_MatrixF scale = PuRe_MatrixF::Scale(PuRe_Vector3F(1.0f, 1.0f, 1.0f));
        //                PuRe_Vector3F trans = PuRe_Vector3F((i % 10)*4.0f, ((int)(i / 150))*4.0f, (float)(((int)((i / 10)*4.0f)) % 50));
        //                PuRe_MatrixF translate = PuRe_MatrixF::Translation(trans);
        //                inst[count].World = scale * rot * translate;
        //                inst[count].Color = PuRe_Color(1.0f,1.0f,1.0f);
        //                count++;
        //            }
        //        }

        //        this->m_pInstanceBuffer->Unlock();
        //        this->m_pModel->DrawInstanced(activeCam, this->m_pInstancedMaterial, PuRe_Primitive::Triangles, this->m_pInstanceBuffer);
        //        SAFE_DELETE(this->m_pInstanceBuffer);
        //    }
        //    SAFE_DELETE(in);

        //    //////////////////// END GEOMETRY ////////////////////

        //    //////////////////// DRAW LIGHT ////////////////////
        //    this->m_pRenderTarget->ApplyLightPass(PuRe_CLEAR);
        //    this->m_pPointLight->Draw(a_pGraphics, activeCam, this->m_pPostCamera, this->m_pRenderTarget, PuRe_Vector3F(0.0f, 0.0f, 0.0f), PuRe_Vector3F(1.0f, 0.0f, 0.0f), 1.0f, 0.01f, 0.01f);


        //    //////////////// END LIGHT ////////////////////


        //    ////////////////// FINAL PASS ////////////////////
        //    a_pGraphics->Begin();

        //    this->m_pSkyBox->Draw(activeCam, this->m_pSkyBoxMaterial, PuRe_Vector3F(0.0f, 0.0f, 0.0f));

        //    this->m_pPostMaterial->Apply();
        //    this->m_pPostMaterial->SetFloat((float)textureID, "textureID");
        //    this->m_pPostMaterial->SetVector3(PuRe_Vector3F(0.0f, 0.0f, 0.0f), "ambient");
        //    this->m_pRenderTarget->Draw(this->m_pPostCamera, this->m_pPostMaterial, PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f), PuRe_Vector3F(gdesc.ResolutionWidth / 2.0f, gdesc.ResolutionHeight / 2.0f, 0.0f));
        //    }

        //a_pGraphics->End();
        this->m_pRenderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(2.0f, 0.0f, 2.0f), PuRe_Color(1.0f, 0.0f, 0.0f));
        this->m_pRenderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(0.0f, 0.0f, 5.0f), PuRe_Color(1.0f, 1.0f, 0.0f));
        this->m_pRenderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(2.0f, 3.0f, 10.0f), PuRe_Color(1.0f, 0.0f, 1.0f));
        this->m_pRenderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(5.0f, 10.0f, 2.0f), PuRe_Color(1.0f, 1.0f, 1.0f));
        this->m_pRenderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(5.0f, 30.0f, 2.0f), PuRe_Color(0.0f, 1.0f, 0.0f));
        this->m_pRenderer->Draw(this->m_pPointLight, this->m_pPointLightMaterial, PuRe_Vector3F(2.0f, 20.0f, 5.0f), PuRe_Color(0.0f, 0.0f, 1.0f));

        this->m_pRenderer->Set(3.0f, "floatname");
        //this->m_pRenderer->Draw(this->m_pSkyBox, this->m_pSkyBoxMaterial);

        int instances = 1000;
        PuRe_RenderInstance* renderInstance = new PuRe_RenderInstance[instances];
        float x_diff = 2.0f;
        float y_diff = 2.0f;
        float z_diff = 2.0f;
        int x_Bricks = 10;
        int z_Bricks = 10;
        int y_Bricks = 50;
        for (int i = 0; i < instances; i++)
        {
            renderInstance[i].Center = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
            renderInstance[i].Size = PuRe_Vector3F(1.0f,1.0f,1.0f);
            renderInstance[i].Rotation = PuRe_QuaternionF(PuRe_Vector3F(0.0f, this->rot, 0.0f)).GetMatrix();
            renderInstance[i].Color = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
            renderInstance[i].Position = PuRe_Vector3F(-5.0f, 0.0f, -5.0f);
            renderInstance[i].Position += PuRe_Vector3F(x_diff* (i % x_Bricks), y_diff * (i / y_Bricks), z_diff*((i / z_Bricks) % (y_Bricks / 10)) + 1.0f);
        }
        //PuRe_Box q(a_pGraphics);
        //this->m_pRenderer->Draw(q.GetBuffer(), q.GetBuffer()->GetSize(), PuRe_Primitive::Linestrip, this->m_pInstancedMaterial, renderInstance, instances);
        this->m_pRenderer->Draw(this->m_pModel, PuRe_Primitive::Triangles, this->m_pInstancedMaterial, renderInstance, instances);
        this->m_pRenderer->DrawUI(this->m_pModel, PuRe_Primitive::Triangles, this->m_pMaterial, PuRe_Vector3F(100.0f, 100.0f, 1.0f), PuRe_Vector3F(10.0f*PuRe_DegToRad, this->rot, 0.0f), PuRe_Vector3F(), PuRe_Vector3F(100.0f, 100.0f, 100.0f));



        PuRe_CLEAR = PuRe_Color(0.0f, 0.4f, 1.0f, 1.0f);
        PuRe_Vector3F size = PuRe_Vector3F((float)gdesc.ResolutionWidth, (float)gdesc.ResolutionHeight, 0.0f);
        this->m_pRenderer->Set((float)this->textureID, "textureID");
        this->m_pRenderer->Begin(PuRe_CLEAR); // Player 1 Cam
        this->m_pRenderer->Render(this->m_pCamera, this->m_pPostMaterial); // Player 1 Cam


        this->m_pRenderer->End();


        SAFE_DELETE(renderInstance);


        ///////////////// END FINAL PASS ////////////////////
    }

    // **************************************************************************
    // **************************************************************************
    void CTestScene::Exit()
    {
        this->m_SocketWork = false;
        //DELETE OBJECTS
        SAFE_DELETE(this->m_pFont);
        SAFE_DELETE(this->m_pModel);
        SAFE_DELETE(this->m_pSprite);
        SAFE_DELETE(this->m_pSkyBox);
        //DELETE RENDERING
        SAFE_DELETE(this->m_pRenderer);
        SAFE_DELETE(this->m_pPointLight);
        SAFE_DELETE(this->m_pDirectionalLight);
        //SAFE_DELETE(this->m_pRenderTarget);
        //DELETE MATERIALS
        SAFE_DELETE(this->m_pFontMaterial);
        SAFE_DELETE(this->m_pSSAOMaterial);
        SAFE_DELETE(this->m_pSkyBoxMaterial);
        SAFE_DELETE(this->m_pInstancedMaterial);
        SAFE_DELETE(this->m_pDirectionalMaterial);
        SAFE_DELETE(this->m_pPointLightMaterial);
        SAFE_DELETE(this->m_pPostMaterial);
        SAFE_DELETE(this->m_pMaterial);
        //DELETE CAMERA
        SAFE_DELETE(this->m_pPostCamera);
        SAFE_DELETE(this->m_pCamera);
        //DELETE Socket
        SAFE_DELETE(this->m_pSocket);
    }
}
