#include "test.h"
#include "restitutionTest.h"
#include "HierarchieTest.h"
#include "myMath.h"

#undef main

int main()
{

	Test* test = new HierarchieTest();
	test->init();

	test->run();


	return 0;
}