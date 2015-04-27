#ifndef _BRICKWORKER_H_
#define _BRICKWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Brick.h"
#include "BrickInstance.h"

namespace Game
{
    class CBrickWorker
    {
    public:

    private:
        float32 m_currentBrickRotation;
        float32 m_currentRotation;
        int32 m_maxBrickDistance;
        PuRe_Vector2F m_currentBrickPosition;
        PuRe_Vector2F m_currentPosition;
        float32 m_currentHeight;

        int32 m_playerIdx;

        CBrick* m_pCurrentBrick;

        PuRe_IMaterial* m_pGridMaterial;
        PuRe_Model* m_pGridBrick;
        PuRe_IMaterial* m_pTmpBrickMat;

        PuRe_List<CBrickInstance*>* m_pSpaceship;

        bool lastInputIsGamepad;

    public:
        CBrickWorker(int32 a_playerIdx);
        ~CBrickWorker();

        void Initialize(PuRe_IGraphics* a_pGraphics);
        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, PuRe_Vector3F a_cameraLook);
        void Render(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera);

    private:
        void UpdateTranslation(PuRe_IInput* a_pInput, PuRe_Vector3F a_cameraLook, float32 a_speed);
        void UpdateRotation(PuRe_IInput* a_pInput, float32 a_rotationPerClick);
        void UpdatePlacement(PuRe_IInput* a_pInput);
    };
}

#endif /* _BRICKWORKER_H_ */