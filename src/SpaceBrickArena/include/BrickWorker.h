#ifndef _BRICKWORKER_H_
#define _BRICKWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <TheBrick/TheBrick.h>
#include <Onager/World.h>

#include "BrickBozz.h"
#include "EditorCamera.h"

namespace Game
{
    class CBrickWorker
    {
    public:

    private:
        CEditorCamera* m_pCamera;

        float m_currentBrickRotation;
        float m_currentRotation;
        int m_maxBrickDistance;
        PuRe_Vector2F m_currentBrickPosition;
        PuRe_Vector2F m_currentPosition;
        float m_currentHeight;

        int m_playerIdx;

        TB_Brick* m_pCurrentBrick;

        PuRe_IMaterial* m_pGridMaterial;
        PuRe_Model* m_pGridBrick;

        PuRe_List<TB_BrickInstance*>* m_pSpaceship;

        bool lastInputIsGamepad;

    public:
        CEditorCamera* GetCamera() const
        {
            return this->m_pCamera;
        }

    public:
        CBrickWorker(int a_playerIdx);
        ~CBrickWorker();

        void Initialize(PuRe_IGraphics* a_pGraphics);
        void Update(PuRe_IGraphics* a_pGraphics, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, PuRe_Timer* a_pTimer, PuRe_SoundPlayer* a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick);
        void Render(PuRe_IGraphics* a_pGraphics);

    private:
        void UpdateTranslation(PuRe_IInput* a_pInput, PuRe_Vector3F a_cameraLook, float a_speed);
        void UpdateRotation(PuRe_IInput* a_pInput, float a_rotationPerClick);
        void UpdatePlacement(PuRe_IInput* a_pInput, ong::World* a_pWorld);
    };
}

#endif /* _BRICKWORKER_H_ */