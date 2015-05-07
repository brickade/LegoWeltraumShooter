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
    void CBrickManager::Load(const char* a_pFolder)
    {
        
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