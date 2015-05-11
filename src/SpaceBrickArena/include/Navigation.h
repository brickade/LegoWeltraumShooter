#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace Game
{
    struct EDirection
    {
        enum Type
        {
            None,
            Up,
            Down,
            Left,
            Right,
        };
    };

    class CNavigation
    {
        

    private:
        int m_playerIdx;

    public:
        CNavigation();
        ~CNavigation();

    };
}

#endif /* _NAVIGATION_H_ */