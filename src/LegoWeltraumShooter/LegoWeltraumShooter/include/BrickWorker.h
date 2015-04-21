#ifndef _BRICKWORKER_H_
#define _BRICKWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace Game
{
    #define BRICK_WIDTH 0.39f
    #define BRICK_HEIGHT 1.0f
    #define PLATE_HEIGHT 0.1f
    class CBrickWorker
    {
    public:

        CBrickWorker(int32 a_playerIdx);
        ~CBrickWorker();

        void Initialize(PuRe_IGraphics* a_pGraphics);
        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, PuRe_Vector3F a_cameraLook);
        void Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

    private:
        int32 m_maxBrickDistance;
        PuRe_Vector2F m_currentBrickPosition;
        PuRe_Vector2F m_currentPosition;
        float32 m_currentHeight;

        int32 m_playerIdx;

        PuRe_Model* m_pCurrentBrick;

        PuRe_IMaterial* m_pGridMaterial;
        PuRe_Model* m_pGridBrick;
        PuRe_IMaterial* m_pTmpBrickMat;

        bool lastInputIsGamepad;
    };
}

#endif /* _BRICKWORKER_H_ */