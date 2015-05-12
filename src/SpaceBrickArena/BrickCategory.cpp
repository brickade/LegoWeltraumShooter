#include "include/BrickCategory.h"
namespace Game
{
    // **************************************************************************
    // **************************************************************************
    CBrickCategory::CBrickCategory(int a_Id)
    {
        this->m_Id = a_Id;
        this->m_Bricks = BrickBozz::Instance()->BrickManager->GetCategoryStart(a_Id);
        int lastBrickSubId = -1;
        while (this->m_Bricks[lastBrickSubId + 1] != nullptr)
        {
            lastBrickSubId++;
        }
        this->m_pNavigation = new CNavigation(3, lastBrickSubId);
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
        this->m_Rotation += a_pTimer->GetElapsedSeconds() * this->m_RotationSpeed;
        this->m_Rotation = fmod(this->m_Rotation, 6.28318531f);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        for (int i = 0; i <= this->m_pNavigation->GetLastElementId(); i++)
        {
            PuRe_Vector2F listPos = PuRe_Vector2F(i % this->m_pNavigation->GetElementsCountPerLine(), floor(i / this->m_pNavigation->GetElementsCountPerLine()));
            PuRe_Vector3F pos = PuRe_Vector3F(this->m_ListStart + this->m_ListStep * listPos, 0);
            pos.Y = a_pGraphics->GetDescription().ResolutionHeight - pos.Y; //Invert Y

            PuRe_MatrixF rot = PuRe_MatrixF::Translation(-this->m_Bricks[i]->GetPivotOffset() * this->m_ElementSize) * PuRe_MatrixF::RotationAxis(PuRe_Vector3F(0, 1, 0), this->m_Rotation) * PuRe_MatrixF::Rotation(-this->m_Pitch, 0, 0);
            
            PuRe_Color color = PuRe_Color(0, 0.05f, 0);
            if (this->m_pNavigation->GetFocusedElementId() == i)
            {
                color = PuRe_Color(0, 0.01f, 0);
            }

            float size = this->m_ElementSize / this->m_Bricks[i]->GetPivotOffset().Length();
            size += this->m_ElementSize;
            size *= 0.5f;
            this->m_Bricks[i]->Draw(a_pGraphics, a_pCamera, pos, rot, color, PuRe_Vector3F(size, size, size));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::RenderTab(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, bool a_IsSelected)
    {
        PuRe_Vector3F pos = PuRe_Vector3F(this->m_TabStart + this->m_TabStep * this->m_Id, 0);
        pos.Y = a_pGraphics->GetDescription().ResolutionHeight - pos.Y; //Invert Y

        PuRe_MatrixF rot = PuRe_MatrixF::Translation(-this->m_Bricks[0]->GetPivotOffset() * this->m_TabSize) * PuRe_MatrixF::RotationAxis(PuRe_Vector3F(0, 1, 0), this->m_TabRotation) * PuRe_MatrixF::Rotation(-this->m_Pitch, 0, 0);
        
        PuRe_Color color = PuRe_Color(0.05f, 0, 0);
        if (a_IsSelected)
        {
            color = PuRe_Color(0.01f, 0, 0);
        }

        float size = this->m_TabSize / this->m_Bricks[0]->GetPivotOffset().Length();
        size += this->m_TabSize;
        size *= 0.5f;
        this->m_Bricks[0]->Draw(a_pGraphics, a_pCamera, pos, rot, color, PuRe_Vector3F(size, size, size));
    }
}