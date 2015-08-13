#ifndef _EDITORHISTORY_H_
#define _EDITORHISTORY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/myMath.h>

#include <TheBrick/BrickInstance.h>

namespace sba
{
    class CSpaceship;
}

namespace Editor
{
    struct SHistoryBrick
    {
        TheBrick::CBrickInstance* BrickInstance;
        TheBrick::CBrick* Brick;
        ong::Transform Transform;
        PuRe_Color Color;
    };
    struct SHistoryStep
    {
        SHistoryBrick Brick;
        bool Delete;
        SHistoryStep* DeleteBrick_Step;
        std::vector<SHistoryStep*> DeleteAdhesiveBricks_Steps;
    };

    class CHistory
    {
    public:

    private:
        SHistoryStep* m_Storage;
        int m_CurrentPos;
        int m_RedoEndPos;
        int m_StorageEnd;
        int m_PerformanceBuffer;

    public:
        CHistory(int a_size, int a_performanceBuffer);
        ~CHistory();

        void AddStep(SHistoryStep& step);
        void AddStep(TheBrick::CBrickInstance* a_pBrickInstance, std::vector<TheBrick::CBrickInstance*>* a_AdhesiveBricks = nullptr, bool a_Delete = false);
        void CutRedos();
        void Clear();
        SHistoryStep* Undo();
        SHistoryStep* Redo();

        static void RecreateBrick(SHistoryStep* a_HistoryStep, sba::CSpaceship& a_rSpaceship, ong::World& a_rWorld);
    };
}

#endif /* _EDITORHISTORY_H_ */