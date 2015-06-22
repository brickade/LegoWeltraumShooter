#include "include/BrickManager.h"

#include "include/Space.h"

namespace sba
{
    // **************************************************************************
    // **************************************************************************
    CBrickManager::CBrickManager(const char* a_pFolder)
    {
        this->m_FolderPath = a_pFolder;
    }

    // **************************************************************************
    // **************************************************************************
    CBrickManager::~CBrickManager()
    {

    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Initialize()
    {
        this->m_pBrickMaterial = sba_Application->GetGraphics()->LoadMaterial("../data/effects/instanced/default");
        this->m_pBrickUIMaterial = sba_Application->GetGraphics()->LoadMaterial("../data/effects/editor/default");
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Load()
    {
        TheBrick::CSerializer* serializer = new TheBrick::CSerializer();
        int i = 0;
        std::string file = sba_Application->GetWindow()->GetFileAtIndex(i, this->m_FolderPath.c_str());
        while (file != "")
        {
            TheBrick::CBrick* brick = new TheBrick::CBrick(*this->GetBrickMaterial());
            file.insert(0, this->m_FolderPath);
            
            serializer->OpenRead(file.c_str());
            brick->Deserialize(*serializer, *sba_Application->GetGraphics(), *sba_World);
            serializer->Close();
            
            this->m_bricks[brick->GetBrickId()] = brick;
            
            i++;
            file = sba_Application->GetWindow()->GetFileAtIndex(i, this->m_FolderPath.c_str());
        }
        delete serializer;
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::Render()
    {
        for (TheBrick::BrickArray::iterator it = this->m_bricks.begin(); it != this->m_bricks.end(); ++it)
        {
            if (*it != nullptr)
            {
                (*it)->Draw(*sba_Renderer);
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    void CBrickManager::RebuildRenderInstances()
    {
        for (TheBrick::BrickArray::iterator it = this->m_bricks.begin(); it != this->m_bricks.end(); ++it)
        {
            if (*it != nullptr)
            {
                (*it)->RebuildRenderInstances();
            }
        }
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrick& CBrickManager::GetBrick(size_t a_BrickId)
    {
        #ifdef _DEBUG
            try //Bounds checking
            {
                return *this->m_bricks.at(a_BrickId);
            }
            catch (std::out_of_range)
            {
                assert(false);
            }
        #else
            return *this->m_bricks[a_BrickId];
        #endif
    }

    // **************************************************************************
    // **************************************************************************
    TheBrick::CBrick** CBrickManager::GetCategoryStart(int a_CategoryId)
    {
#ifdef _DEBUG
            try //Bounds checking
            {
                return &this->m_bricks.at(a_CategoryId * 100);
            }
            catch (std::out_of_range)
            {
                assert(false);
            }
        #else
            return &this->m_bricks[a_CategoryId * 100];
        #endif
    }

    // **************************************************************************
    // **************************************************************************
    int CBrickManager::GetCategoryCount()
    {
        return (int)ceil(this->m_bricks.max_size() / 100.0f);
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_IMaterial* CBrickManager::GetBrickMaterial()
    {
#ifdef _DEBUG
            if (this->m_pBrickMaterial == nullptr)
            {
                printf("%s\n", "CBrickManager::GetBrickMaterial() was called before initializing the BrickManager!!!");       
            }
        #endif
        return this->m_pBrickMaterial;
    }

    // **************************************************************************
    // **************************************************************************
    PuRe_IMaterial* CBrickManager::GetBrickUIMaterial()
    {
#ifdef _DEBUG
        if (this->m_pBrickUIMaterial == nullptr)
            {
                printf("%s\n", "CBrickManager::GetBrickUIMaterial() was called before initializing the BrickManager!!!");
            }
        #endif
        return this->m_pBrickUIMaterial;
    }
}