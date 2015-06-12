#ifndef _EDITORCOLORFIELDS_H_
#define _EDITORCOLORFIELDS_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace sba
{
    class CNavigation;
}

namespace TheBrick
{
    class CGameObject;
    class CBrickInstance;
    class CSpaceship;
}

namespace Editor
{
    class CColorFields
    {

    public:

    private:
        int m_PlayerIdx;

        std::vector<PuRe_Color> m_Colors;
        sba::CNavigation* m_pNavigation;

        PuRe_Quad* m_pQuad;

        float m_ElementSize = 30;
        PuRe_Vector2F m_ListStart = PuRe_Vector2F(100, 750);
        PuRe_Vector2F m_ListStep = PuRe_Vector2F(80, 80);

    public:
        CColorFields(int a_PlayerIdx);
        ~CColorFields();

        void Initialize(PuRe_IGraphics& a_pGraphics);
        void Update(PuRe_IGraphics& a_pGraphics, PuRe_IWindow& a_pWindow, PuRe_Timer& a_pTimer, PuRe_SoundPlayer& a_pSoundPlayer);
        void Render(PuRe_IGraphics& a_pGraphics);

        PuRe_Color& GetCurrentColor();

    private:
        void Add(const PuRe_Color color);
        void Add(float r, float g, float b);
        void Remove(const PuRe_Color color);
    };
}

#endif /* _EDITORCOLORFIELDS_H_ */