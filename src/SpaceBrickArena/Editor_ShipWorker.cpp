#include "include/Editor_ShipWorker.h"

#include "include\Space.h"
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
    }

    // **************************************************************************
    // **************************************************************************
    CShipWorker::~CShipWorker()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CShipWorker::LoadShipFromFile(const char* a_pFile)
    {
        this->m_pCurrentSpaceship = new TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if(serializer->OpenRead(a_pFile))
        {
            this->m_pCurrentSpaceship->Deserialize(*serializer, *sba_BrickManager, *sba_World);
            serializer->Close();
            std::string tmp = std::string(a_pFile);
            this->m_pCurrentSpaceship->SetNameFromFilename(tmp.substr(tmp.find_last_of("/") + 1));
        }
        else
        {
            delete serializer;
            this->ResetShip();
            this->SaveShipToFile(a_pFile);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CShipWorker::SaveShipToFile(const char* a_pFile)
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        if(serializer->OpenWrite(a_pFile))
        {
            this->m_pCurrentSpaceship->Serialize(*serializer);
        }
        serializer->Close();
    }

    // **************************************************************************
    // **************************************************************************
    void CShipWorker::ResetShip()
    {
        SAFE_DELETE(this->m_pCurrentSpaceship);
        this->m_pCurrentSpaceship = new TheBrick::CSpaceship(*sba_World, ong::vec3(0, 0, 0));
        TheBrick::CBrickInstance* brickInstance = sba_BrickManager->GetBrick(1).CreateInstance(*this->m_pCurrentSpaceship, *sba_World);
        brickInstance->SetTransform(ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
        brickInstance->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, 0.0f, 0.0f));
        brickInstance->m_Color = PuRe_Color(0, 0, 1);
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrickInstance* CShipWorker::AddBrickInstanceToShip(const TheBrick::CBrickInstance& a_pTemplate)
    {
        TheBrick::CBrickInstance* brickInstance = a_pTemplate.m_pBrick->CreateInstance(*this->m_pCurrentSpaceship, *sba_World);
        brickInstance->SetTransform(a_pTemplate.GetTransform());
        brickInstance->m_Color = a_pTemplate.m_Color;
        return brickInstance;
    }
}