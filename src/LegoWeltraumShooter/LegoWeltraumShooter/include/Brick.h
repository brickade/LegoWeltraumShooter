#ifndef _BRICK_H_
#define _BRICK_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

namespace Game
{
    class CBrick
    {
    public:
        uint32 m_BrickId;
    private:
        PuRe_Model* m_pModel;

    public:
        CBrick();
        ~CBrick();

    };
}

#endif /* _BRICK_H_ */