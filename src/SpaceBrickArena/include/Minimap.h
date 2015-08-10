#ifndef _MINIMAP_H_
#define _MINIMAP_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>


#include "SpriteReader.h"
#include "GameCamera.h"

namespace sba
{
    class CMinimap
    {
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
        /// @brief Draw the Player at Position
        ///
        /// @param Graphic Object
        /// @param Camera to use
        /// @param Material to use
        /// @param Position to draw at
        /// @param Bounding Box
        ///
        void Draw(CGameCamera* a_pCamera, sba::CSpriteReader* a_pUI, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, int a_Player, int a_Target, float a_OriginDistance);
    };
}
#endif /* _MINIMAP_H_ */