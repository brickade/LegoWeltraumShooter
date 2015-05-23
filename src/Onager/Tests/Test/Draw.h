#pragma once

#include "Collider.h"
#include <GL\GLEW.h>


inline void drawCollider(Collider* collider)
{

	switch (collider->getShape().getType())
	{
	case ShapeType::HULL:
	{
		const Hull* hull = collider->getShape();

		glBegin(GL_LINES);

		for (int i = 0; i < hull->numEdges; i += 2)
		{
			vec3 p1 = hull->pVertices[hull->pEdges[i].tail];
			vec3 p2 = hull->pVertices[hull->pEdges[i + 1].tail];

			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
		}

		glEnd();
		break;
	}
	case ShapeType::SPHERE:
	{
		const Sphere* sphere = collider->getShape();
		
		static const int POINTCOUNT = 32;


		for (int i = 0; i < 3; ++i)
		{
			glBegin(GL_LINE_LOOP);
			for (int j = 0; j < POINTCOUNT * sphere->r; ++j)
			{
				vec3 p = sphere->c;

				p[(i + 1) % 3] += sin(2 * ong_PI * j / (POINTCOUNT * sphere->r)) * sphere->r;
				p[(i + 2) % 3] += cos(2 * ong_PI * j / (POINTCOUNT * sphere->r)) * sphere->r;

				glVertex3f(p.x, p.y, p.z);
			}
			glEnd();
		}

		break;
	}
	case ShapeType::CAPSULE:
	{
		const Capsule* c = collider->getShape();

		static const int POINTCOUNT = 32;

		glPushMatrix();
		vec3 up = normalize(c->c2 - c->c1);
		vec3 l;
		if (abs(up.x) < abs(up.y) && abs(up.x) < abs(up.z))
			l = normalize(cross(vec3(1, 0, 0), up));
		else if (abs(up.y) < abs(up.z))
			l = normalize(cross(vec3(0, 1, 0), up));
		else
			l = normalize(cross(vec3(0, 0, 1), up));


		vec3 s = normalize(cross(up, l));

		float rot[16] =
		{
			s.x, s.y, s.z, 0.0f,
			up.x, up.y, up.z, 0.0f,
			l.x, l.y, l.z, 0.0f,
			0, 0, 0, 1
		};

		glMultMatrixf((float*)rot);

		vec3 v = vec3(0, 1, 0);
		float h = length(c->c2 - c->c1)/2;

		for (int i = 0; i < 3; i+=2)
		{
			glBegin(GL_LINE_LOOP);
			for (int j = 0; j < POINTCOUNT * c->r; ++j)
			{
				float angle = 2 * ong_PI * j / (POINTCOUNT * c->r);

				
				vec3 p(0,0,0);
				p[(i + 1) % 3] = sin(angle) * c->r;
				p[(i + 2) % 3] = cos(angle) * c->r;


				if (p.y > 0.0f)
					glVertex3f(p.x, p.y + h, p.z);
				else if (p.y < 0.0f)
					glVertex3f(p.x, p.y -h, p.z);
				else
				{
					int s = j >= POINTCOUNT / 2 * c->r ? 1 : -1;

					glVertex3f(p.x, p.y + s*h, p.z);
					glVertex3f(p.x, p.y - s*h, p.z);

				}

				
			}
			glEnd();
		}

		for (int i = -1; i < 2; i+=2)
		{
			glBegin(GL_LINE_LOOP);
			for (int j = 0; j < POINTCOUNT * c->r; ++j)
			{
				float angle = 2 * ong_PI * j / (POINTCOUNT * c->r);

				vec3 p(0, 0, 0);
				p[2] = sin(angle) * c->r;
				p[0] = cos(angle) * c->r;

				glVertex3f(p.x, p.y + i*h, p.z);
			}
			glEnd();
		}

		glPopMatrix();
		break;
	}
	default:
		break;
	}

}


inline void drawBox(vec3 c, vec3 e)
{

	static const vec3 faces[6][4] = 
	{
		{ vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1) },
		{ vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1) },
		{ vec3(1, 1, 1), vec3(1, 1, -1), vec3(1, -1, -1), vec3(1, -1, 1) },
		{ vec3(-1, 1, 1), vec3(-1, 1, -1), vec3(-1, -1, -1), vec3(-1, -1, 1) },
		{ vec3(1, 1, 1), vec3(1, 1, -1), vec3(-1, 1, -1), vec3(-1, 1, 1) },
		{ vec3(1, -1, 1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, -1, 1) }
	};


	for (int i = 0; i < 6; ++i)
	{
		glBegin(GL_LINE_LOOP);
		for (int j = 0; j < 4; ++j)
		{
			vec3 p = c + hadamardProduct(faces[i][j], e);
			glVertex3f(p.x, p.y, p.z);
		}
		glEnd();
	}

}