#include "include/Editor_History.h"

#include "include/Spaceship.h"

namespace Editor
{
    // **************************************************************************
    // **************************************************************************
    CHistory::CHistory(int a_size, int a_performanceBuffer)
    {
        this->m_PerformanceBuffer = a_performanceBuffer;
        this->m_StorageEnd = a_size + a_performanceBuffer;
        this->m_Storage = new SHistoryStep[this->m_StorageEnd - 1];
        //memset(this->m_Storage, 0, sizeof(SHistoryStep) * (this->m_StorageEnd - 1));
        this->m_CurrentPos = -1;
        this->m_RedoEndPos = -1;
    }


    // **************************************************************************
    // **************************************************************************
    CHistory::~CHistory()
    {
        SAFE_DELETE_ARRAY(this->m_Storage);
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::AddStep(SHistoryStep& step)
    {
        //Add to History
        this->m_CurrentPos++;
        this->m_RedoEndPos++;
        if (this->m_CurrentPos == this->m_StorageEnd - this->m_PerformanceBuffer)
        {
            for (int i = 0; i <= this->m_StorageEnd - this->m_PerformanceBuffer; i++)
            {
                this->m_Storage[i] = this->m_Storage[i + this->m_PerformanceBuffer];
            }
            this->m_CurrentPos -= this->m_PerformanceBuffer;
            this->m_RedoEndPos -= this->m_PerformanceBuffer;
        }
        this->m_Storage[this->m_CurrentPos] = step;
        if (!step.Delete)
        {
            step.Brick.BrickInstance->m_UserData = &this->m_Storage[this->m_CurrentPos];
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::AddStep(TheBrick::CBrickInstance* a_pBrickInstance, std::vector<TheBrick::CBrickInstance*>* a_AdhesiveBricks, bool a_Delete)
    {
        SHistoryStep step;
        memset(&step, 0, sizeof(SHistoryStep) - sizeof(std::vector<SHistoryStep*>));
        //step.DeleteAdhesiveBricks_Steps = std::vector<SHistoryStep*>();
        if (!(step.Delete = a_Delete))
        {
            SHistoryBrick brick;
            brick.BrickInstance = a_pBrickInstance;
            brick.Brick = a_pBrickInstance->m_pBrick;
            brick.Transform = a_pBrickInstance->GetTransform();
            brick.Color = a_pBrickInstance->m_Color;
            step.Brick = brick;
        }
        else
        {
            step.DeleteBrick_Step = reinterpret_cast<SHistoryStep*>(a_pBrickInstance->m_UserData);
            if (a_AdhesiveBricks != nullptr)
            {
                step.DeleteAdhesiveBricks_Steps.reserve(a_AdhesiveBricks->size());
                for (size_t i = 0; i < a_AdhesiveBricks->size(); i++)
                {
                    step.DeleteAdhesiveBricks_Steps[i] = reinterpret_cast<SHistoryStep*>((*a_AdhesiveBricks)[i]->m_UserData);
                }
            }
        }
        this->AddStep(step); //Add History step
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::CutRedos()
    {
        this->m_RedoEndPos = this->m_CurrentPos;
    }

    // **************************************************************************
    // **************************************************************************
    void CHistory::Clear()
    {
        this->m_Storage = new SHistoryStep[this->m_StorageEnd - 1];
        memset(this->m_Storage, 0, sizeof(SHistoryStep) * (this->m_StorageEnd - 1));
        this->m_CurrentPos = -1;
        this->m_RedoEndPos = -1;
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
        return &this->m_Storage[this->m_CurrentPos+1];
    }

    // **************************************************************************
    // **************************************************************************
    SHistoryStep* CHistory::Redo()
    {
        this->m_CurrentPos++;
        if (this->m_CurrentPos > this->m_RedoEndPos)
        {
            this->m_CurrentPos = this->m_RedoEndPos;
            return nullptr;
        }
        return &this->m_Storage[this->m_CurrentPos];
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