#include "test.h"
#include "restitutionTest.h"
#include "HierarchieTest.h"
#include "StressTest.h"
#include "RayMarcherTest.h"
#include "FilterTest.h"
#include "ServerTest.h"
#include "ClientTest.h"
#include "ShapeTest.h"
#include "DestructionTest.h"
#include "myMath.h"

#undef main

int main()
{

	Test* test = new Test();
	test->init();

	test->run();


	return 0;
}