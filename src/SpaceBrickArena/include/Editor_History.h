#ifndef _EDITORHISTORY_H_
#define _EDITORHISTORY_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/myMath.h>

#include <TheBrick/BrickInstance.h>

namespace Editor
{
    struct SHistoryStep
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
        SHistoryStep* m_Storage;
        int m_CurrentPos;
        int m_RedoEndPos;
        int m_StorageEnd;
        int m_PerformanceBuffer;

    public:
        CHistory(int a_size, int a_performanceBuffer);
        ~CHistory();

        void AddStep(SHistoryStep& step);
        void CutRedos();
        void Clear();
        SHistoryStep* Undo();
        SHistoryStep* Redo();
    };
}

#endif /* _EDITORHISTORY_H_ */