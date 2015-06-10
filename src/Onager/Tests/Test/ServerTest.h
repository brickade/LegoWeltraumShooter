#pragma once

#include "NetworkTest.h"
#include "defines.h"
#include <vector>
#include <queue>
#include <mutex>
typedef size_t SOCKET;


struct Server;
class ServerTest : public NetworkTest
{
public:
	ServerTest();
	void init() override;
	void stepPhysics(float physicsTimer) override;
	void update(float dt) override;
private:
	SOCKET m_socket;
	SOCKET m_clients[32];
	
	std::mutex m_mutex;

	Package m_buffer[32];
	int m_numReceived[32];
	int m_send[32];

	void listenForClients();
	void receiveFromClients(int i);
	bool inputExists();
};

