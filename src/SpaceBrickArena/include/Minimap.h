#ifndef _MINIMAP_H_
#define _MINIMAP_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace sba
{
    class CMinimap
    {
    private:
        /// @brief Minimap's Position
        ///
        PuRe_Vector3F m_Position;
        /// @brief Box for 3D Minimap
        ///
        PuRe_IVertexBuffer* m_pBoxVertexBuffer;
    public:
        /// @brief Constructor to create a minimap
        ///
        /// @param graphics object to create sprites
        ///
        CMinimap(PuRe_IGraphics* a_pGraphics);
        /// @brief Destructor to destroy a minimap
        ///
        ~CMinimap();
    public:
        /// @brief Draw the Map at Position
        ///
        /// @param Graphic Object
        /// @param Camera to use
        /// @param Material to use
        /// @param Position to draw at
        ///
        void Draw(PuRe_Renderer* a_pRenderer, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, PuRe_MatrixF a_CamRotation);
        /// @brief Draw the Player at Position (Screenposition)
        ///
        /// @param Graphic Object
        /// @param Camera to use
        /// @param Material to use
        /// @param Position to draw at
        /// @param Bounding Box
        ///
        void DrawPlayer(PuRe_Renderer* a_pRenderer, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, const PuRe_BoundingBox &a_Boundaries, PuRe_MatrixF a_CamRotation);
    };
}
#endif /* _MINIMAP_H_ */