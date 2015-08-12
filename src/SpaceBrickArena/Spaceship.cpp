#include "include/Spaceship.h"

#include "TheBrick/Conversion.h"
#include "TheBrick/BrickInstance.h"
#include "TheBrick/Brick.h"

#include "include/Player.h"
#include "include/Editor_BrickCategory.h"

#include "include/Space.h"

#include "Onager/BVH.h"

namespace sba
{
    const float g_Rand[] = { -56.0f, -250.0f, 45.0f, -360.0f, 106.0f,
        256.0f, -221.0f, -343.0f, 281.0f, -91.0f,
        -328.0f, 132.0f, 175.0f, 318.0f, 40.0f,
        306.0f, -316.0f, 231.0f, -157.0f, 159.0f,
        -150.0f, -150.0f, -400.0f, 183.0f, 145.0f,
        -167.0f, 85.0f, -100.0f, 174.0f, -40.0f,
        -134.0f, -142.0f, 66.0f, 184.0f, 363.0f,
        397.0f, 38.0f, 358.0f, -355.0f, -201.0f,
        -88.0f, -196.0f, -302.0f, 108.0f, 252.0f,
        -286.0f, 212.0f, 337.0f, 215.0f, -365.0f,
        -340.0f, -383.0f, 322.0f, -226.0f, 274.0f,
        -306.0f, -1.0f, -256.0f, 351.0f, -177.0f,
        147.0f, -201.0f, 384.0f, -224.0f, -251.0f,
        -135.0f, -290.0f, 198.0f, 223.0f, -337.0f,
        283.0f, -282.0f, -11.0f, -323.0f, 330.0f,
        -232.0f, -321.0f, -117.0f, -150.0f, -19.0f,
        -184.0f, 175.0f, 276.0f, 90.0f, 198.0f,
        189.0f, 361.0f, 107.0f, -187.0f, -438.0f,
        -380.0f, -298.0f, 299.0f, 298.0f, -137.0f,
        367.0f, 107.0f, 254.0f, 1.0f, 300.0f };
#ifdef EDITOR_DEV
    const int CSpaceship::MAX_BRICK_COUNT = 500;
    const int CSpaceship::MAX_BRICK_WIDTH = 40 * 3;
#else
    const int CSpaceship::MAX_BRICK_COUNT = 200;
    const int CSpaceship::MAX_BRICK_WIDTH = 15 * 3;
#endif
    const int CSpaceship::MAX_BRICK_HEIGHT = CSpaceship::MAX_BRICK_WIDTH;
    const int CSpaceship::MAX_COCKPITS = 2;
    const int CSpaceship::MAX_ENGINES = 4;
    const int CSpaceship::MAX_WEAPONS = 4;
    const int CSpaceship::MAX_PERKS = 6;

    // **************************************************************************
    // **************************************************************************
	CSpaceship::CSpaceship(ong::World& a_rWorld, std::string a_Name) : CDestructibleObject(a_rWorld, nullptr)
    {
        this->m_Type = TheBrick::EGameObjectType::Ship;
        this->m_TargetVec = ong::vec3(0.0f, 0.0f, 0.0f);
        this->m_TargetAng = ong::vec3(0.0f, 0.0f, 0.0f);
        this->m_Name = a_Name;
        this->m_Respawn = 0.0f;
        this->m_pBrickArray = NULL;
    }

