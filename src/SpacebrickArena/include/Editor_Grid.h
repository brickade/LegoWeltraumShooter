#ifndef _EDITORGRID_H_
#define _EDITORGRID_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace Editor
{
    class CGrid
    {
    private:
        PuRe_Quad* m_pGridQuad;
        PuRe_ITexture2D* m_GridTextureTop;
        PuRe_ITexture2D* m_GridTextureBottom;

        PuRe_IMaterial* m_pGridMaterial;
        PuRe_RenderInstance* m_pGridRenderInstancesTop;
        PuRe_RenderInstance* m_pGridRenderInstancesBottom;
        int m_GridRenderInstanceCount;

        PuRe_Sprite* m_pArrow;
        float m_ArrowSize = 0.01f;

    public:
        CGrid(PuRe_IGraphics& a_rGraphics);
        ~CGrid();

        void Draw(bool a_DrawArrow);
    };
}

#endif /* _EDITORGRID_H_ */