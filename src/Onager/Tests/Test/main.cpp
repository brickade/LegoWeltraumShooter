#include "test.h"
#include "restitutionTest.h"
#include "HierarchieTest.h"
#include "BrickTest.h"
#include "myMath.h"

#undef main

int main()
{

	Test* test = new BrickTest();
	test->init();

	test->run();


	return 0;
}