#include "include/Editor_ShipWorker.h"

#include "TheBrick\Serializer.h"
#include "TheBrick\Spaceship.h"
#include "TheBrick\BrickInstance.h"
#include "TheBrick\BrickManager.h"
#include "TheBrick\Brick.h"
#include "TheBrick/GameObject.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CShipWorker::CShipWorker()
    {
        this->m_pNavigation = new sba::CNavigation();
    }

    // **************************************************************************
    // **************************************************************************
    CShipWorker::~CShipWorker()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CShipWorker::AddShip(const char* a_pName)
    {
        sba_ShipManager->AddNewShip(a_pName);
        this->m_pNavigation->AddElement(0);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipWorker::SaveCurrentShip()
    {
        sba_ShipManager->SaveShip(this->m_pNavigation->GetFocusedElementId());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipWorker::ResetCurrentShip()
    {
        sba_ShipManager->ResetShip(this->m_pNavigation->GetFocusedElementId());
    }

    // **************************************************************************
    // **************************************************************************
    void CShipWorker::DeleteCurrentShip()
    {
        sba_ShipManager->DeleteShip(this->m_pNavigation->GetFocusedElementId());
        this->m_pNavigation->RemoveElement(this->m_pNavigation->GetFocusedElementId());
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrickInstance* CShipWorker::AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate)
    {
        TheBrick::CBrickInstance* brickInstance = a_pTemplate.m_pBrick->CreateInstance(*this->GetCurrentSpaceShip(), *sba_World);
        brickInstance->SetTransform(a_pTemplate.GetTransform());
        brickInstance->m_Color = a_pTemplate.m_Color;
        return brickInstance;
    }
}