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
        this->m_RenameShip = false;
        this->m_pInputField = new sba::CInputField((sba::CInputField::InputTypes::Type)(sba::CInputField::InputTypes::NUMBERS | sba::CInputField::InputTypes::LETTERS_UPPERCASE | sba::CInputField::InputTypes::LETTERS_LOWERCASE | sba::CInputField::InputTypes::UNDERLINE));
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
        if (sba_Input->ButtonPressed(sba_Button::EditorRenameShip, this->m_playerIdx))
        {
            this->m_RenameShip = !this->m_RenameShip;
            if (this->m_RenameShip)
            {
                this->m_OldShipName = this->m_pCurrentSpaceship->GetName();
                this->m_pInputField->SetValue(this->m_OldShipName);
            }
        }
        if (this->m_RenameShip)
        {
            if (this->m_pInputField->Update(*sba_Application->GetInput()))
            { //Close Rename
                this->m_RenameShip = false;
                if (this->m_pInputField->GetValue().size() == 0)
                {
                    this->m_pInputField->SetValue("GiveMeAName_Idiot");
                }
            }
            this->m_pCurrentSpaceship->SetName(this->m_pInputField->GetValue()); //Update Name
            if (!this->m_RenameShip && this->m_pCurrentSpaceship->GetName() != this->m_OldShipName)
            { //Ship was renamed
                this->UpdateCurrentShipName(); //Save to disk
            }
        }
        else
        {
            int focus = this->m_pNavigation->GetFocusedElementId();
            this->m_pNavigation->Update(*sba_Application->GetTimer(), sba_Input->Direction(sba_Direction::Navigate, this->m_playerIdx), false);
            if (this->m_pNavigation->GetFocusedElementId() != focus)
            {
                SAFE_DELETE(this->m_pCurrentSpaceship);
                this->m_pCurrentSpaceship = sba_ShipManager->GetShip(this->m_pNavigation->GetFocusedElementId());
            }
            if (sba_Input->ButtonPressed(sba_Button::EditorAddNewShip, this->m_playerIdx))
            {
                this->AddShip("Banana");
            }
            if (sba_Input->ButtonPressed(sba_Button::EditorDeleteShip, this->m_playerIdx) && this->GetCurrentSpaceShip() != nullptr)
            {
                this->DeleteCurrentShip();
            }
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
            sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus - 1], sba_Space->SpriteMaterial, PuRe_Vector3F(0 + this->m_OuterPreviewInset, center.Y + this->m_OuterPreviewYOffset, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0), PuRe_Vector3F(this->m_OuterPreviewSize, this->m_OuterPreviewSize, this->m_OuterPreviewSize));
        }
        //Center
        sba_Space->RenderFont("Ship " + std::to_string(focus + 1) + "/" + std::to_string(sba_ShipManager->GetShipCount()), center + this->m_TextOffset + this->m_TextStep * 0, 18);
        sba_Space->RenderFont("\"" + this->m_pCurrentSpaceship->GetName() + "\"", center + this->m_TextOffset + this->m_TextStep * 1, this->m_RenameShip ? 24 : 18);
        sba_Space->RenderFont(std::to_string(this->m_pCurrentSpaceship->m_pBricks.size()) + " Bricks", center + this->m_TextOffset + this->m_TextStep * 2, 18);
        
        sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus], sba_Space->SpriteMaterial, PuRe_Vector3F(center + this->m_PreviewOffset, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0));
        //Right
        if (focus < this->m_pNavigation->GetLastElementId())
        {
            sba_Renderer->Draw(1, false, (*sba_ShipManager)[focus + 1], sba_Space->SpriteMaterial, PuRe_Vector3F(sba_Width - this->m_OuterPreviewInset, center.Y + this->m_OuterPreviewYOffset, 0), PuRe_MatrixF::Identity(), PuRe_Vector3F((*sba_ShipManager)[focus]->GetSize() * 0.5f, 0), PuRe_Vector3F(this->m_OuterPreviewSize, this->m_OuterPreviewSize, this->m_OuterPreviewSize));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::AddShip(const char* a_pName)
    {
        SAFE_DELETE(this->m_pCurrentSpaceship);
        sba_ShipManager->AddNewShip(a_pName);
        this->m_pNavigation->AddElement();
        this->m_pNavigation->SetFocusedElementId(this->m_pNavigation->GetLastElementId());
        this->m_pCurrentSpaceship = sba_ShipManager->GetShip(this->m_pNavigation->GetFocusedElementId());
        this->m_RenameShip = true;
        this->m_OldShipName = this->m_pCurrentSpaceship->GetName();
        this->m_pInputField->SetValue(this->m_OldShipName);
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
        SAFE_DELETE(this->m_pCurrentSpaceship);
        this->m_pCurrentSpaceship = sba_ShipManager->GetShip(this->m_pNavigation->GetFocusedElementId());
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

    // **************************************************************************
    // **************************************************************************
    void CShipHandler::UpdateCurrentShipName()
    {
        sba_ShipManager->UpdateShipName(*this->GetCurrentSpaceShip(), this->m_OldShipName);
    }
}