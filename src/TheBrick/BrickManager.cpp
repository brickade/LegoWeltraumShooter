#include "include/TheBrick/BrickManager.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CBrickManager::CBrickManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    CBrickManager::~CBrickManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Initialize(PuRe_IGraphics* a_pGraphics, PuRe_SoundPlayer* a_pSoundPlayer)
    {
        this->m_pBrickMaterial = a_pGraphics->LoadMaterial("../data/effects/editor/default");
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Load(PuRe_IGraphics* a_pGraphics, ong::World* a_pWorld, PuRe_IMaterial* a_pMaterial, const char* a_pFolder)
    {
        char p[300];
        memset(p, 0, 300);
        strcat(p, a_pFolder);
        strcat(p, "Brick1X6.brick");
        CSerializer* serializer = new CSerializer();
        serializer->OpenRead(p);
        this->m_bricks[0] = new CBrick(a_pMaterial);
        this->m_bricks[0]->Deserialize(serializer, a_pGraphics, a_pWorld);
        serializer->Close();

        memset(p, 0, 300);
        strcat(p, a_pFolder);
        strcat(p, "Brick1X8.brick");
        serializer->OpenRead(p);
        this->m_bricks[1] = new CBrick(a_pMaterial);
        this->m_bricks[1]->Deserialize(serializer, a_pGraphics, a_pWorld);
        serializer->Close();
        
        delete serializer;
    }

    // **************************************************************************
    // **************************************************************************
    CBrick* CBrickManager::GetBrick(int a_BrickId)
    {
        #ifdef DEBUG
            try //Bounds checking
            {
                return this->m_bricks.at(a_BrickId);
            }
            catch (std::out_of_range)
            {
                return nullptr;
            }
        #else
            return this->m_bricks[a_BrickId];
        #endif
    }

    // **************************************************************************
    // **************************************************************************
    CBrick** CBrickManager::GetCategoryStart(int a_CategoryId)
    {
        #ifdef DEBUG
            try //Bounds checking
            {
                return this->m_bricks.at(a_CategoryId * 100);
            }
            catch (std::out_of_range)
            {
                return nullptr;
            }
        #else
            return &this->m_bricks[a_CategoryId * 100];
        #endif
    }

    // **************************************************************************
    // **************************************************************************
    int CBrickManager::GetCategoryCount()
    {
        return ceil(this->m_bricks.max_size() / 100.0f);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_IMaterial* CBrickManager::GetBrickMaterial()
    {
        #ifdef DEBUG
            if (this->m_pBrickMaterial == nullptr)
            {
                print("CBrickManager::GetBrickMaterial() was called before initializing the BrickManager!!!");       
            }
        #endif
        return this->m_pBrickMaterial;
    }
}