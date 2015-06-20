#ifndef _BRICKDATA_H_
#define _BRICKDATA_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <Onager/Collider.h>
#include <Onager/Shapes.h>
#include <Onager/World.h>

namespace TheBrick
{
    /// @brief Data for a single Brick, which is send over Network
    ///
    struct SBrickData
    {
        unsigned int ID;
        ong::Transform Transform;
        PuRe_Color Color;
    };
}
#endif /* _BRICKDATA_H_ */