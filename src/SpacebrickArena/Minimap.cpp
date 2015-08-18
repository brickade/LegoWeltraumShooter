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
    void CMinimap::Draw(CGameCamera* a_pCamera, sba::CSpriteReader* a_pUI, PuRe_IMaterial* a_pMaterial, int a_Player, int a_Target, float a_OriginDistance)
    {
        //center
        ong::vec3 playerpos = sba_Players[a_Player]->Ship->m_pBody->getWorldCenter();
        ong::Quaternion playerrot = sba_Players[a_Player]->Ship->m_pBody->getOrientation();

        PuRe_QuaternionF rot = a_pCamera->GetQuaternion();
        PuRe_QuaternionF urot = PuRe_QuaternionF(PuRe_Vector3F(85.0f*PuRe_DegToRad, 0.0f, 0.0f));
        PuRe_Vector3F cforw = a_pCamera->GetForward();
        PuRe_Vector3F cside = a_pCamera->GetSide();
        PuRe_Vector3F cup = a_pCamera->GetUp();
        rot *= urot;



        PuRe_Vector3F pos = a_pCamera->GetPosition();
        pos += cforw;
        pos -= cup*0.3f;
        pos -= cforw*0.01f;

        PuRe_Vector3F epos, ediff;
        playerrot.v = -playerrot.v;
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            if (i != a_Player)
            {
                ong::vec3 diff = playerpos - sba_Players[i]->Ship->m_pBody->getWorldCenter();
                //Normalize
                diff.x /= a_OriginDistance;
                diff.y /= a_OriginDistance;
                diff.z /= a_OriginDistance;
                diff = ong::rotate(ong::vec3(diff.x,diff.y,diff.z), playerrot);

                ediff = (PuRe_Vector3F(-diff.x,-diff.z,diff.y) * rot);

                epos = pos + ediff*0.1f;



                //a_pUI->Draw(1, sba_Renderer, "minimap_enemy", a_pMaterial, epos, rot.GetMatrix(), a_Target, PuRe_Vector2F(1.0f, diff.y*10.0f));
                sba_Renderer->Set(1, 0.0f, "verlauf");
                a_pUI->Draw(1, sba_Renderer, "minimap_enemy", a_pMaterial, epos, rot.GetMatrix(), a_Target, PuRe_Vector2F(0.001f, 0.001f));

            }
        }
        pos += cforw*0.01f;
        sba_Renderer->Set(1, 0.0f, "verlauf");
        a_pUI->Draw(1, sba_Renderer, "minimap_ground", a_pMaterial, pos, rot.GetMatrix(), a_Target, PuRe_Vector2F(0.0005f, 0.0005f));
        pos -= cforw*0.01f;
        sba_Renderer->Set(1, 0.0f, "verlauf");
        a_pUI->Draw(1, sba_Renderer, "minimap_player", a_pMaterial, pos, rot.GetMatrix(), a_Target, PuRe_Vector2F(0.001f, 0.001f));
        pos += (PuRe_Vector3F(0.0f, 0.0f, -0.01f) * rot);
        sba_Renderer->Set(1, 0.0f, "verlauf");
        a_pUI->Draw(1, sba_Renderer, "minimap_parrow", a_pMaterial, pos, rot.GetMatrix(), a_Target, PuRe_Vector2F(0.001f, 0.001f));
        pos -= (PuRe_Vector3F(0.0f, 0.0f, -0.01f) * rot);

    }

}