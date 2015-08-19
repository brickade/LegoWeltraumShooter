#include "include/Editor_History.h"

#include "include/Spaceship.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CHistory::CHistory()
    {
        this->m_CurrentPos = -1;
    }


    // **************************************************************************
    // **************************************************************************
    CHistory::~CHistory()
    {
        while(!this->m_Storage.empty())
        {
            SAFE_DELETE(this->m_Storage.back());
            this->m_Storage.pop_back();
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::AddStep(TheBrick::CBrickInstance* a_pBrickInstance, std::vector<TheBrick::CBrickInstance*>* a_AdhesiveBricks, bool a_Delete)
    {
        SHistoryStep* step = new SHistoryStep();
        step->Brick.BrickInstance = nullptr;
        step->Brick.Brick = nullptr;
        step->Brick.Transform = ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1));
        step->Brick.Color = PuRe_Color(1, 1, 1, 1);
        step->Delete = a_Delete;
        step->DeleteBrick_Step = nullptr;
        step->DeleteAdhesiveBricks_Steps = std::vector<SHistoryStep*>();

        if (!a_Delete)
        {
            step->Brick.BrickInstance = a_pBrickInstance;
            step->Brick.BrickInstance->m_UserData = step;
            step->Brick.Brick = a_pBrickInstance->m_pBrick;
            step->Brick.Transform = a_pBrickInstance->GetTransform();
            step->Brick.Color = a_pBrickInstance->m_Color;
        }
        else
        {
            step->DeleteBrick_Step = reinterpret_cast<SHistoryStep*>(a_pBrickInstance->m_UserData);
            if (a_AdhesiveBricks != nullptr)
            {
                step->DeleteAdhesiveBricks_Steps.reserve(a_AdhesiveBricks->size()); //Allocate at once not each time
                for (size_t i = 0; i < a_AdhesiveBricks->size(); i++)
                {
                    step->DeleteAdhesiveBricks_Steps.push_back(reinterpret_cast<SHistoryStep*>((*a_AdhesiveBricks)[i]->m_UserData));
                }
            }
        }
        //Cut Redos
        while (this->m_Storage.size() > this->m_CurrentPos + 1)
        {
            SAFE_DELETE(this->m_Storage.back());
            this->m_Storage.pop_back();
        }
        //Add to History
        this->m_CurrentPos++;
        this->m_Storage.push_back(step);
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::Clear()
    {
        while (!this->m_Storage.empty())
        {
            SAFE_DELETE(this->m_Storage.back());
            this->m_Storage.pop_back();
        }
        this->m_Storage.clear();
        this->m_CurrentPos = -1;
    }

    // **************************************************************************
    // **************************************************************************
    SHistoryStep* CHistory::Undo()
    {
        this->m_CurrentPos--;
        if (this->m_CurrentPos < -1)
        {
            this->m_CurrentPos = -1;
            return nullptr;
        }
        return this->m_Storage[this->m_CurrentPos+1];
    }

    // **************************************************************************
    // **************************************************************************
    SHistoryStep* CHistory::Redo()
    {
        this->m_CurrentPos++;
        if (this->m_CurrentPos > this->m_Storage.size() - 1)
        {
            this->m_CurrentPos = this->m_Storage.size() - 1;
            return nullptr;
        }
        return this->m_Storage[this->m_CurrentPos];
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::RecreateBrick(SHistoryStep* a_HistoryStep, sba::CSpaceship& a_rSpaceship, ong::World& a_rWorld)
    {
        assert(!a_HistoryStep->Delete);
        SHistoryBrick* brick = &a_HistoryStep->Brick;
        brick->BrickInstance = new TheBrick::CBrickInstance(*brick->Brick, a_rSpaceship, a_rWorld, brick->Color);
        brick->BrickInstance->SetTransform(brick->Transform);
        brick->BrickInstance->m_UserData = a_HistoryStep;
    }


}