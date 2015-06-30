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

    private:
        int m_playerIdx;

        sba::CNavigation* m_pNavigation;
        sba::CSpaceship* m_pCurrentSpaceship;

        bool m_RenameShip;
        sba::CInputField* m_pInputField;
        std::string m_OldShipName;

        PuRe_Vector2F m_PreviewOffset = PuRe_Vector2F(0.0f, -50.0f);
        PuRe_Vector2F m_TextOffset = PuRe_Vector2F(-50.0f, 450.0f);
        PuRe_Vector2F m_TextStep = PuRe_Vector2F(0.0f, -50.0f);
        float m_OuterPreviewInset = 100;
        float m_OuterPreviewYOffset = 50;
        float m_OuterPreviewSize = 0.5f;


    public:
        sba::CSpaceship* GetCurrentSpaceShip()
        {
            return this->m_pCurrentSpaceship;
        }
        bool Renaming()
        {
            return this->m_RenameShip;
        }

    public:
        CShipHandler(int a_playerIdx);
        ~CShipHandler();

        void Update();
        void Render();

        void AddShip(const char* a_pName);
        void SaveCurrentShip();
        void ResetCurrentShip();
        void DeleteCurrentShip();

        TheBrick::CBrickInstance* AddBrickInstanceToCurrentShip(const TheBrick::CBrickInstance& a_pTemplate);

        void UpdateCurrentShipPreview();
        void UpdateCurrentShipName();
    };
}

#endif /* _EDITORSHIPHANDLER_H_ */