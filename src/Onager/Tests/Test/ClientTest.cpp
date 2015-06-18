#include "ClientTest.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>

void ClientTest::init()
{
	
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	addrinfo* result;
	getaddrinfo("127.0.0.1", "1234", &hints, &result);
	
	connect(m_socket, result->ai_addr, result->ai_addrlen);

	Package package;
	while (recv(m_socket, (char*)&package, sizeof(Package), 0))
	{
		if (package.type == Package::START)
			break;
	}

	m_numPlayers = package.numPlayers;
	memset(m_buffer, -1, sizeof(Package) * 31);

	std::thread thread(&ClientTest::receiveFromServer, this);
	thread.detach();
		
	start();

	//send init
	{
		Package package;
		package.type = Package::TICK;
		for (int i = 0; i < 6; ++i)
		{

			package.frame = i;
			package.movement[0] = 0;
			send(m_socket, (char*)&package, sizeof(Package), 0);
		}
	}

	m_stepping = false;
}

bool ClientTest::inputExists()
{
	return m_buffer[0].frame == m_frame;
}


void ClientTest::receiveFromServer()
{
	Package package;
	while (recv(m_socket, (char*)&package, sizeof(Package), 0))
	{
		if (package.type == Package::TICK)
		{
			m_mutex.lock();
			printf("received tick %d\n", package.frame);
			m_buffer[package.frame - m_frame] = package;
			m_mutex.unlock();
		}
		
	}
}

void ClientTest::stepPhysics(float dt)
{
	m_physicsTimer += dt;
	m_mutex.lock();
	while (m_physicsTimer >= 1.0f / 60.0f && inputExists())
	{
		//send
		Package package;
		package.type = Package::TICK;
		package.frame = m_frame + 6;
		package.movement[0] = 0;
		if (SDL_GetKeyboardState(0)[SDL_SCANCODE_RIGHT])
			package.movement[0] |= 0x01;
		if (SDL_GetKeyboardState(0)[SDL_SCANCODE_LEFT])
			package.movement[0] |= 0x02;
		if (SDL_GetKeyboardState(0)[SDL_SCANCODE_UP])
			package.movement[0] |= 0x010;
		if (SDL_GetKeyboardState(0)[SDL_SCANCODE_DOWN])
			package.movement[0] |= 0x020;
		if (SDL_GetKeyboardState(0)[SDL_SCANCODE_PAGEUP])
			package.movement[0] |= 0x04;

		printf("send package %d\n", package.frame);
		send(m_socket, (char*)&package, sizeof(Package), 0);



		for (int i = 0; i < m_numPlayers; ++i)
		{
			addMovement(m_buffer[0].movement[i], i);
		}

		memcpy(m_buffer, m_buffer + 1, sizeof(Package) * 31);
		m_buffer[31].frame = -1;

		m_world->step(1.0f / 60.0f);
		m_physicsTimer -= 1.0f / 60.0f;
		m_frame++;
	}
	m_mutex.unlock();
}


