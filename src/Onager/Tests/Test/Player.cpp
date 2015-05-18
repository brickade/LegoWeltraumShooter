#include "Player.h"
#include "SDL.h"
#include "Bullet.h"


Player::Player(Body* body, vec3 color, SDL_Window* window, std::vector<Entity*>* entities)
	: Entity(body, color),
	m_pWindow(window),
	m_entities(entities)
{

	Material material;

	material.density = 100.0f;
	material.restitution = 0.6f;
	material.friction = 0.1f;

	m_bulletMaterial = m_body->getWorld()->createMaterial(material);
}


void bulletImpact(Collider* collider, Contact* contact)
{
	float impulse = 0;

	for (int i = 0; i < contact->manifold.numPoints; ++i)
	{
		impulse = ong_MAX(contact->accImpulseN[i], impulse);
	}
	if (impulse == 0)
		return;

	impulse *= 100.0f;


	Body* b = (collider == contact->colliderA ? contact->colliderB : contact->colliderB)->getBody();

	Entity* entity = (Entity*)b->getUserData();
	entity->damage(impulse * b->getInverseMass());
	
	
	Entity* bullet = (Entity*)collider->getBody()->getUserData();
	bullet->damage(100);
}

void Player::shoot()
{
	if (m_coolDown > 0.0f)
		return;

	vec3 cannons[] =
	{
		vec3(0.75, 0.0f, 1.0),
		vec3(-0.75, 0.0f, 1.0)
	};

	for (vec3& cannon : cannons)
	{

		if (!bulletShape)
		{
			ShapeDescription descr;
			descr.constructionType = ShapeConstruction::HULL_FROM_BOX;
			descr.hullFromBox.c = vec3(0, 0, 0);
			descr.hullFromBox.e = vec3(0.1, 0.1, 0.1);
			
			bulletShape = m_body->getWorld()->createShape(descr);
		}

		ColliderDescription colliderDescr;
		colliderDescr.material = m_bulletMaterial;
		colliderDescr.transform.p = vec3(0.0f, 0.0f, 0.0f);
		colliderDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f),0.0f);
		colliderDescr.shape = bulletShape;

		Collider* collider = m_body->getWorld()->createCollider(colliderDescr);

		ColliderCallbacks callbacks;
		callbacks.postSolve = bulletImpact;
		collider->setCallbacks(callbacks);

		BodyDescription bodyDescr;
		bodyDescr.type = BodyType::Dynamic;
		bodyDescr.transform.p = transformVec3(cannon, m_body->getTransform());
		bodyDescr.transform.q = m_body->getOrientation();;
		bodyDescr.linearMomentum = rotate(vec3(0.0f, 0.0f, 50.0f), m_body->getOrientation());
		bodyDescr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);

		Body* body = m_body->getWorld()->createBody(bodyDescr);
		body->addCollider(collider);

		m_entities->push_back(new Bullet(body, vec3(0, 1, 1)));
	}

	m_coolDown = 0.2f;
}


void Player::grab()
{
	Transform bodyTransform = m_body->getTransform();

	vec3 rayO = transformVec3(vec3(0, 0, 2.0f), bodyTransform);
	vec3 rayDir = rotate(vec3(0, 0, 1), bodyTransform.q);

	RayQueryResult result;
	if (m_body->getWorld()->queryRay(rayO, rayDir, &result))
	{
		if (result.collider->getBody()->getType() == BodyType::Dynamic)
		{
			m_grab.body = result.collider->getBody();
			m_grab.point = invTransformVec3(result.point, m_grab.body->getTransform());
			m_grab.anchor = result.t;
		}
	}

}

void Player::setFocus(bool focus)
{
	m_inFocus = focus;
}

