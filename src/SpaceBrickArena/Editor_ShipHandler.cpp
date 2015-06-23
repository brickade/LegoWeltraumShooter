#include "include/Editor_ShipHandler.h"

#include "TheBrick\Serializer.h"
#include "include/Spaceship.h"
#include "TheBrick\BrickInstance.h"
#include "TheBrick\Brick.h"
#include "TheBrick/GameObject.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CShipHandler::CShipHandler(int a_playerIdx)
    {
        this->m_playerIdx = a_playerIdx;
        int elements = sba_ShipManager->GetShipCount() - 1;
        if (elements < 0)
        {
            elements = 0;
        }
        this->m_pNavigation = new sba::CNavigation(1, elements);
        this->m_pCurrentSpaceship = sba_ShipManager->GetShip(0);
    }

    // **************************************************************************
    // **************************************************************************
    CShipHandler::~CShipHandler()
    {
        SAFE_DELETE(this->m_pCurrentSpaceship);
        SAFE_DELETE(this->m_pNavigation);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::Update()
    {
        int focus = this->m_pNavigation->GetFocusedElementId();
        this->m_pNavigation->Update(*sba_Application->GetTimer(), sba_Input->Direction(sba_Direction::Navigate, this->m_playerIdx), false);
        if (this->m_pNavigation->GetFocusedElementId() != focus)
        {
            SAFE_DELETE(this->m_pCurrentSpaceship);
            this->m_pCurrentSpaceship = sba_ShipManager->GetShip(this->m_pNavigation->GetFocusedElementId());
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CShipHandler::Render()
    {
        PuRe_Vector2F center(sba_Width / 2, sba_Height / 2);
        int focus = this->m_pNavigation->GetFocusedElementId();
        //Left
        if (focus > 0)
        {
            sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus-1], sba_Space->SpriteMaterial, PuRe_Vector3F(0 + this->m_OuterPreviewInset, center.Y, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0));
        }
        //Center
        sba_Space->RenderFont("Bricks: " + std::to_string(this->m_pCurrentSpaceship->m_pBricks.size()), center + this->m_TextOffset, 18);
        sba_Space->RenderFont("Name: " + this->m_pCurrentSpaceship->GetName(), center + this->m_TextOffset + this->m_TextStep, 18);
        sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus], sba_Space->SpriteMaterial, PuRe_Vector3F(center + this->m_PreviewOffset, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0));
        //Right
        if (focus < this->m_pNavigation->GetLastElementId())
        {
            sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus + 1], sba_Space->SpriteMaterial, PuRe_Vector3F(sba_Width - this->m_OuterPreviewInset, center.Y, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::AddShip(const char* a_pName)
    {
        sba_ShipManager->AddNewShip(a_pName);
        this->m_pNavigation->AddElement(0);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::SaveCurrentShip()
    {
        sba_ShipManager->SaveShipToFile(*this->GetCurrentSpaceShip());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::ResetCurrentShip()
    {
        sba_ShipManager->ResetShip(*this->GetCurrentSpaceShip());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::DeleteCurrentShip()
    {
        sba_ShipManager->DeleteShip(*this->GetCurrentSpaceShip());
        this->m_pNavigation->RemoveElement(this->m_pNavigation->GetFocusedElementId());
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrickInstance* CShipHandler::AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate)
    {
        TheBrick::CBrickInstance* brickInstance = a_pTemplate.m_pBrick->CreateInstance(*this->GetCurrentSpaceShip(), *sba_World);
        brickInstance->SetTransform(a_pTemplate.GetTransform());
        brickInstance->m_Color = a_pTemplate.m_Color;
        return brickInstance;
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::UpdateCurrentShipPreview()
    {
        sba_ShipManager->UpdateSprite(*this->GetCurrentSpaceShip());
    }
}