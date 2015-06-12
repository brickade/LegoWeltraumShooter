#ifndef _EDITORBRICKSUPERVISOR_H_
#define _EDITORBRICKSUPERVISOR_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <cassert>

#include "Editor_BrickCategory.h"

namespace Editor
{
    class CBrickSupervisor
    {
    public:
        int m_PlayerIdx;
        std::array<CBrickCategory*, 10> m_Categories;
        int m_CategoriesCount;
        CBrickCategory* m_pActiveCategory;
        sba::CNavigation* m_pNavigation;

        bool m_NavigateTabs = false;
        float m_RotationSpeed = 1;
        float m_TabRotation = 0.4f;

    public:
        CBrickSupervisor(int a_PlayerIdx);
        ~CBrickSupervisor();

        void Initialize(PuRe_IGraphics& a_pGraphics);

        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer);
        void Render(PuRe_IGraphics& a_pGraphics, float a_Visibility);

        TheBrick::CBrick* GetSelectedBrick() const;
    };
}

#endif /* _EDITORBRICKSUPERVISOR_H_ */