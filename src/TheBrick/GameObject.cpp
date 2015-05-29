#include "include/TheBrick/GameObject.h"

#include "include/TheBrick/BrickInstance.h"
#include "include/TheBrick/Serializer.h"
#include "include/TheBrick/BrickManager.h"
#include "include/TheBrick/Brick.h"
#include "include/TheBrick/DebugDraw.h"
#include "include/TheBrick/Conversion.h"

namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CGameObject::CGameObject(ong::World& a_rWorld, ong::BodyDescription* a_pBodyDesc)
    {
        if (a_pBodyDesc == nullptr)
        {
            ong::BodyDescription body;
            body.transform = ong::Transform(ong::vec3(0.0f, 0.0f, 0.0f), ong::Quaternion(ong::vec3(0, 0, 0), 1));
            body.type = ong::BodyType::Dynamic;
            body.angularMomentum = ong::vec3(0, 0, 0);
            body.linearMomentum = ong::vec3(0, 0, 0);
            a_pBodyDesc = &body;
        }
        ong::Body* body = a_rWorld.createBody(*a_pBodyDesc);
        body->setUserData(this);
        this->m_pBody = body;
    }

    // **************************************************************************
    // **************************************************************************
    CGameObject::~CGameObject()
    {
        //Delete BrickInstances
        for (size_t i = 0; i < this->m_pBricks.size(); i++)
        {
            SAFE_DELETE(this->m_pBricks[i]);
        }
        this->m_pBody->getWorld()->destroyBody(this->m_pBody);
        this->m_pBody = nullptr;
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Update(float a_DeltaTime)
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera)
    {
        DrawBody(this->m_pBody, a_pCamera, a_pGraphics);
        return;
        for (auto&& brickinstance : this->m_pBricks)
        {
            std::vector<TheBrick::SNub>& nubs = brickinstance->m_pBrick->GetNubs();
            for (auto&& nub : nubs)
            {
                ong::Sphere s;
                s.r = 0.1f;
                s.c = TheBrick::PuReToOng(nub.Position);
                TheBrick::DrawShape(&s, ong::Transform(brickinstance->GetTransform()), PuRe_Vector3F(0, 1, 1), a_pCamera, a_pGraphics);
            }
        }
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::Deserialize(CSerializer& a_pSerializer, CBrickManager& a_pBrickManager, ong::World& a_pWorld)
    {
        //m_pBricks
        unsigned int bricksSize = a_pSerializer.ReadIntUnsigned();
        for (unsigned int i = 0; i < bricksSize; i++)
        {
            //m_pBrick
            int brickId = a_pSerializer.ReadInt();
            CBrickInstance* brick = new CBrickInstance(a_pBrickManager.GetBrick(brickId), *this, a_pWorld);
            //m_Transform
            ong::Transform transform;
            a_pSerializer.Read(&transform, sizeof(transform));
            brick->SetTransform(transform);
            //m_Color
            a_pSerializer.Read(&brick->m_Color, sizeof(*&brick->m_Color));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::Serialize(CSerializer& a_pSerializer)
    {
        //m_pBricks
        a_pSerializer.Write(this->m_pBricks.size());
        for (unsigned int i = 0; i < this->m_pBricks.size(); i++)
        {
            //m_pBrick
            CBrickInstance* brick = this->m_pBricks[i];
            a_pSerializer.Write(brick->m_pBrick->GetBrickId());
            //m_Transform
            a_pSerializer.Write(&brick->GetTransform(), sizeof(*&brick->GetTransform()));
            //m_Color
            a_pSerializer.Write(&brick->m_Color, sizeof(*&brick->m_Color));
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CGameObject::AddBrickInstance(CBrickInstance* a_pBrickInstance, ong::World& a_rWorld)
    {
        for (size_t i = 0; i < a_pBrickInstance->m_pCollider.size(); i++)
        {
            this->m_pBody->addCollider(a_pBrickInstance->m_pCollider[i]);
        }
        this->m_pBricks.push_back(a_pBrickInstance);
        a_pBrickInstance->SetGameObject(this);
    }


    // **************************************************************************
    // **************************************************************************
    void CGameObject::RemoveBrickInstance(const CBrickInstance& a_BrickInstance)
    {
        for (size_t i = 0; i < a_BrickInstance.m_pCollider.size(); i++)
        {
            this->m_pBody->removeCollider(a_BrickInstance.m_pCollider[i]);
        }
        for (std::vector<CBrickInstance*>::iterator it = this->m_pBricks.begin(); it != this->m_pBricks.end(); ++it)
        {
            if (*it == &a_BrickInstance)
            {
                this->m_pBricks.erase(it);
                break;
            }
        }
    }
}