#ifndef _EDITORWORKER_H_
#define _EDITORWORKER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "TheBrick/GameObject.h"

#include "Spaceship.h"

#include "Editor_Camera.h"

#include "TheBrick/Brick.h"
#include "SpriteReader.h"

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
    class CGrid;

    class CWorker
    {
        struct BrickToDeleteCache
        {
            TheBrick::CBrickInstance* BrickInstance = nullptr;
            PuRe_Color Color;
        };
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

        bool m_DeleteMode = false;

        TheBrick::CGameObject* m_pCurrentBrickObject;
        TheBrick::CBrickInstance* m_pCurrentBrick = nullptr;

        BrickToDeleteCache m_BrickToDelete;
        std::vector<BrickToDeleteCache> m_AdhesiveBricksToDelete;
        CCamera* m_pCamera;
        CHistory* m_pHistory;

        double m_UndoRedoScrollTimer = 0;
        float m_ScrollingStart = 0.6f;
        float m_ScrollingStep = 0.05f;

        CGrid* m_pGrid;

    public:
        CCamera* GetCamera() const
        {
            return this->m_pCamera;
        }

    public:
        CWorker(int a_playerIdx);
        ~CWorker();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer, TheBrick::CBrick* a_pCurrentBrick, PuRe_Color& a_rCurrentColor, CShipHandler& a_rShipHandler, bool a_Delete);
        void Render(CShipHandler& a_rShipHandler, sba::CSpriteReader& a_rSpriteReader);
        void Suspend();
        void Resume(CShipHandler& a_rShipHandler);

    private:
        void UpdateTranslation(PuRe_Vector3F a_cameraLook, float a_speed);
        void UpdateRotation();
        void UpdateHeight(CShipHandler& a_rShipHandler);
        void UpdateDeleteHeight(CShipHandler& a_rShipHandler);
        static void TraverseBricks(sba::SBrickDestruction& a_rStartBrickDestruction);
        void ApplyToCurrentBrick();
        void UpdatePlacement(CShipHandler& a_rShipHandler);
        void UpdateDelete(CShipHandler& a_rShipHandler);
        void UpdateMiscellaneous(CShipHandler& a_rShipHandler, TheBrick::CBrick* a_pCurrentBrick);

        void RestoreAdhesiveBricksToDelete();
        
    };
}

#endif /* _EDITORWORKER_H_ */