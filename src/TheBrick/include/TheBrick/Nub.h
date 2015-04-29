#ifndef _NUB_H_
#define _NUB_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace TheBrick
{
    struct SNub
    {
        PuRe_Vector3F Position = PuRe_Vector3F::Zero();
        PuRe_Vector3F Orientation = PuRe_Vector3F::UnitY();
        bool isMale = true;
    };
}

#endif /* _NUB_H_ */