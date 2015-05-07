#ifndef _BRICKTIMER_H_
#define _BRICKTIMER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace TheBrick
{
    class CBrickTimer : public PuRe_Timer
    {

    public:
        int m_Id;


    public:
        CBrickTimer();
        ~CBrickTimer();

        void Reset();
    };
}

#endif /* _BRICKTIMER_H_ */