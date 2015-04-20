#ifndef _BRICKWORKER_H_
#define _BRICKWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace Game
{
    class CBrickWorker
    {
    public:

        CBrickWorker(int32 a_playerIdx);
        ~CBrickWorker();

        void Initialize();
        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, PuRe_Vector3F a_cameraLook);
        void Render(PuRe_IGraphics* a_pGraphics);

    private:
        int32 m_playerIdx;
        float32 m_gamepadThreshold;
        PuRe_Model* m_pCurrentBrick;
    };
}

#endif /* _BRICKWORKER_H_ */