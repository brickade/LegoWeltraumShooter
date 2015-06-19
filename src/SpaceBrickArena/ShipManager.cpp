#include "include/ShipManager.h"

#include "include/Space.h"
#include "TheBrick/Serializer.h"
#include "TheBrick/Brick.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CShipManager::CShipManager()
    {
        
    }

    // **************************************************************************
    // **************************************************************************
    CShipManager::~CShipManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::Load()
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        int i = 0;
        PuRe_IWindow* window = sba_Application->GetWindow();
        std::string file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        while (file != "")
        {
            //Get Ship
            TheBrick::CSpaceship* ship = new TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
            serializer->OpenRead(std::string(file).insert(0, this->m_FolderPath).c_str());
            ship->Deserialize(*serializer, *sba_BrickManager, *sba_World);
            serializer->Close();
            //Set Name
            ship->SetNameFromFilename(file);
            //Register Ship
            this->AddShip(ship);
            //Next
            file = window->GetFileAtIndex(i, this->m_FolderPath.c_str());
        }
        delete serializer;
        if (this->m_Spaceships.size() == 0)
        {
            this->AddNewShip("Banana.ship");
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddNewShip(const char* a_pName)
    {
        TheBrick::CSpaceship* ship = new TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
        ship->SetNameFromFilename(a_pName);
        //Register
        this->AddShip(ship);
        //Set to default
        this->ResetShip(this->m_Spaceships.size() - 1);
        //Save to file
        this->SaveShip(this->m_Spaceships.size() - 1);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::AddShip(TheBrick::CSpaceship* a_pShip)
    {
        this->m_Spaceships.push_back(a_pShip);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::SaveShip(int a_Index)
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if (serializer->OpenWrite(this->PathFromShip(this->m_Spaceships[a_Index])))
        {
            this->m_Spaceships[a_Index]->Serialize(*serializer);
        }
        serializer->Close();
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::ResetShip(int a_Index)
    {
        SAFE_DELETE(this->m_Spaceships[a_Index]);
        delete this->m_Spaceships[a_Index];
        this->m_Spaceships[a_Index] = new TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
        TheBrick::CBrickInstance* brickInstance = sba_BrickManager->GetBrick(1).CreateInstance(*this->m_Spaceships[a_Index], *sba_World);
        brickInstance->SetTransform(ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
        brickInstance->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, 0.0f, 0.0f));
        brickInstance->m_Color = PuRe_Color(0, 0, 1);
    }

    // **************************************************************************
    // **************************************************************************
    void CShipManager::DeleteShip(int a_Index)
    {
        delete this->m_Spaceships[a_Index];
        this->m_Spaceships.erase(this->m_Spaceships.begin() + a_Index);
    }
}