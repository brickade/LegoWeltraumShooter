#include "test.h"
#include "restitutionTest.h"
#include "HierarchieTest.h"

#undef main

int main()
{

	Test* test = new Test();
	test->init();

	test->run();


	return 0;
}