    // **************************************************************************
    // **************************************************************************
    CSpaceship::~CSpaceship()
    {
        for (unsigned int i = 0; i < this->m_EngineEmitter.size(); i++)
            SAFE_DELETE(this->m_EngineEmitter[i]);
        this->m_EngineEmitter.clear();
        SAFE_DELETE_ARRAY(this->m_pBrickArray);
        this->m_pBrickArray = NULL;
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Collision(ong::Collider* thisCollider, ong::Contact* contact)
    {
        CSpaceship* Ship = (CSpaceship*)thisCollider->getBody()->getUserData();
        ong::Collider* other;
        if (thisCollider == contact->colliderA)
            other = contact->colliderB;
        else
            other = contact->colliderA;

        CGameObject* object = static_cast<CGameObject*>(other->getBody()->getUserData());
        if (object->m_Type == TheBrick::EGameObjectType::Ship)
        {

            //CSpaceship* oship = static_cast<CSpaceship*>(object);
            //if (oship->m_Respawn == 0.0f)
            //    oship->m_Life -= 10;
            //if (Ship->m_Respawn == 0.0f)
            //    Ship->m_Life -= 10;

        }
        else if (object->m_Type == TheBrick::EGameObjectType::Item)
        {
            CItem* item = static_cast<CItem*>(object);
            if (!item->m_Collided)
            {
                switch (item->GetType())
                {
                    case sba::EItemType::Repair:
                        Ship->m_Life += 10;
                        if (Ship->m_Life  > Ship->m_MaxLife)
                            Ship->m_Life = Ship->m_MaxLife;
                        break;
                    case sba::EItemType::Shield:
                        Ship->m_Shield += 100;
                        if (Ship->m_Shield  > 100)
                            Ship->m_Shield = 100;
                        break;
                    case sba::EItemType::Speed:
                        Ship->m_SpeedFactor = 5.0f;
                        break;
                }
                item->m_Collided = true;
            }
        }
        else if (object->m_Type == TheBrick::EGameObjectType::Bullet)
        {
            CBullet* bull = static_cast<CBullet*>(object);
            if (!bull->m_Collided)
            {
                if (Ship->m_Respawn == 0.0f)
                {
                    Ship->m_Shake = 1.0f;
                    int damage = bull->m_Damage;
                    //If shield is on, make shield get damage before the ship
                    if (Ship->m_Shield > 0)
                    {
                        Ship->m_Shield -= damage;
                        if (Ship->m_Shield < 0)
                        {
                            damage = -Ship->m_Shield;
                            Ship->m_Shield = 0;
                        }
                        else
                            damage = 0;
                    }

                    Ship->m_Life -= damage;
                    if (Ship->m_Life < 0)
                    {
                        bull->m_pOwner->Points += 100;
                        bull->m_pOwner->KilledTimer = 1.0f;
                    }
                    bull->m_pOwner->Marker = 1.0f;
                }
                if (bull->m_ID != TheBrick::Laser)
                    bull->m_Collided = true;
            }
        }
        else
        {

        }
        
            
    }

	void CSpaceship::CalculateProperties()
    {

		float mass = 1.0f / this->m_pBody->getInverseMass();
		this->m_SpeedAcceleration = mass*30.0f;
        this->m_MaxRotationSpeed = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        this->m_RotationAcceleration = PuRe_Vector3F(mass*100.0f, mass*200.0f, mass*200.0f);
        this->m_MaxSpeed = 0.0f;
        float rot = 0.0f;
        std::vector<TheBrick::CBrickInstance**> engines;
        this->GetEngines(engines);
        for (std::vector<TheBrick::CBrickInstance**>::iterator it = engines.begin(); it != engines.end(); ++it)
        {
            TheBrick::CBrickInstance* engine = *(*it);
            if (engine->m_pBrick->GetBrickId() == 700)
            {
                this->m_MaxSpeed += std::stof(sba_Balancing->GetValue("Engine1_Speed"));
                rot = std::stof(sba_Balancing->GetValue("Engine1_Rot"))/100.0f;
                this->m_MaxRotationSpeed += PuRe_Vector3F(rot, rot, rot);
            }
            else if (engine->m_pBrick->GetBrickId() == 701)
            {
                this->m_MaxSpeed += std::stof(sba_Balancing->GetValue("Engine2_Speed"));
                rot = std::stof(sba_Balancing->GetValue("Engine2_Rot")) / 100.0f;
                this->m_MaxRotationSpeed += PuRe_Vector3F(rot, rot, rot);
            }
            else if (engine->m_pBrick->GetBrickId() == 702)
            {
                this->m_MaxSpeed += std::stof(sba_Balancing->GetValue("Engine3_Speed"));
                rot = std::stof(sba_Balancing->GetValue("Engine3_Rot")) / 100.0f;
                this->m_MaxRotationSpeed += PuRe_Vector3F(rot, rot, rot);
            }
        }
        this->m_MaxSpeed /= mass;
		
		this->m_Shield = 0;
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Respawn(ong::vec3 a_Position)
    {
        unsigned int size = this->m_BrickCount;
        for (size_t i = 0; i < this->m_pBricks.size(); i++)
        {
            SAFE_DELETE(this->m_pBricks[i]);
            i--;
        }
        this->m_pBricks.clear();

        for (unsigned int i = 0; i < this->m_EngineEmitter.size(); i++)
            SAFE_DELETE(this->m_EngineEmitter[i]);
        this->m_EngineEmitter.clear();

        for (unsigned int i=0;i<size;i++)
            this->AddBrick(this->m_pBrickArray[i], sba_BrickManager->GetBrickArray(), *sba_World);


        this->m_Respawn = 0.0f;
        ong::vec3 null = ong::vec3(0, 0, 0);
        this->m_pBody->setPosition(a_Position);
        this->m_pBody->setAngularMomentum(null);
        this->m_pBody->setLinearMomentum(null);
        this->m_pBody->setOrientation(ong::QuatFromTwoVectors(ong::normalize(a_Position), null));
        this->CalculateProperties();
        this->CalculateReset();

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::CalculateReset()
    {
        ong::Collider* c = this->m_pBody->getCollider();
        while (c)
        {
            ong::ColliderCallbacks cb = c->getColliderCallbacks();
            cb.beginContact = CSpaceship::Collision;

            c->setCallbacks(cb);
            c = c->getNext();
        }

        //Add emitter
        std::vector<TheBrick::CBrickInstance**> engines;
        this->GetEngines(engines);
        for (std::vector<TheBrick::CBrickInstance**>::iterator it = engines.begin(); it != engines.end(); ++it)
        {
            TheBrick::CBrickInstance* engine = *(*it);
            PuRe_Vector3F pos = engine->PosToWorldSpace(engine->GetCenter());
            PuRe_ParticleEmitter* emitter = new PuRe_ParticleEmitter(pos, PuRe_QuaternionF());
            this->m_EngineEmitter.push_back(emitter);
        }

        Build();

        float mass = 1.0f / this->m_pBody->getInverseMass();
        this->m_MaxLife = (int)(mass*std::stof(sba_Balancing->GetValue("LifeMultiplicator")));
        this->m_OldMass = mass;
        this->m_Life = this->m_MaxLife;
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::CalculateData()
    {
        //Set Data once
        SAFE_DELETE_ARRAY(this->m_pBrickArray);
        this->m_pBrickArray = new TheBrick::SBrickData[this->m_pBricks.size()];
        this->m_BrickCount = this->m_pBricks.size();
        for (unsigned int i=0;i<this->m_pBricks.size();i++)
        {
            this->m_pBrickArray[i].ID = this->m_pBricks[i]->m_pBrick->GetBrickId();
            this->m_pBrickArray[i].Transform = this->m_pBricks[i]->GetTransform();
            this->m_pBrickArray[i].Color = this->m_pBricks[i]->m_Color;
        }
        //set max life here because it is set here with full Data

        CalculateProperties();
        this->CalculateReset();
    }

	void CSpaceship::ReCalculateData()
	{
        CalculateProperties();

        float mass = 1.0f / this->m_pBody->getInverseMass();
        float maxmass = this->m_MaxLife/10.0f;
        float maxxDiff = ((this->m_OldMass - mass) / maxmass);

		// adjust life procentual
        int damage = (int)(this->m_MaxLife * maxxDiff);
        damage *= 5;
        assert(damage >= 0);	
        this->m_Life -= damage;
        //mass difference
        this->m_OldMass = mass;
        if (this->m_Life < 0) this->m_Life = 0;

		printf("damage: %i\n", damage);
	}


    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Shoot(int a_Weapon, std::vector<CBullet*>& a_rBullets, SPlayer* a_pOwner, PuRe_Vector2F a_Direction,int a_Time)
    {
        if (this->m_Life > 0)
        {
            ong::Body* b = this->m_pBody;
            ong::World* w = b->getWorld();
            //Add emitter
            std::vector<TheBrick::CBrickInstance**> weapons;
            this->GetWeapons(weapons);
            unsigned int wID = 0;

            a_Direction.X /= 2.0f;
            a_Direction.Y /= 3.0f;

            if (a_Direction.X > 0.0f)
                a_Direction.X *= a_Direction.X;
            else
                a_Direction.X *= -a_Direction.X;

            if (a_Direction.Y > 0.0f)
                a_Direction.Y *= a_Direction.Y*4.0f;
            else
                a_Direction.Y *= -a_Direction.Y;

            for (std::vector<TheBrick::CBrickInstance**>::iterator it = weapons.begin(); it != weapons.end(); ++it)
            {
                TheBrick::CBrickInstance* weapon = *(*it);
                if (weapon->m_pBrick->GetBrickId() == a_Weapon)
                {
                    ong::Transform transform = weapon->GetTransform();
                    ong::Transform ship = this->m_pBody->getTransform();
                    ong::Transform wtransform = ong::transformTransform(transform, ship);
                    PuRe_Vector3F pos = TheBrick::OngToPuRe(wtransform.p);

                    PuRe_Vector3F forward = PuRe_Vector3F(0.0f, 0.0f, 1.0f) * TheBrick::OngToPuRe(wtransform.q);
                    PuRe_Vector3F sforward = PuRe_Vector3F(0.0f, 0.0f, 1.0f) * TheBrick::OngToPuRe(ship.q);

                    PuRe_Vector3F side = PuRe_Vector3F(1.0f, 0.0f, 0.0f) * TheBrick::OngToPuRe(wtransform.q);
                    PuRe_Vector3F up = PuRe_Vector3F(0.0f, 1.0f, 0.0f) * TheBrick::OngToPuRe(wtransform.q);



                    float len = TheBrick::OngToPuRe(this->m_pBody->getLinearVelocity()).Length();
                    PuRe_Vector3F speed;
                    a_Direction.X -= transform.p.x / 50.0f;
                    a_Direction.Y -= transform.p.y / 50.0f;
                    if (PuRe_Vector3F::Dot(forward, sforward) > 0.9f) // in front
                    {
                        PuRe_Vector3F dir = PuRe_Vector3F(a_Direction.X, -a_Direction.Y, 1.0f) * TheBrick::OngToPuRe(wtransform.q);
                        speed = dir*200.0f + dir * len;
                    }
                    else
                        speed = forward*200.0f+forward *len;

                    speed *= 1.0f / 1000.0f;

                    ong::BodyDescription bdesc;
                    ong::Quaternion diff = ong::QuatFromEulerAngles(a_Direction.X, a_Direction.Y, 0.0f);


                    bdesc.angularMomentum = ong::vec3(0, 0, 0);
                    bdesc.transform.q = ship.q*transform.q*diff;
                    bdesc.type = ong::BodyType::Dynamic;
					bdesc.continuousPhysics = false;

                    unsigned int id=0;
                    PuRe_Color col = PuRe_Color(1.0f,0.0f,0.0f,1.0f);
                    switch (weapon->m_pBrick->GetBrickId())
                    {

                    case TheBrick::Laser-100: //Laser
                        id = TheBrick::Laser;
                        pos += forward*10.0f;
                        speed *= std::stof(sba_Balancing->GetValue("Laser_Speed"));
                        bdesc.transform.q = ship.q*transform.q;
                        col = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
						bdesc.continuousPhysics = true;
                        bdesc.type = ong::BodyType::Static;
                        break;

                    case TheBrick::MG - 100: //MG
                        id = TheBrick::MG;
                        pos += forward*10.0f;
                        speed *= std::stof(sba_Balancing->GetValue("MG_Speed"));
                        speed *= 0;
                        col = PuRe_Color(1.0f, 0.0f, 0.0f, 1.0f);
						bdesc.continuousPhysics = true;
                        break;

                    case TheBrick::Mine - 100: //Mine
                        id = TheBrick::Mine;
                        pos += forward*4.0f;
                        speed *= std::stof(sba_Balancing->GetValue("Mine_Speed"));
                        col = PuRe_Color(1.0f, 0.0f, 1.0f, 1.0f);
                        break;

                    case TheBrick::Rocket - 100: //Rocket
                        pos += forward*10.0f;
                        id = TheBrick::Rocket;
                        speed *= std::stof(sba_Balancing->GetValue("Rocket_Speed"));
                        col = PuRe_Color(1.0f, 0.0f, 0.0f, 1.0f);
                        break;
                    }
                    bdesc.linearMomentum = TheBrick::PuReToOng(speed);
                    bdesc.transform.p = TheBrick::PuReToOng(pos);
                    if (weapon->m_pBrick->GetBrickId() == TheBrick::Rocket - 100)
                    {
                        pos += side*1.0f;
                        bdesc.transform.p = TheBrick::PuReToOng(pos);
                        a_rBullets.push_back(new CBullet(&bdesc, *w, a_pOwner, col, id,weapon));
                        pos -= side*2.0f;
                        bdesc.transform.p = TheBrick::PuReToOng(pos);
                        a_rBullets.push_back(new CBullet(&bdesc, *w, a_pOwner, col, id, weapon));
                        pos += side*1.0f;
                        pos += up*1.0f;
                        bdesc.transform.p = TheBrick::PuReToOng(pos);
                        a_rBullets.push_back(new CBullet(&bdesc, *w, a_pOwner, col, id, weapon));
                        pos -= up*2.0f;
                        bdesc.transform.p = TheBrick::PuReToOng(pos);
                        a_rBullets.push_back(new CBullet(&bdesc, *w, a_pOwner, col, id, weapon));
                    }
                    else if(weapon->m_pBrick->GetBrickId() == TheBrick::Mine - 100)
                    {
                        int mines_row = std::stoi(sba_Balancing->GetValue("Mine_Row"));
                        int mines_column = std::stoi(sba_Balancing->GetValue("Mine_Column"));
                        float mines_dist = std::stof(sba_Balancing->GetValue("Mine_Dist"));
                        pos += side*(mines_column / 2.0f)*mines_dist;
                        pos += up*(mines_row / 2.0f)*mines_dist;
                        for (int y=0;y<mines_row;y++)
                        {
                            for (int x = 0; x < mines_column; x++)
                            {
                                pos -= side*(float)(sin(a_Time)+x)*mines_dist;
                                pos -= up*(float)(cos(a_Time) + y)*mines_dist;
                                pos += forward*((float)sin(a_Time + x*10.0f + y*10.0f) + 1.0f)*mines_dist;
                                bdesc.transform.p = TheBrick::PuReToOng(pos);
                                a_rBullets.push_back(new CBullet(&bdesc, *w, a_pOwner, col, id,weapon));
                                pos += side*(float)(sin(a_Time) + x)*mines_dist;
                                pos += up*(float)(cos(a_Time) + y)*mines_dist;
                                pos -= forward*((float)sin(a_Time + x*10.0f + y*10.0f) + 1.0f)*mines_dist;
                            }
                        }

                    }
                    else
                        a_rBullets.push_back(new CBullet(&bdesc, *w, a_pOwner, col, id, weapon));
                }
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Spin(float a_Spin)
    {
        if (a_Spin > 0.2f || a_Spin < -0.2f)
            this->m_TargetAng.z = a_Spin * this->m_MaxRotationSpeed.X;
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Move(PuRe_Vector2F a_Move)
    {
        if (a_Move.Length() > 0.5f)
        {
            this->m_TargetAng.x = a_Move.Y * this->m_MaxRotationSpeed.Z;
            this->m_TargetAng.y = a_Move.X * this->m_MaxRotationSpeed.Y;
        }

    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Thrust(float a_Thrust)
    {
        this->m_TargetVec.z += a_Thrust * this->m_MaxSpeed;
    }
    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Update(int a_OID, float a_DeltaTime, int a_Time)
    {

        ong::vec3 currVel = ong::rotate(this->m_pBody->getLinearVelocity(), ong::conjugate(this->m_pBody->getOrientation()));
        if (this->m_Respawn > 0.0f)
        {
            this->m_Respawn -= a_DeltaTime;
            if (this->m_Respawn < 0.0f)
            {
                ong::vec3 pos;
                int index = (a_OID + a_Time) % 100;
                int index2 = (index + 1) % 100;
                int index3 = (index + 2) % 100;
                pos.x = g_Rand[index];
                pos.y = g_Rand[index2];
                pos.z = g_Rand[index3];
                printf("Pos: %f %f %f \n",pos.x,pos.y,pos.z);
                this->Respawn(pos);
            }
        }
        else
        {
            if (this->m_SpeedFactor != 0.0f)
            {
                this->m_SpeedFactor -= a_DeltaTime;
                if (this->m_SpeedFactor < 0.0f)
                    this->m_SpeedFactor = 0.0f;
                else
                    this->m_TargetVec.z *= 2.0f;
            }
            ong::vec3 currAng = ong::rotate(this->m_pBody->getAngularVelocity(), ong::conjugate(this->m_pBody->getOrientation()));

            float forAcc = this->m_SpeedAcceleration;
            float yawAcc = this->m_RotationAcceleration.Y;
            float pitchAcc = this->m_RotationAcceleration.Z;
            float rollAcc = this->m_RotationAcceleration.X;
            // negate rotational velocity
            this->m_pBody->applyRelativeImpulse(1.0f / this->m_pBody->getInverseMass() * ong::vec3(this->m_TargetVec.x - currVel.x, this->m_TargetVec.y - currVel.y, 0));

            // apply thrust
            if (lengthSq(currVel - this->m_TargetVec) > 0.1f * 0.1f)
                this->m_pBody->applyRelativeForce(hadamardProduct(ong::vec3(0, 0, forAcc), normalize(this->m_TargetVec - currVel)), a_DeltaTime);

            //apply rotation
            if (ong::lengthSq(currAng - this->m_TargetAng) > 0.0f)
            {

                ong::vec3 f = ong::hadamardProduct(ong::vec3(pitchAcc, yawAcc, rollAcc), ong::normalize(this->m_TargetAng - currAng));
                ong::vec3 dAng = this->m_pBody->getInverseMass() * a_DeltaTime* f;

                if (ong::lengthSq(dAng) > ong::lengthSq(currAng - this->m_TargetAng))
                {
                    ong::vec3 dAngularMomentum = 1.0f / this->m_pBody->getInverseMass() * (this->m_TargetAng - currAng);
                    this->m_pBody->applyRelativeAngularImpulse(dAngularMomentum);
                }
                else
                {
                    this->m_pBody->applyRelativeTorque(f, a_DeltaTime);
                }
            }

            //this->m_Transform = this->m_pBody->getTransform();
            this->m_TargetVec = ong::vec3(0.0f, 0.0f, this->m_MaxSpeed/5.0f);
            this->m_TargetAng = ong::vec3(0.0f, 0.0f, 0.0f);

            ong::Transform t = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));

            //draw particles
        }



        //Add emitter
        std::vector<TheBrick::CBrickInstance**> engines;
        this->GetEngines(engines);
        unsigned int eID = 0;
        float amount = TheBrick::OngToPuRe(currVel).Length() / this->m_MaxSpeed;

		//remove unused emitter from engine that were destroyed
		while (engines.size() < m_EngineEmitter.size())
		{
			delete m_EngineEmitter.back();
			m_EngineEmitter.pop_back();
		}

        for (std::vector<TheBrick::CBrickInstance**>::iterator it = engines.begin(); it != engines.end(); ++it)
        {
            PuRe_ParticleEmitter* emitter = this->m_EngineEmitter[eID];
            if (this->m_Respawn == 0.0f)
			{
                TheBrick::CBrickInstance* engine = *(*it);
                PuRe_Vector3F pos = engine->PosToWorldSpace(engine->GetCenter());
                //TheBrick::OngToPuRe(this->m_pBody->getOrientation())
                emitter->m_Position = pos;
                emitter->m_Rotation = TheBrick::OngToPuRe(this->m_pBody->getOrientation());

                if (emitter->GetAmount() < (96 * amount) + 4)
                {
                    pos = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
                    pos.X += (std::rand() % 100) / 1000.0f;
                    pos.Y += (std::rand() % 100) / 1000.0f;
                    if (engine->m_pBrick->GetBrickId() != 702)
                    {
                        pos.X += 0.20f;
                        pos.Y += 0.20f;
                    }
                    pos.Z += -3.0f;

                    PuRe_Vector3F size = PuRe_Vector3F(1.5f, 1.5f, 1.5f);
                    PuRe_Vector3F velocity = PuRe_Vector3F(0.0f, 0.0f, (-0.001f*amount) - 0.0005f);
                    PuRe_Color color;
                    color.R = 1.0f;
                    color.B = 1.0f;
                    color.G = 1.0f;
                    emitter->Spawn(0.2f, pos, size, velocity, emitter->m_Rotation, color);
                }
            }
            emitter->Update(a_DeltaTime);
            eID++;
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::DrawEmitter(PuRe_Sprite* a_pSprite, PuRe_IMaterial* a_pMaterial, PuRe_PointLight* a_pLight, PuRe_IMaterial* a_pLightMaterial)
    {
        for (unsigned int i = 0; i<this->m_EngineEmitter.size(); i++)
        {
            sba_Renderer->SetCulling(0,false);
            sba_Renderer->Draw(0, true, this->m_EngineEmitter[i], a_pMaterial, a_pSprite,-1,0.2f);
            PuRe_Vector3F emitterpos = this->m_EngineEmitter[i]->m_Position + PuRe_Vector3F(0.0f,0.0f,-3.0f)*this->m_EngineEmitter[i]->m_Rotation;
            sba_Renderer->Draw(0, false, a_pLight, a_pLightMaterial, emitterpos, PuRe_Color(0.25f, 0.5f, 1.0f, 1.0f), 4.0f, 10.0f);
            sba_Renderer->SetCulling(0, true);
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Deserialize(TheBrick::CSerializer& a_pSerializer, TheBrick::BrickArray& a_rBricks, ong::World& a_pWorld)
    {
		CDestructibleObject::Deserialize(a_pSerializer, a_rBricks, a_pWorld);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Serialize(TheBrick::CSerializer& a_pSerializer)
    {
		CDestructibleObject::Serialize(a_pSerializer);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Reset(TheBrick::CBrick& a_rStartBrick, ong::World& a_rWorld)
    {
        this->Reset();
        TheBrick::CBrickInstance* brickInstance = a_rStartBrick.CreateInstance(*this, a_rWorld);
        brickInstance->SetTransform(ong::Transform(ong::vec3(0, 0, 0), ong::Quaternion(ong::vec3(0, 0, 0), 1)));
        brickInstance->RotateAroundPivotOffset(PuRe_QuaternionF(0.0f, 0.0f, 0.0f));
        brickInstance->m_Color = PuRe_Color(0, 0, 1);
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::Reset()
    {
        for (size_t i = 0; i < this->m_pBricks.size(); i++)
        {
            SAFE_DELETE(this->m_pBricks[i]);
            i--;
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::GetWeapons(std::vector<TheBrick::CBrickInstance**>& a_rOutVector)
    {
        for (std::vector<TheBrick::CBrickInstance*>::iterator it = this->m_pBricks.begin(); it != this->m_pBricks.end(); ++it)
        {
            if ((*it)->m_pBrick->GetCategoryId() == Editor::CBrickCategory::CATEGORY_WEAPONS)
            {
                a_rOutVector.push_back(&(*it));
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::GetEngines(std::vector<TheBrick::CBrickInstance**>& a_rOutVector)
    {
        for (std::vector<TheBrick::CBrickInstance*>::iterator it = this->m_pBricks.begin(); it != this->m_pBricks.end(); ++it)
        {
            if ((*it)->m_pBrick->GetCategoryId() == Editor::CBrickCategory::CATEGORY_ENGINES)
            {
                a_rOutVector.push_back(&(*it));
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CSpaceship::GetCockpits(std::vector<TheBrick::CBrickInstance**>& a_rOutVector)
    {
        for (std::vector<TheBrick::CBrickInstance*>::iterator it = this->m_pBricks.begin(); it != this->m_pBricks.end(); ++it)
        {
            if ((*it)->m_pBrick->GetCategoryId() == Editor::CBrickCategory::CATEGORY_COCKPITS)
            {
                a_rOutVector.push_back(&(*it));
            }
        }
    }
}