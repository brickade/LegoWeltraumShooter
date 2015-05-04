#ifndef _NUB_H_
#define _NUB_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace TheBrick
{
    struct SNub
    {
        ong::Transform Transform = ong::Transform(ong::vec3(0,0,0), ong::QuatFromEulerAngles(0, 0, 0));
        bool isMale = true;
    };
}

#endif /* _NUB_H_ */