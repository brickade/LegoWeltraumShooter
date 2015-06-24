#ifndef _GAME_NETWORK_
#define _GAME_NETWORK_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include "Space.h"

namespace sba
{

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

    typedef void(*pInput)(std::vector<CBullet*>&, SPlayer*, sba::SInputData*,float);
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
        sba::SInputPacket m_inputs[sba::Delay];
    public:
        bool m_Run;
    public:
        CGameNetwork();
        ~CGameNetwork();
    public:
        void Initialize();
        void UpdateNetwork(std::vector<CBullet*>& a_rBullets,PuRe_IInput* a_pInput, float a_DeltaTime, float& a_rEndTime, hInput hInput, pInput pInput);
        void Receive(SOCKET s);
        void Update(float a_DeltaTime);
    };
}
#endif /* _GAME_NETWORK_ */