#include "include/TheBrick/Spaceship.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CSpaceship::CSpaceship()
    {

    }

    // **************************************************************************
    // **************************************************************************
    CSpaceship::~CSpaceship()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {

    }

    // **************************************************************************
    // **************************************************************************
    static CSpaceship* Deserialize(CSerializer* a_pSerializer, CBrickManager* a_pBrickManager)
    {
        CSpaceship* ship = new CSpaceship();
        //GameObject
        CGameObject* gameobject = CGameObject::Deserialize(a_pSerializer, a_pBrickManager);
        memcpy(ship, gameobject, sizeof(gameobject)); //Does this work 1:1?
        SAFE_DELETE(gameobject);

        return ship;
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Serialize(CSerializer* a_pSerializer)
    {
        //GameObject
        reinterpret_cast<CGameObject*>(this)->Serialize(a_pSerializer);

    }
}