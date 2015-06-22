#include "include/Editor_ShipHandler.h"

#include "TheBrick\Serializer.h"
#include "TheBrick\Spaceship.h"
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
        this->m_pNavigation = new sba::CNavigation();
        this->m_pCurrentSpaceship = sba_ShipManager->GetShip(0);
    }

    // **************************************************************************
    // **************************************************************************
    CShipHandler::~CShipHandler()
    {

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