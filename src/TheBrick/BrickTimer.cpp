#include "include/TheBrick/BrickTimer.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    void CBrickTimer::Reset()
    {
        this->m_StartTime = Time_Now;
    }
}