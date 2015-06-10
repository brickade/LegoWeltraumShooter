#pragma once

#include "NetworkTest.h"
#include "defines.h"
#include <vector>
#include <queue>
#include <mutex>
typedef size_t SOCKET;


struct Server;
class ClientTest : public NetworkTest
{
public:
	void init() override;
	void stepPhysics(float physicsTimer) override;
	//void update(float dt) override;
private:
	SOCKET m_socket;
	std::mutex m_mutex;
	Package m_buffer[32];
	
	void receiveFromServer();
	bool inputExists();
};

