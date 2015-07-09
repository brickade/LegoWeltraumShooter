#include "ServerTest.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>




ServerTest::ServerTest()
{
	
}

void ServerTest::listenForClients()
{
	SOCKADDR_IN clientAddress = { 0 };
	int clientAddressSize = sizeof(clientAddress);
	SOCKET connection;
	while ((connection = accept(m_socket, (sockaddr*)&clientAddress, &clientAddressSize)) != INVALID_SOCKET)
	{
		m_clients[m_numPlayers] = connection;
		std::thread thread(&ServerTest::receiveFromClients, this, m_numPlayers);
		thread.detach();

		m_numPlayers++;
	}
}

void ServerTest::receiveFromClients(int i)
{
	Package package;
	while (recv(m_clients[i], (char*)&package, sizeof(Package), 0) > 0)
	{
		if (package.type == Package::TICK)
		{
			m_mutex.lock();
			m_buffer[package.frame - m_frame].movement[i] = package.movement[0];
			m_numReceived[package.frame - m_frame]++;
			printf("received tick %d from player %d\n", package.frame, i);
			m_mutex.unlock();
		}

	}
}


void ServerTest::init()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	addrinfo hints;
	ZeroMemory(&hints, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// get address
	addrinfo* result;
	getaddrinfo(NULL, "1234", &hints, &result);
	m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	
	bind(m_socket, result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);

	
	if (listen(m_socket, 10) != -1)
	{
		std::thread listenThread(&ServerTest::listenForClients, this);
		listenThread.detach();
	}

	char buffer[1024];
	while (0 != strcmp(gets(buffer), "start"));
	closesocket(m_socket);

	start();

	for (int i = 0; i < 32; ++i)
	{
		m_buffer[i].type = Package::TICK;
		m_buffer[i].frame = i;
		memset(m_buffer[i].movement, 0, sizeof(int) * 32);
		m_send[i] = 0;
		m_numReceived[i] = 0;
	}

	Package startPackage;
	startPackage.type = Package::START;
	startPackage.numPlayers = m_numPlayers;
	for (int i = 0; i < m_numPlayers; ++i)
	{
		send(m_clients[i], (char*)&startPackage, sizeof(Package), 0);
	}

	m_stepping = false;
}


bool ServerTest::inputExists()
{
	assert(m_buffer[0].frame == m_frame);
	return (m_send[0] == 1);
}

void ServerTest::update(float dt)
{
	m_mutex.lock();
	for (int i = 0; i < 32; ++i)
	{
		if (!m_send[i] && m_numReceived[i] >= m_numPlayers)
		{
			printf("send tick %d\n", m_buffer[i].frame);
			for (int j = 0; j < m_numPlayers; ++j)
			{
				send(m_clients[j], (char*)(m_buffer + i), sizeof(Package), 0);
			}
			m_send[i] = 1;
		}
	}
	m_mutex.unlock();
}

void ServerTest::stepPhysics(float dt)
{
	m_physicsTimer += dt;
	m_mutex.lock();
	while (m_physicsTimer >= 1.0f / 60.0f && inputExists())
	{
		
		for (int i = 0; i < m_numPlayers; ++i)
		{
			addMovement(m_buffer[0].movement[i], i);
		}

		memcpy(m_buffer, m_buffer + 1, sizeof(Package) * 31);
		memcpy(m_numReceived, m_numReceived + 1, sizeof(int) * 31);
		memcpy(m_send, m_send + 1, sizeof(int) * 31);
		m_numReceived[31] = 0;
		m_send[31] = 0;
		m_buffer[31].frame = m_frame + 32;

		m_world->step(1.0f / 60.0f);
		m_physicsTimer -= 1.0f / 60.0f;
		m_frame++;
	}
	m_mutex.unlock();
}