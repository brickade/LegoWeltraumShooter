#pragma once

#include "test.h"



class NetworkTest : public Test
{
public:

protected:
	struct Package
	{
		enum TYPE
		{
			START,
			TICK,
		} type;

		int frame;

		union
		{
			int movement[32];
			int numPlayers;
		};
	};

	NetworkTest()
		: m_frame(0)
	{

	}
	void start();
	void addMovement(int movement, int idx);

	int m_numPlayers;
	int m_frame;
	Entity** m_players;
private:

};
