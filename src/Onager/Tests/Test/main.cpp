#include "test.h"
#include "restitutionTest.h"
#include "HierarchieTest.h"
#include "myMath.h"

#undef main

int main()
{

	Quaternion q = QuatFromEulerAngles(0.1, 0.2, 0.3);
	vec3 euler = rotMatToEulerAngles(toRotMat(q));
	
	Test* test = new Test();
	test->init();

	test->run();


	return 0;
}