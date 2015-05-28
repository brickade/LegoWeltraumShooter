#ifndef _HISTORY_H_
#define _HISTORY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/myMath.h>

#include <TheBrick/BrickInstance.h>

namespace Game
{
    struct HistoryStep
    {
        TheBrick::CBrickInstance* BrickInstance;
        TheBrick::CBrick* Brick;
        ong::Transform Transform;
        PuRe_Color Color;
    };

    class CHistory
    {
    public:

    private:
        HistoryStep* m_Storage;
        int m_CurrentPos;
        int m_RedoEndPos;
        int m_StorageEnd;
        int m_PerformanceBuffer;

    public:
        CHistory(int a_size, int a_performanceBuffer);
        ~CHistory();

        void AddStep(HistoryStep& step);
        void CutRedos();
        HistoryStep* Undo();
        HistoryStep* Redo();
    };
}

#endif /* _HISTORY_H_ */