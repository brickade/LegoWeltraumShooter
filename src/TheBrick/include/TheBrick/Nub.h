#ifndef _NUB_H_
#define _NUB_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace TheBrick
{
    struct SNub
    {
        PuRe_Vector3F Position = PuRe_Vector3F(0,0,0);
        PuRe_Vector3F Direction = PuRe_Vector3F(0, 1, 0);
        bool isMale = true;
    };
}

#endif /* _NUB_H_ */