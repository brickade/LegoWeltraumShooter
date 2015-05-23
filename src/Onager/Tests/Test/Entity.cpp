#include "Entity.h"
#include "BVH.h"
#include "Draw.h"




void impulseDamage(Collider* collider, Contact* contact)
{
	float maxImpulse = 0;

	for (int i = 0; i < contact->manifold.numPoints; ++i)
	{
		maxImpulse = ong_MAX(contact->accImpulseN[i], maxImpulse);
	}

	Entity* entity = (Entity*)collider->getBody()->getUserData();

	entity->damage((int)(collider->getBody()->getInverseMass() * maxImpulse));
}						

Entity::Entity(Body* body, vec3 color)
	:m_body(body),
	m_color(color),
	m_hp(100)
{
	m_body->setUserData(this);

	//ColliderCallbacks callbacks;
	//callbacks.postSolve = impulseDamage;

	//for (Collider* c = m_body->getCollider(); c != 0; c = c->getNext())
	//{
	//	c->setCallbacks(callbacks);
	//}
}


Entity::~Entity()
{
	if (m_body)
		m_body->getWorld()->destroyBody(m_body);
}



void Entity::render(GLuint colorLocation)
{
	if (!m_body)
		return;
	
	//contacts
#if 1
	glUniform3f(colorLocation, 1, 0, 1);
	glBegin(GL_LINES);
	if (m_body->getType() == BodyType::Dynamic)
	{
		ContactIter* iter = m_body->getContacts();
		
		while (iter)
		{
			Contact* c = iter->contact;
			ContactManifold* man = &c->manifold;


			for (int i = 0; i < man->numPoints; ++i)
			{
				vec3 p = man->points[i].position;
				vec3 n = p + man->normal;

				glVertex3f(p.x, p.y, p.z);
				glVertex3f(n.x, n.y, n.z);

				glUniform3f(colorLocation, 0, 1, 0);

				vec3 pen = p + man->points[i].penetration * man->normal;
				glVertex3f(p.x, p.y, p.z);
				glVertex3f(pen.x, pen.y, pen.z);
			}



			iter = iter->next;
		}
	}
	glEnd();
#endif



	glPushMatrix();
	vec3 p = m_body->getPosition();
	glTranslatef(p.x, p.y, p.z);

	mat3x3 q = transpose(toRotMat(m_body->getOrientation()));

	float rot[16] =
	{
		q[0][0], q[0][1], q[0][2], 0.0f,
		q[1][0], q[1][1], q[1][2], 0.0f,
		q[2][0], q[2][1], q[2][2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	glMultMatrixf(rot);
	

	vec3 cm = m_body->getLocalCenter();
	glBegin(GL_LINES);
	glVertex3f(cm.x, cm.y, cm.z);
	glVertex3f(cm.x, cm.y, cm.z + 0.1f);
	glVertex3f(cm.x, cm.y, cm.z);
	glVertex3f(cm.x, cm.y + 0.1f, cm.z);
	glEnd();



	Collider* c = m_body->getCollider();
	while (c)
	{
		glPushMatrix();

		glUniform3f(colorLocation, m_color.x, m_color.y, m_color.z);
		vec3 p = c->getTransform().p;
		glTranslatef(p.x, p.y, p.z);

		mat3x3 q = transpose(toRotMat(c->getTransform().q));

		float rot[16] =
		{
			q[0][0], q[0][1], q[0][2], 0.0f,
			q[1][0], q[1][1], q[1][2], 0.0f,
			q[2][0], q[2][1], q[2][2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};

		glMultMatrixf(rot);

		drawCollider(c);



		static const vec3 Cube[8] = 
		{
			vec3(1, 1, 1),
			vec3(-1, 1, 1),
			vec3(-1, -1, 1),
			vec3(1, -1, 1),

			vec3(1, 1, 1),
			vec3(-1, 1, 1),
			vec3(-1, -1, 1),
			vec3(1, -1, 1),
		};

		//aabb

		glPopMatrix();
#if 0
		glUniform3f(colorLocation, 1, 1, 1);
		drawBox(c->getAABB().c, c->getAABB().e);
#endif

		c = c->getNext();
	}
	



	BVTree* tree = m_body->getBVTree();

	if (false)
	{

		glUniform3f(colorLocation, 1, 1, 0);



		std::stack<BVTree*> nodes;
		nodes.push(tree);

		while (!nodes.empty())
		{
			BVTree* t = nodes.top();
			nodes.pop();


			for (int i = 0; i < 3; ++i)
			{
				for (int j = -1; j <= 1; j += 2)
				{
					glBegin(GL_LINE_STRIP);

					vec3 p[4];
					int x = i;
					int y = (i + 1) % 3;
					int z = (i + 2) % 3;


					p[0][x] = t->aabb.c[x] + j*t->aabb.e[x];
					p[0][y] = t->aabb.c[y] + t->aabb.e[y];
					p[0][z] = t->aabb.c[z] + t->aabb.e[z];

					p[1][x] = t->aabb.c[x] + j*t->aabb.e[x];
					p[1][y] = t->aabb.c[y] - t->aabb.e[y];
					p[1][z] = t->aabb.c[z] + t->aabb.e[z];

					p[2][x] = t->aabb.c[x] + j*t->aabb.e[x];
					p[2][y] = t->aabb.c[y] - t->aabb.e[y];
					p[2][z] = t->aabb.c[z] - t->aabb.e[z];

					p[3][x] = t->aabb.c[x] + j*t->aabb.e[x];
					p[3][y] = t->aabb.c[y] + t->aabb.e[y];
					p[3][z] = t->aabb.c[z] - t->aabb.e[z];


					glVertex3f(p[3].x, p[3].y, p[3].z);
					for (int i = 0; i < 4; ++i)
						glVertex3f(p[i].x, p[i].y, p[i].z);

					glEnd();
				}
			}


			if (t->type == NodeType::BRANCH)
			{
				if (t->right)
					nodes.push(tree + t->right);
				if (t->left)
					nodes.push(tree + t->left);
			}

		}
	}

	glPopMatrix();
	
}

struct ARGS
{
	Body* body;
	bool valid = true;
};

void Entity::destroy(std::vector<Entity*>& entities)
{
	World* world = m_body->getWorld();



	Collider* c = m_body->getCollider();

	ShapeDescription sDescr;
	sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;

	ColliderDescription cDescr;
	cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);
	cDescr.transform.p = vec3(0, 0, 0);
	cDescr.isSensor = false;

	BodyDescription bDescr;
	bDescr.type = BodyType::Dynamic;
	bDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

	bDescr.linearMomentum = 0.5f*m_body->getLinearMomentum();
	bDescr.angularMomentum = 0.5f*m_body->getAngularMomentum();
	
	

	for (; c != 0; c = c->getNext())
	{
		float chunkSize = 0;
		for (int i = 0; i < 3; ++i)
			chunkSize += c->getAABB().e[i];

		chunkSize /= 6.0f;


		sDescr.hullFromBox.c = vec3(0, 0, 0);
		sDescr.hullFromBox.e = vec3(chunkSize, chunkSize, chunkSize);
		
		cDescr.material = c->getMaterial();
		cDescr.shape = world->createShape(sDescr);

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{

				for (int i = 0; i < 3; ++i)
				{
					bDescr.transform.p[i] = (fmodf((float)rand(), (2.0f * c->getAABB().e.x)) - c->getAABB().e[i]) * 2.0f;
				}

				bDescr.transform.p += c->getAABB().c;


				bDescr.transform = transformTransform(bDescr.transform, m_body->getTransform());

				ARGS args;

				args.body = m_body;

				auto callback = [](Collider* collider, void* userData)->bool
				{
					if (collider->getBody() != ((ARGS*)userData)->body)
						((ARGS*)userData)->valid = false;
					return 1;
				};
				

				world->queryShape(cDescr.shape, bDescr.transform, callback, &args);

				if (args.valid)
				{
					Body* body = world->createBody(bDescr);

					body->addCollider(world->createCollider(cDescr));

					entities.push_back(new Entity(body, m_color));
					break;
				}

			}

		}

	}


	
}