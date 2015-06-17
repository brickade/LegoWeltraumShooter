#pragma once
#include "test.h"
#include "Draw.h"


using namespace ong;

class ShapeTest : public Test
{
public:
	void init()
	{
		m_world = new World();
		ShapeDescription descr;

		vec3 points[8]=
		{
			{4.97272825f, 3.55330205f, -6.59857512f },
			{5.03817558f, 3.49487448f, -6.64214849f },
			{5.07426167f, 3.42778206f, -6.63117599f },
			{5.01576138f, 3.48291874f, -6.58213043f },
			{11.3647861f, 8.75802231f, -2.98575115f },
			{12.0192566f, 8.17374420f, -3.42148352f },
			{12.3801165f, 7.50282240f, -3.31175780f },
			{11.7951183f, 8.05418777f, -2.82130241f }
		};

		descr.constructionType = ShapeConstruction::HULL_FROM_POINTS;
		descr.hullFromPoints.numPoints = 8;
		descr.hullFromPoints.points = points;
		m_shape = m_world->createShape(descr);

		ColliderDescription cDescr;
		cDescr.isSensor = true;
		cDescr.material = 0;
		cDescr.shape = m_shape;
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

		m_collider = m_world->createCollider(cDescr);
	}
	void render() 
	{
		drawCollider(m_collider);
	};
	
private:
	ong::ShapePtr m_shape;
	ong::Collider* m_collider;

};



