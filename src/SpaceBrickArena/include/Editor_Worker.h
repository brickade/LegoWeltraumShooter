#ifndef _EDITORWORKER_H_
#define _EDITORWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "TheBrick/GameObject.h"

#include "Spaceship.h"

#include "Editor_Camera.h"

#include "TheBrick/Brick.h"

namespace TheBrick
{
    class CGameObject;
    class CBrickInstance;
}

namespace Editor
{
    class CEditorScene;
    class CHistory;
    class CShipHandler;

    class CWorker
    {
    public:

    private:
        int m_PlayerIdx;
        float m_GamepadMovementSpeedupTimer = 0;

        PuRe_Vector2I m_CurrentPosition;
        PuRe_Vector2F m_CurrentPositionCache;
        int m_CurrentRotation;
        int m_CurrentHeight;
        bool m_CurrentHeightIsInvalid;
        PuRe_Color m_CurrentColor;
        bool m_PlaceBelow;
        bool m_CanPlaceHere = false; 

        TheBrick::CGameObject* m_pCurrentBrickObject;
        TheBrick::CBrickInstance* m_pCurrentBrick;
        
        CCamera* m_pCamera;
        CHistory* m_pHistory;

        /*PuRe_IMaterial* m_pGridMaterial;
        PuRe_Model* m_pGridBrick;*/

    public:
        CCamera* GetCamera() const
        {
            return this->m_pCamera;
        }

    public:
        CWorker(int a_playerIdx);
        ~CWorker();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick, PuRe_Color& a_rCurrentColor, CShipHandler& a_rShipHandler);
        void Render(CShipHandler& a_rShipHandler);
        void Suspend();

    private:
        void UpdateTranslation(PuRe_Vector3F a_cameraLook, float a_speed);
        void UpdateRotation();
        void UpdateHeight(CShipHandler& a_rShipHandler);
        void ApplyToCurrentBrick();
        void UpdatePlacement(CShipHandler& a_rShipHandler);
    };
}

#endif /* _EDITORWORKER_H_ */