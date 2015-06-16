#include "include/Menu_Lobby.h"

namespace Menu
{

    CLobby::CLobby()
    {
        this->m_pNavigation = new sba::CNavigation(1, 1);
    }

    CLobby::~CLobby()
    {
        SAFE_DELETE(this->m_pNavigation);
    }

    int CLobby::Update(PuRe_Timer* a_pTimer, PuRe_IWindow* a_pWindow, PuRe_IInput* a_pInput, int a_PlayerIdx)
    {
        this->m_pNavigation->Update(*a_pTimer, sba_Input->Direction(sba_Direction::MenuMove, a_PlayerIdx));
        if (sba_Input->ButtonPressed(sba_Button::MenuClick, a_PlayerIdx))
        {
            switch (this->m_pNavigation->GetFocusedElementId())
            {
            case 0: //Start
                return 2;
                break;
            case 1: //Back
                return 0;
                break;
            }
        }

        for (int i = 1; i < 4; i++)
        {
            if (a_pInput->GamepadPressed(a_pInput->Pad_A, i))
            {
                bool exist = false;
                for (unsigned j = 0; j<sba_Players.size(); j++)
                {
                    if (sba_Players[j]->PadID == i)
                    {
                        exist = true;
                        break;
                    }
                }
                if (!exist)
                    sba_Space->CreatePlayer(i, a_pWindow);
            }
            else if (a_pInput->GamepadPressed(a_pInput->Pad_B, i))
            {
                for (unsigned j = 0; j<sba_Players.size(); j++)
                {
                    if (sba_Players[j]->PadID == i)
                    {
                        sba_Space->DeletePlayer(j);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < 4; i++)
        {
            bool leftPress = a_pInput->GamepadPressed(a_pInput->DPAD_Left, i);
            bool rightPress = a_pInput->GamepadPressed(a_pInput->DPAD_Right, i);
            if (leftPress || rightPress)
            {
                for (unsigned j = 0; j<sba_Players.size(); j++)
                {
                    if (sba_Players[j]->PadID == i)
                    {
                        sba::Player* p = sba_Players[j];
                        int s = 0;
                        const char* path = "../data/ships/";
                        bool right = false;

                        std::string file = a_pWindow->GetFileAtIndex(s, path);
                        std::string lastFile = file;
                        std::string working = file;
                        bool found = false;
                        //aslong as the file is not right
                        while (!right)
                        {
                            if (!found&&file.substr(0, file.find_last_of(".")) == p->Ship->GetName())
                            { 
                                //if he has not been found and the file is the current one
                                working = file;
                                found = true; //current one found
                            }
                            else if (found)
                            {
                                //if he has been found
                                if (file.substr(file.find_last_of(".") + 1) == "ship")
                                {
                                    //this is a ship, take it!
                                    working = file;
                                    break;
                                }
                            }
                            if (!found||rightPress)
                                s++;
                            else if (leftPress)
                                s--;
                            if (s >= 0)
                                file = a_pWindow->GetFileAtIndex(s, path);
                            if (s < 0||lastFile == file)
                            {
                                file = working;
                                break;
                            }
                            else
                                lastFile = file;
                        }

                        std::string name = file.substr(0, file.find_last_of("."));

                        file = path + file;
                        TheBrick::CSerializer serializer;
                        serializer.OpenRead(file.c_str());
                        ong::vec3 pos = ong::vec3(0.0f, 0.0f, 0.0f);
                        SAFE_DELETE(p->Ship);
                        p->Ship = new TheBrick::CSpaceship(*sba_World, name);
                        p->Ship->Deserialize(serializer, *sba_BrickManager, *sba_World);
                        serializer.Close();
                        break;
                    }
                }
            }
        }


        return 1;
    }

    void CLobby::Reset()
    {

    }

    void CLobby::Render(PuRe_Renderer* a_pRenderer,PuRe_Timer* a_pTimer, PuRe_Font* a_pFont, PuRe_IMaterial* a_pFontMaterial, PuRe_Vector2F a_Resolution)
    {
        PuRe_Vector3F Position;
        Position.X = 100.0f;
        Position.Y = 1080.0f;
        Position.Y -= 100.0f;

        PuRe_Color color = PuRe_Color();

        if (this->m_pNavigation->GetFocusedElementId() == 0)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Start", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);

        if (this->m_pNavigation->GetFocusedElementId() == 1)
            color = PuRe_Color(1.0f, 0.0f, 0.0f);
        else
            color = PuRe_Color(1.0f, 1.0f, 1.0f);
        Position.Y = 100.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Back", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(36.0f, 36.0f, 0.0f), 32.0f, color);

        Position.X = 1920.0f - 600.0f;
        Position.Y = 1080.0f - 200.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Player", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
        Position.X += 300.0f;
        a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Ship", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
        Position.Y -= 64.0f;
        Position.X -= 300.0f;
        color = PuRe_Color(1.0f, 1.0f, 1.0f);
        for (unsigned int i = 0; i < sba_Players.size(); i++)
        {
            std::string name = "Player " + std::to_string(sba_Players[i]->ID);
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, name.c_str(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
            Position.X += 300.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, sba_Players[i]->Ship->GetName(), Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(26.0f, 26.0f, 0.0f), 24.0f, color);
            Position.Y -= 64.0f;
            Position.X -= 300.0f;
        }
        for (unsigned int i = 0; i < 4-sba_Players.size(); i++)
        {
            PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f);
            color.A = ((sin(a_pTimer->GetTotalElapsedSeconds()*2.0f))+0.8f)/2.0f;
            a_pRenderer->Draw(1, false, a_pFont, a_pFontMaterial, "Press A to join ...", Position, PuRe_MatrixF::Identity(), PuRe_Vector3F(22.0f, 22.0f, 0.0f), 20.0f, color);
            Position.Y -= 64.0f;
        }
    }
}