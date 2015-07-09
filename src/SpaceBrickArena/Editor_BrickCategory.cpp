#include "include/Editor_BrickCategory.h"

#include "include/Space.h"

namespace Editor
{
    const int CBrickCategory::CATEGORY_WEAPONS = 8;
    const int CBrickCategory::CATEGORY_ENGINES = 7;
    const int CBrickCategory::CATEGORY_COCKPITS = 2;

    // **************************************************************************
    // **************************************************************************
    CBrickCategory::CBrickCategory(int a_Id)
    {
        this->m_Id = a_Id;
        this->m_Bricks = sba_BrickManager->GetCategoryStart(a_Id);
        int lastBrickSubId = -1;
        while (this->m_Bricks[lastBrickSubId + 1] != nullptr)
        {
            lastBrickSubId++;
        }
        this->m_pNavigation = new sba::CNavigation(3, lastBrickSubId);
    }

    // **************************************************************************
    // **************************************************************************
    CBrickCategory::~CBrickCategory()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, int a_PlayerIdx, float a_RotationSpeed)
    {
        this->m_pNavigation->Update(a_pTimer, sba_Input->Direction(sba_Direction::EditorChooseElement, a_PlayerIdx));
        this->m_Rotation += a_pTimer.GetElapsedSeconds() * a_RotationSpeed;
        this->m_Rotation = fmod(this->m_Rotation, 6.28318531f);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::Render(PuRe_IGraphics& a_pGraphics, float a_Visibility)
    {
        for (int i = 0; i <= this->m_pNavigation->GetLastElementId(); i++)
        {
            PuRe_Vector2F listPos = PuRe_Vector2F((float)(i % this->m_pNavigation->GetElementsCountPerLine()), floorf(i / (float)this->m_pNavigation->GetElementsCountPerLine()));
            if (this->m_pNavigation->GetLineCount() > this->m_ListShowMax)
            { //Scrolling needed
                //Calculate current scroll value
                this->m_ListScrollCurrent = max(this->m_pNavigation->GetFocusedLineId() - this->m_ListScrollStart, 0);
                //Scroll
                listPos.Y -= this->m_ListScrollCurrent;
                //Dont scroll last line
                listPos.Y += max(this->m_ListShowMax + this->m_ListScrollCurrent - this->m_pNavigation->GetLineCount(), 0);
                if (listPos.Y < 0 || listPos.Y > this->m_ListShowMax)
                {
                    continue;
                }
            }
            PuRe_Vector3F pos = PuRe_Vector3F(this->m_ListStart + this->m_ListStep * listPos, 0);
            pos.Y = a_pGraphics.GetDescription().ResolutionHeight - pos.Y; //Invert Y
            pos.X -= (1.0f - a_Visibility) * (this->m_pNavigation->GetElementsCountPerLine() * this->m_ListStep.X + this->m_ListStart.X);
            assert(this->m_Bricks[i]->GetPivotOffset().Length() > FLT_EPSILON * 5);
            PuRe_MatrixF rot = PuRe_MatrixF::Translation(-this->m_Bricks[i]->GetPivotOffset() * this->m_ElementSize) * PuRe_MatrixF::RotationAxis(PuRe_Vector3F(0, 1, 0), this->m_Rotation) * PuRe_MatrixF::Rotation(-this->m_Pitch, 0, 0);

            PuRe_Color color = PuRe_Color(0, 1, 0);
            if (this->m_pNavigation->GetFocusedElementId() == i)
            {
                color = PuRe_Color(0, 0, 1);
            }

            float size = this->m_ElementSize / this->m_Bricks[i]->GetPivotOffset().Length();
            size += this->m_ElementSize;
            size *= 0.5f;
            //Tmp Engine resize
            if (this->m_Bricks[i]->GetCategoryId() == CBrickCategory::CATEGORY_ENGINES)
            {
                size *= 0.65f;
            }
            sba_Renderer->Draw(1, false, this->m_Bricks[i]->GetModel(), PuRe_Primitive::Triangles, sba_BrickManager->GetBrickUIMaterial(), pos, rot, PuRe_Vector3F::Zero(), PuRe_Vector3F(size, size, size), color);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickCategory::RenderTab(PuRe_IGraphics& a_pGraphics, float a_TabRotation, bool a_IsSelected, float a_Visibility)
    {
        PuRe_Vector3F pos = PuRe_Vector3F(this->m_TabStart + this->m_TabStep * (float)this->m_Id, 0);
        pos.Y = a_pGraphics.GetDescription().ResolutionHeight - pos.Y; //Invert Y
        pos.X -= (1.0f - a_Visibility) * (this->m_pNavigation->GetElementsCountPerLine() * this->m_ListStep.X + this->m_ListStart.X);

        PuRe_MatrixF rot = PuRe_MatrixF::Translation(-this->m_Bricks[0]->GetPivotOffset() * this->m_TabSize) * PuRe_MatrixF::RotationAxis(PuRe_Vector3F(0, 1, 0), a_TabRotation) * PuRe_MatrixF::Rotation(-this->m_Pitch, 0, 0);

        PuRe_Color color = PuRe_Color(1, 0, 0);
        if (a_IsSelected)
        {
            color = PuRe_Color(0, 1, 1);
        }

        float size = this->m_TabSize / this->m_Bricks[0]->GetPivotOffset().Length();
        size += this->m_TabSize;
        size *= 0.5f;
        if (this->m_Bricks[0]->GetCategoryId() == CBrickCategory::CATEGORY_ENGINES)
        {
            size *= 0.65f;
        }
        sba_Renderer->Draw(1, false, this->m_Bricks[0]->GetModel(), PuRe_Primitive::Triangles, sba_BrickManager->GetBrickUIMaterial(), pos, rot, PuRe_Vector3F::Zero(), PuRe_Vector3F(size, size, size), color);
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrick* CBrickCategory::GetFocusedBrick() const
    {
        return this->m_Bricks[this->m_pNavigation->GetFocusedElementId()];
    }
}