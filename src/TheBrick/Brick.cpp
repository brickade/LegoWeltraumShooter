#include "include/TheBrick/Brick.h"
namespace TheBrick
{
    const float CBrick::SEGMENT_WIDTH = 0.39f;
    const float CBrick::SEGMENT_HEIGHT = 0.1f;

    // **************************************************************************
    // **************************************************************************
    CBrick::CBrick(PuRe_Model* a_pModel)
    {
        this->m_pModel = a_pModel;
    }

    // **************************************************************************
    // **************************************************************************
    CBrick::~CBrick()
    {
        SAFE_DELETE(this->m_pModel);
    }

    // **************************************************************************
    // **************************************************************************
    CBrick* CBrick::FromFile(PuRe_IGraphics* a_pGraphics, const char* a_pPath)
    {
        PuRe_IMaterial* material = a_pGraphics->LoadMaterial("../data/effects/editor/default");
        PuRe_Model* model = new PuRe_Model(a_pGraphics, material, a_pPath);
        return new CBrick(model);
    }

    // **************************************************************************
    // **************************************************************************
    CBrickInstance* CBrick::CreateInstance()
    {
        return new CBrickInstance(this);
    }

    // **************************************************************************
    // **************************************************************************
    void CBrick::Draw(PuRe_IGraphics* a_pGraphics, PuRe_Camera* a_pCamera, PuRe_Vector3F a_position, PuRe_Vector3F a_Scale, PuRe_Vector3F a_rotation)
    {
        this->m_pModel->Draw(a_pCamera, PuRe_Primitive::Triangles, a_position, a_Scale, a_rotation, PuRe_Vector3F(0, 0, 0));
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_List<SNub>* CBrick::GetNubs()
    {
        return &this->m_pNubs;
    }

    // **************************************************************************
    // **************************************************************************
    static CBrick* Deserialize(CSerializer* a_pSerializer, PuRe_IGraphics* a_pGraphics, PuRe_IMaterial* a_pMaterial)
    {
        //m_pModel
        char* buffer = (char*)malloc(100);
        a_pSerializer->Read(buffer, 100);
        CBrick* brick = new CBrick(new PuRe_Model(a_pGraphics, a_pMaterial, buffer));

        //m_BrickId
        brick->m_BrickId = a_pSerializer->ReadIntUnsigned();

        //m_pNubs
        for (int i = 0; i < a_pSerializer->ReadInt(); i++)
        {
            SNub nub;
            a_pSerializer->Read(&nub, sizeof(nub));
            brick->m_pNubs.push_back(nub);
        }

        //m_pColliderData
        for (int i = 0; i < a_pSerializer->ReadInt(); i++)
        {
            ong::ColliderData colliderData;
            //transform
            a_pSerializer->Read(&colliderData.transform, sizeof(colliderData.transform));
            //pMaterial

            //massData
            a_pSerializer->Read(&colliderData.massData, sizeof(colliderData.massData));
            //aabb
            a_pSerializer->Read(&colliderData.aabb, sizeof(colliderData.aabb));
            //shape
            ong::ShapeType::Type stype = (ong::ShapeType::Type)a_pSerializer->ReadInt(); //m_type
            switch (stype) //m_shape
            {
            case ong::ShapeType::SPHERE:
                colliderData.shape = new ong::Sphere();
                a_pSerializer->Read(&colliderData.shape, sizeof(colliderData.shape));
                break;
            case ong::ShapeType::CAPSULE:
                colliderData.shape = new ong::Capsule();
                a_pSerializer->Read(&colliderData.shape, sizeof(colliderData.shape));
                break;
            case ong::ShapeType::HULL:
                colliderData.shape = new ong::Hull();
                colliderData.shape.toHull()->centroid = a_pSerializer->ReadVector3();
                //Vertices
                colliderData.shape.toHull()->numVertices = a_pSerializer->ReadInt();
                for (int t = 0; t < colliderData.shape.toHull()->numVertices; t++)
                {
                    *(colliderData.shape.toHull()->pVertices + t) = a_pSerializer->ReadVector3();
                }
                //Edges
                colliderData.shape.toHull()->numEdges = a_pSerializer->ReadInt();
                for (int t = 0; t < colliderData.shape.toHull()->numEdges; t++)
                {
                    ong::HalfEdge* edge = (colliderData.shape.toHull()->pEdges + t);
                    a_pSerializer->Read(&edge, sizeof(edge));
                }
                //Faces
                colliderData.shape.toHull()->numFaces = a_pSerializer->ReadInt();
                for (int t = 0; t < colliderData.shape.toHull()->numFaces; t++)
                {
                    ong::Face* face = (colliderData.shape.toHull()->pFaces + t);
                    a_pSerializer->Read(&face, sizeof(face));
                    ong::Plane* plane = (colliderData.shape.toHull()->pPlanes + t);
                    a_pSerializer->Read(&plane, sizeof(plane));
                }
                //epsilon
                colliderData.shape.toHull()->epsilon = a_pSerializer->ReadFloat();
                break;
            }
            brick->m_pColliderData.push_back(colliderData);
        }

        return brick;
    }

    // **************************************************************************
    // **************************************************************************
    void CBrick::Serialize(CSerializer* a_pSerializer)
    {
        
    }
}