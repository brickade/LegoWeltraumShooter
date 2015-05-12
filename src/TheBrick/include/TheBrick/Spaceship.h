#ifndef _SPACESHIP_H_
#define _SPACESHIP_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "GameObject.h"

#include "DebugDraw.h"

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
        void Update(float a_DeltaTime) override;

        void HandleInput(PuRe_Camera* a_pCamera, PuRe_IInput* a_pInput, float a_DeltaTime);

        void Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager, ong::World* a_pWorld) override;
        void Serialize(CSerializer* a_pSerializer);
    };
}

#endif /* _SPACESHIP_H_ */