void Player::update(float dt)
{
	//printf("hp: %d\n", m_hp);

	m_coolDown -= dt;


	if (SDL_GetMouseState(0, 0) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		shoot();
	}

	//update grab
	if (SDL_GetKeyboardState(0)[SDL_SCANCODE_E])
	{
		if (!m_grab.body)
		{
			grab();
		}

		if (m_grab.body)
		{
			vec3 anchor = transformVec3(vec3(0, 0, 10.0f), m_body->getTransform());
			vec3 forceDir = invTransformVec3(anchor, m_grab.body->getTransform()) - m_grab.point;
			float forceStrength = 3000.0f;
			
			vec3 v = m_grab.body->getRelativeLinearVelocity();

			vec3 damp = 500.0f * v;


			m_grab.body->applyRelativeForce(forceStrength * forceDir - damp, m_grab.point, dt);
		}
	}
	else
	{
		m_grab.body = 0;
	}
	

	float forAcc = 5000.0f;
	float yawAcc = 2000.0f;
	float pitchAcc = 2000.0f;
	float rollAcc = 2000.0f;

	float maxForSpeed = 7.0f;
	//float maxForSpeed = 0.0f;
	float maxYawSpeed = 5.0f;
	float maxPitchSpeed = 5.0f;
	float maxRollSpeed = 2.0f;
		
	

	if (SDL_GetKeyboardState(0)[SDL_SCANCODE_W])
	{
		maxForSpeed *= 2.0f;

		if (SDL_GetModState() & KMOD_LSHIFT)
		{
			maxForSpeed *= 2.0f;
		}

	}
	else if (SDL_GetKeyboardState(0)[SDL_SCANCODE_S])
	{
		maxForSpeed /= 2.0f;
	}
	

	float targetRollSpeed = 0.0f;
	if (SDL_GetKeyboardState(0)[SDL_SCANCODE_A])
	{
		targetRollSpeed = maxRollSpeed;
	}
	else if (SDL_GetKeyboardState(0)[SDL_SCANCODE_D])
	{
		targetRollSpeed = -maxRollSpeed;
	}

	
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	int width, height;
	SDL_GetWindowSize(m_pWindow, &width, &height);
	
	float x = (mouseX / (float)width) - 0.5f;
	float y = (mouseY / (float)height) - 0.5f;

	//if (abs(x) < 0.1f) x = 0.0f;
	//if (abs(y) < 0.1f) y = 0.0f;


	//x = y = 0.0f;

	float targetPitchSpeed = y * maxPitchSpeed;
	float targetYawSpeed = x * maxPitchSpeed;


	vec3 currVel = rotate(m_body->getLinearVelocity(), conjugate(m_body->getOrientation()));
	vec3 currAng = rotate(m_body->getAngularVelocity(), conjugate(m_body->getOrientation()));

	float currSpeed = currVel.x;
	float rollSpeed = currAng.z;
	float pitchSpeed = currAng.x;
	float yawSpeed = currAng.y;


	vec3 Torque(0, 0, 0);

	vec3 targetVel = vec3(0, 0, maxForSpeed);
	vec3 targetAng = vec3(targetPitchSpeed, targetYawSpeed, targetRollSpeed);


	// negate rotational velocity
	m_body->applyRelativeImpulse(1.0f/m_body->getInverseMass() * vec3(targetVel.x - currVel.x, targetVel.y - currVel.y, 0));


	// apply thrust
	if (lengthSq(currVel-targetVel) > 0.1f * 0.1f)
		m_body->applyRelativeForce(hadamardProduct(vec3(0, 0, forAcc), normalize(targetVel - currVel)), dt);

	if (lengthSq(currAng - targetAng) > 0.00f*0.01f)
	{
		
		vec3 f = hadamardProduct(vec3(-pitchAcc, -yawAcc, -rollAcc), normalize(currAng - targetAng));
		vec3 dAng = m_body->getInverseMass() * dt* f;
		
		if (lengthSq(dAng) > lengthSq(currAng - targetAng))
		{
			vec3 dAngularMomentum = 1.0f / m_body->getInverseMass() * (targetAng - currAng);
			m_body->applyRelativeAngularImpulse(dAngularMomentum);
		}
		else
		{
			m_body->applyRelativeTorque(f, dt);
		}
		
	}


	//printf("currSpeed:	(%.2f|%.2f|%.2f),	currAng:	(%.2f|%.2f|%.2f)\n", currVel.x, currVel.y, currVel.z, currAng.x, currAng.y, currAng.y);
	//printf("targetSpeed:	(%.2f|%.2f|%.2f),	targetAng:	(%.2f|%.2f|%.2f)\n", targetVel.x, targetVel.y, targetVel.z, targetAng.x, targetAng.y, targetAng.y);
}


Transform Player::getView()
{
	Transform view;

	float v = (m_body->getRelativeLinearVelocity().z * 0.8f) + 1.5;
	vec3 w = m_body->getRelativeAngularVelocity();

	Quaternion q = QuatFromAxisAngle(normalize(w), -0.05f*length(w));

	vec3 l = rotate(vec3(0, 0, 1), q);
 
	float focal = 15.0f;

	l = focal / l.z * l;


	vec3 eye = vec3(0.0f, 2.0f, -v) +  2.0f * (vec3(0,0,focal) - l);

	view.p = transformVec3(eye, m_body->getTransform());

	view.q =  m_body->getOrientation();


	return view;
}


void Player::render(GLuint colorLocation)
{
	vec3 p = vec3(0, 0, 15.0f);

	vec3 p1 = transformVec3(p + 0.5f * vec3(-1, 1, 0), m_body->getTransform());
	vec3 p2 = transformVec3(p + 0.5f * vec3(1, 1, 0), m_body->getTransform());
	vec3 p3 = transformVec3(p + 0.5f * vec3(1, -1, 0), m_body->getTransform());
	vec3 p4 = transformVec3(p + 0.5f * vec3(-1, -1, 0), m_body->getTransform());
	
	
	glUniform3f(colorLocation, 1, 0, 1);

	glBegin(GL_LINE_STRIP);
	{
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
		glVertex3f(p4.x, p4.y, p4.z);
		glVertex3f(p1.x, p1.y, p1.z);
	}
	glEnd();

	if (m_grab.body)
	{
		glBegin(GL_LINES);
		{
			vec3 anchor = transformVec3(vec3(0, 0, 2.0f), m_body->getTransform());
			vec3 point = transformVec3(m_grab.point, m_grab.body->getTransform());

			glVertex3f(anchor.x, anchor.y, anchor.z);
			glVertex3f(point.x, point.y, point.z);
		}
		glEnd();
	}



	Entity::render(colorLocation);
}