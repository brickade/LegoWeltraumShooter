#pragma once

#include "test.h"
#include "GL\glew.h"
#include "SDL_opengl.h"
#include <stdio.h>
#include <GL\GLU.h>
#include <GL\GL.h>
#include "../Profiler.h"

class RayMarchTest : public Test
{
public:

	RayMarchTest()
		:m_texture(0)

	{
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		m_width = WIDTH;
		m_height = HEIGHT;

	}

	void render() override
	{
		GLenum error = glGetError();

		Transform view = m_eye;
		if (m_playerSpec && m_player)
			m_eye = m_player->getView();



		if (view.p == m_view.p && view.q.v == m_view.q.v && view.q.w == m_view.q.w)
		{
			++m_counter;
			if (m_counter > 60)
			{
				m_width *= 2;
				m_height *= 2;

			}
		}
		else
		{
			m_counter = 0;
			m_view = view;
			m_width = 40;
			m_height = 30;

		}

		glBindTexture(GL_TEXTURE_2D, m_texture);
		if (m_width <= WIDTH || m_height <= HEIGHT)
		{

			float aspect = m_width / m_height;

			float fovY = 60 * ong_PI / 180.0f;
			float fovX = fovY * aspect;



			for (int y = 0; y < m_height; y += 1)
			{
				for (int x = (m_start + y) % m_steps; x < m_width; x += m_steps)
				{
					float _x = x / (float)m_width - 0.5f;
					float _y = y / (float)m_height - 0.5f;

					vec3 dir;

					dir.z = 0.1f;
					dir.y = -tan(_y*fovY) * 0.1f;
					dir.x = tan(_x*fovX) * 0.1f;

					//dir.x = 0;
					//dir.y = 0;
					
					dir = normalize(rotate(dir, view.q));

					vec3 o = view.p;
					//vec3 o = transformVec3( 10 * vec3(_x, -_y*aspect, 0.1f), view);


					uint8* pixel = (uint8*)((uint32*)m_pixels + y * m_width + x);

					RayQueryResult result;
					if (m_world->queryRay(o, dir, &result))
					{
						//*pixel = ((int)dot(dir, result.normal) * 255) << 8 | 255;

						pixel[0] = ong_MIN(ong_MAX(dot(-dir, result.normal)  * (1 - result.t / 120.0f), 0), 1.0) * 0xff;
						pixel[1] = 0x00;
						pixel[2] = 0x00;
						pixel[3] = 0xff;
					}
					else
					{
						pixel[0] = 0x00;
						pixel[1] = 0x00;
						pixel[2] = 0x00;
						pixel[3] = 0xff;
					}

				}
			}

			
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_pixels);
			
		}
		else
		{
			m_width = WIDTH;
			m_height = HEIGHT;
		}

		error = glGetError();

		int program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);
		glUseProgram(0);

		error = glGetError();



		glPushMatrix();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, m_width, m_height, 0.0, 1.0, -1.0);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0, 0, 0);

		error = glGetError();

		float uvx = m_width / (float)WIDTH;
		float uvy = m_height/ (float)HEIGHT;

		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
			glTexCoord2f(uvx, 0.f); glVertex2f(m_width, 0.f);
			glTexCoord2f(uvx, uvy); glVertex2f(m_width, m_height);
			glTexCoord2f(0.f, uvy); glVertex2f(0.f, m_height);
		}
		glEnd();

		error = glGetError();

		glPopMatrix();


		glBindTexture(GL_TEXTURE_2D, 0);

		glUseProgram(program);	


		if (error != GL_NO_ERROR)
			printf("%s\n", gluErrorString(error));


	}

private:
	static const int WIDTH = 320;
	static const int HEIGHT = 240;

	int m_width;
	int m_height;

	GLuint m_texture;

	uint32 m_pixels[HEIGHT][WIDTH];

	int m_start = 0;
	int m_steps = 1;

	Transform m_view;
	int m_counter = 0;

};


