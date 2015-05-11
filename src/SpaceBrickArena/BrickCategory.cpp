#include "include/BrickCategory.h"
namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CBrickCategory::CBrickCategory(int a_Id)
    {
        this->m_Bricks = BrickBozz::Instance()->BrickManager->GetCategoryStart(a_Id);
        int lastBrickSubId = -1;
        while (this->m_Bricks[lastBrickSubId + 1] != nullptr)
        {
            lastBrickSubId++;
        }
        this->m_pNavigation = new CNavigation(4, lastBrickSubId);
    }

    // **************************************************************************
    // **************************************************************************
    CBrickCategory::~CBrickCategory()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, int a_PlayerIdx)
    {
        PuRe_Vector2F navInput = PuRe_Vector2F::Zero();
        //Gamepad
        navInput += a_pInput->GetGamepadLeftThumb(a_PlayerIdx);
        //Keyboard
        if (a_pInput->KeyPressed(a_pInput->Right))
        {
            navInput.X += 1.0f;
        }
        if (a_pInput->KeyPressed(a_pInput->Left))
        {
            navInput.X -= 1.0f;
        }
        if (a_pInput->KeyPressed(a_pInput->Up))
        {
            navInput.Y += 1.0f;
        }
        if (a_pInput->KeyPressed(a_pInput->Down))
        {
            navInput.Y -= 1.0f;
        }

        this->m_pNavigation->Update(a_pTimer, navInput);
        //printf("%f\n", a_pTimer->GetTotalElapsedSeconds());
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        for (int i = 0; i <= this->m_pNavigation->GetLastElementId(); i++)
        {
            PuRe_Vector3F pos = PuRe_Vector3F(50 + 50 * (i % this->m_pNavigation->GetElementsCountPerLine()), 50 + 50 * floor(i / this->m_pNavigation->GetElementsCountPerLine()), 1);
            PuRe_MatrixF rot = PuRe_MatrixF::Identity();
            pos = PuRe_Vector3F(0, 0, 0);
            //pos -= PuRe_Vector3F(a_pGraphics->GetDescription().ResolutionWidth / 2, a_pGraphics->GetDescription().ResolutionHeight / 2, 0)
            this->m_Bricks[i]->Draw(a_pGraphics, a_pCamera, pos, rot);
        }
    }
}