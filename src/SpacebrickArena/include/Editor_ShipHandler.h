#ifndef _EDITORSHIPHANDLER_H_
#define _EDITORSHIPHANDLER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>
#include <Onager/World.h>

#include "Spaceship.h"
#include "Navigation.h"

#include "include/Space.h"
#include "InputField.h"

namespace sba
{
    class CSpaceship;
}

namespace TheBrick
{
    class CBrickInstance;
}

namespace Editor
{
    class CShipHandler
    {
    public:
        struct ShipHandlerState
        {
            enum Type
            {
                Select,
                Rename,
                Delete
            };
        };

        struct ShipDataCache
        {
            int Weapons = 0;
            int Engines = 0;
            int Cockpits = 0;
            int Total() const
            {
                return Weapons + Engines + Cockpits;
            }
        };

    private:
        int m_playerIdx;

        ShipHandlerState::Type m_State;

        sba::CNavigation* m_pNavigation;
        sba::CSpaceship* m_pCurrentSpaceship;

        ShipDataCache m_CurrentShipDataCache;

        sba::CInputField* m_pInputField;
        std::string m_OldShipName;

        PuRe_Vector2F m_PreviewOffset = PuRe_Vector2F(0.0f, -150.0f);
        PuRe_Vector2F m_TextOffset = PuRe_Vector2F(-100.0f, 450.0f);
        PuRe_Vector2F m_TextStep = PuRe_Vector2F(0.0f, -50.0f);
        float m_OuterPreviewInset = 100;
        float m_OuterPreviewYOffset = 50;
        float m_OuterPreviewSize = 0.5f;


    public:
        sba::CSpaceship* GetCurrentSpaceShip()
        {
            return this->m_pCurrentSpaceship;
        }

        ShipHandlerState::Type GetCurrentState() const
        {
            return this->m_State;
        }

        const ShipDataCache& GetCurrentShipData() const
        {
            return this->m_CurrentShipDataCache;
        }

    public:
        CShipHandler(int a_playerIdx);
        ~CShipHandler();

        void Update();
        void Render(sba::CSpriteReader& a_rSpriteReader);

        void AddShip(const char* a_pName);
        void SaveCurrentShip();
        void ResetCurrentShip();
        void DeleteCurrentShip();

        TheBrick::CBrickInstance* AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate);

        void UpdateCurrentShipPreview();
        void UpdateCurrentShipName();

        void UpdateCurrentShipData();
    };
}

#endif /* _EDITORSHIPHANDLER_H_ */