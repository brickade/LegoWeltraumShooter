#pragma once

#include "Entity.h"

struct SDL_Window;

using namespace ong;

class Player : public Entity
{
public:
	Player(Body* body, vec3 color, SDL_Window* window, std::vector<Entity*>* entities);
	void update(float dt);
	
	void shoot();
		 
	void setFocus(bool focus);

	Transform getView();

	void render(GLuint colorLocation);

private:
	SDL_Window* m_pWindow;
	std::vector<Entity*>* m_entities;

	Material* m_bulletMaterial;
	
	bool m_inFocus = false;

	ShapePtr bulletShape;

	float m_coolDown;
};


