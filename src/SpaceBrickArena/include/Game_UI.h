#ifndef _GAME_UI_H
#define _GAME_UI_H

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Minimap.h"
#include "Space.h"

namespace sba
{
    class CGUI
    {
    private:
        PuRe_Sprite* m_pCrossHairSprite;
        PuRe_IMaterial* m_pSpriteMaterial;
        CMinimap* m_pMinimap;
    public:
        CGUI(PuRe_IGraphics* a_pGraphics);
        ~CGUI();
    public:
        void DisplayUI(PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, float a_EndTime,int a_WonID);
    };
}
#endif /* _GAME_UI_H */