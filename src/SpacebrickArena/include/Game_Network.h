#ifndef _GAME_NETWORK_
#define _GAME_NETWORK_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Space.h"
#include "NetworkHandler.h"
#include "Item.h"
#include "Asteroid.h"
#include "Bullet.h"
#include "Player.h"

namespace sba
{

    struct ExplosionEmitter
    {
        PuRe_ParticleEmitter* pEmitter;
        float LifeTime;
        int Bullet;
    };
    enum { BufferSize = 32 };
    struct PlayOutBuffer
    {
        int Frame;
        sba::SInputData Inputs[sba::MaxPlayers]; //frame saved in here
    };
    struct GotBuffer
    {
        bool Player[sba::MaxPlayers];
    };

    //typedef std::function<void(std::vector<CBullet*>&, SPlayer*, sba::SInputData*, float)> pInput;
    //typedef std::function<SInputData(int)> hInput;

    typedef void(*gUpdate)(std::vector<SItemData>&,std::vector<CAsteroid*>&, std::vector<ExplosionEmitter>&, std::vector<CBullet*>&, std::vector<CItem*>&, float, int);
    typedef void(*pInput)(std::vector<CBullet*>&, SPlayer*, sba::SInputData*, float, int);
    typedef SInputData(*hInput)(int, PuRe_IInput*);

    class CGameNetwork
    {
    private:
        int m_PhysicFrame;
        float m_PhysicTime;
        //when sb is going to be kicked
        float m_Timeout;
        //host only
        unsigned int m_numReceived[BufferSize];
        GotBuffer m_numGot[BufferSize];
        bool m_send[BufferSize];
        //All players
        PlayOutBuffer m_buffer[BufferSize];
    public:
        bool m_Run;
    public:
        CGameNetwork();
        ~CGameNetwork();
    public:
        void Initialize();
        bool UpdateNetwork(std::vector<SItemData>& a_rRespawnItems, std::vector<CAsteroid*>& a_rAsteroids, std::vector<ExplosionEmitter>& a_rExplosions, std::vector<CBullet*>& a_rBullets, std::vector<CItem*>& a_rItems, PuRe_IInput* a_pInput, float a_DeltaTime, float& a_rEndTime, hInput hInput, pInput pInput, gUpdate gUpdate);
        void Receive(SOCKET s);
        void Update(float a_DeltaTime);
    };
}
#endif /* _GAME_NETWORK_ */