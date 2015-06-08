#ifndef _EDITORWORKER_H_
#define _EDITORWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "TheBrick/GameObject.h"
#include "TheBrick/Spaceship.h"

#include "Editor_Camera.h"
#include "Editor_ShipWorker.h"

namespace TheBrick
{
    class CGameObject;
    class CBrickInstance;
    class CSpaceship;
    class CBrick;
}

namespace Editor
{
    class CHistory;
    class CShipWorker;

    class CWorker
    {

    public:

    private:
        CCamera* m_pCamera;

        float m_currentBrickRotation;
        int m_maxBrickDistance;
        PuRe_Vector2F m_currentBrickPosition;
        PuRe_Vector2F m_currentPosition;
        float m_currentHeight;
        PuRe_Color m_currentBrickColor;
        bool m_canPlaceHere = false; 
        float m_nubDockThreshold = 0.01f;
        float m_nubDirThreshold = 0.01f;

        bool m_placeBelow;

        CHistory* m_pHistory;

        int m_playerIdx;

        TheBrick::CGameObject* m_pCurrentBrickObject;
        TheBrick::CBrickInstance* m_pCurrentBrick;

        CShipWorker* m_pShipWorker;

        PuRe_IMaterial* m_pGridMaterial;
        PuRe_Model* m_pGridBrick;

    public:
        CCamera* GetCamera() const
        {
            return this->m_pCamera;
        }
        void DrawDebug(PuRe_IGraphics* a_pGraphics)
        {
            m_pCurrentBrickObject->Draw(a_pGraphics, m_pCamera);
            this->m_pShipWorker->GetCurrentSpaceShip()->Draw(a_pGraphics, this->m_pCamera);
        }

    public:
        CWorker(int a_playerIdx);
        ~CWorker();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick, PuRe_Color& a_rCurrentColor);
        void Render();

    private:
        void UpdateTranslation(PuRe_Vector3F a_cameraLook, float a_speed);
        void UpdateRotation(float a_rotationPerClick);
        void UpdateHeight();
        void ApplyToCurrentBrick();
        void UpdatePlacement();
    };
}

#endif /* _EDITORWORKER_H_ */