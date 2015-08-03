#include "include/Minimap.h"

#include "include/Space.h"

namespace sba
{
    CMinimap::CMinimap(PuRe_IGraphics* a_pGraphics)
    {
    }

    // **************************************************************************
    // **************************************************************************
    CMinimap::~CMinimap()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CMinimap::Draw(sba::CSpriteReader* a_pUI, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, int a_Player, int a_Target, float a_OriginDistance)
    {
        PuRe_Vector3F rot = PuRe_Vector3F(70.0f*PuRe_DegToRad, 0.0f, 0.0f);

        //center
        ong::vec3 playerpos = sba_Players[a_Player]->Ship->m_pBody->getWorldCenter();
        ong::Quaternion playerrot = sba_Players[a_Player]->Ship->m_pBody->getOrientation();
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (i != a_Player)
            {
                ong::vec3 epos = sba_Players[i]->Ship->m_pBody->getWorldCenter();
                ong::vec3 diff = playerpos - epos;
                //Normalize
                diff.x /= a_OriginDistance;
                diff.y /= a_OriginDistance;
                diff.z /= a_OriginDistance;



                diff = ong::rotate(diff, playerrot);

                PuRe_Vector3F pos;
                pos.X = diff.x*64.0f;
                pos.Y = -diff.z*64.0f;
                pos.Z = diff.y*64.0f;

                pos += a_Position;
                a_pUI->Draw(2, sba_Renderer, "minimap_enemy", a_pMaterial, pos, PuRe_Vector3F(), a_Target, PuRe_Vector2F(1.0f, 1.0f));

            }
        }

        a_pUI->Draw(2, sba_Renderer, "minimap_ground", a_pMaterial, a_Position, rot, a_Target, PuRe_Vector2F(0.5f, 0.5f));
        a_pUI->Draw(2, sba_Renderer, "minimap_player", a_pMaterial, a_Position, rot, a_Target, PuRe_Vector2F(1.0f, 1.0f));
        a_Position.Y += 32.0f;
        a_pUI->Draw(2, sba_Renderer, "minimap_parrow", a_pMaterial, a_Position, rot, a_Target, PuRe_Vector2F(1.0f, 1.0f));
        a_Position.Y -= 32.0f;

    }

}