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
    void CBrickManager::Load(PuRe_IGraphics* a_pGraphics, ong::World* a_pWorld, const char* a_pFolder)
    {
        char p[300];
        memset(p, 0, 300);
        strcat(p, a_pFolder);
        strcat(p, "Brick1X3.brick");
        CSerializer* serializer = new CSerializer();
        serializer->OpenRead(p);
        this->m_bricks[0] = new CBrick();
        this->m_bricks[0]->Deserialize(serializer, a_pGraphics, this->m_pBrickMaterial, a_pWorld);
        serializer->Close();

        memset(p, 0, 300);
        strcat(p, a_pFolder);
        strcat(p, "Brick1X4.brick");
        serializer->OpenRead(p);
        this->m_bricks[1] = new CBrick();
        this->m_bricks[1]->Deserialize(serializer, a_pGraphics, this->m_pBrickMaterial, a_pWorld);
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
}