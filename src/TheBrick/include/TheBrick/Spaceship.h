#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "GameObject.h"


namespace TheBrick
{
    class CSpaceship : public CGameObject
    {
    public:

    private:
        

    public:
        CSpaceship();
        ~CSpaceship();

        void Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera) override;
    };
}

#endif /* _SPACESHIP_H_ */