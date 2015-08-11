#include "include/GameMap.h"

#include "include/Space.h"

namespace sba
{

    // **************************************************************************
    // **************************************************************************
    CGameMap::CGameMap()
    {
        this->m_Name = "eridanus";
    }

    // **************************************************************************
    // **************************************************************************
    CGameMap::~CGameMap()
    {
    }

    // **************************************************************************
    // **************************************************************************
    void CGameMap::SetMap(const char* a_pDirectory)
    {
        this->m_Name = a_pDirectory;
    }

    // **************************************************************************
    // **************************************************************************
    std::string CGameMap::GetName()
    {
        return this->m_Name;
    }

    // **************************************************************************
    // **************************************************************************
    std::string CGameMap::GetSkybox()
    {
        return "../data/textures/skybox/" + this->m_Name + "/";
    }

    // **************************************************************************
    // **************************************************************************
    bool CGameMap::GetMapData(std::vector<CAsteroid*>& a_rObjects, std::vector<CItem*>& a_rItems, std::vector<SLightData*>& a_rLights)
    {
        FILE* pFile;
        std::string mapPath = "../data/maps/" + this->m_Name + ".map";
        pFile = fopen(mapPath.c_str(), "r");

        if (pFile != NULL)
        {
            long size = 0;
            char* buffer = nullptr;
            //Get file size
            fseek(pFile, 0, SEEK_END);
            size = ftell(pFile);
            rewind(pFile);

            //allocate buffer and copy file data into it
            buffer = (char*)malloc(sizeof(char)*size);
            memset(buffer,0,size);
            fread(buffer, 1, size, pFile);
            //close file
            fclose(pFile);

            enum read { Name, Pos, Vel, Rot };
            enum readC { One, Two };
            enum Type { Object, Item, Light,Static };

            std::string objectName;
            ong::vec3 pos, vel, rot;
            Type otype;

            std::string buff = "";
            read readWhere = Name;
            readC readCount = One;
            ong::vec3* changed = NULL;
            for (int i = 0; i <= size; i++)
            {
                if (i == size||buffer[i] == '\n')
                {
                    changed->z = std::stof(buff);
                    if (objectName != "")
                    {
                        if (otype == Type::Object)
                        {
                            ong::BodyDescription desc;
                            desc.type = ong::BodyType::Dynamic;
                            desc.transform = ong::Transform(pos, ong::Quaternion(ong::vec3(0, 0, 0), 1));
                            desc.linearMomentum = vel;
                            desc.angularMomentum = rot;
							desc.continuousPhysics = false;
                            sba::CAsteroid* asteroid = new sba::CAsteroid(*sba_World, &desc);
                            TheBrick::CSerializer serializer;
                            serializer.OpenRead(objectName.c_str());
                            asteroid->Deserialize(serializer, sba_BrickManager->GetBrickArray(), *sba_World);
                            a_rObjects.push_back(asteroid);
                            serializer.Close();
                        }
                        else if (otype == Type::Static)
                        {
                            ong::BodyDescription desc;
                            desc.type = ong::BodyType::Static;
                            desc.transform = ong::Transform(pos, ong::Quaternion(ong::vec3(0, 0, 0), 1));
                            desc.linearMomentum = vel;
                            desc.angularMomentum = rot;
							desc.continuousPhysics = false;
                            sba::CAsteroid* asteroid = new sba::CAsteroid(*sba_World, &desc);
                            TheBrick::CSerializer serializer;
                            serializer.OpenRead(objectName.c_str());
                            asteroid->Deserialize(serializer, sba_BrickManager->GetBrickArray(), *sba_World);
                            a_rObjects.push_back(asteroid);
                            serializer.Close();
                        }
                        else if (otype == Type::Item)
                        {
                            EItemType itype;
                            if (objectName == "repair")
                                itype = EItemType::Repair;
                            else if (objectName == "shield")
                                itype = EItemType::Shield;
                            else if (objectName == "speed")
                                itype = EItemType::Speed;
                            sba::CItem* item = new sba::CItem(*sba_World, itype, pos, vel, rot);
                            a_rItems.push_back(item);
                        }
                        else if (otype == Type::Light)
                        {
                            SLightData* light = new SLightData();
                            if (objectName == "directional")
                                light->Type = ELightType::Directional;
                            else if (objectName == "point")
                                light->Type = ELightType::Point;
                            light->Position = TheBrick::OngToPuRe(pos);
                            light->Color = PuRe_Color(vel.x,vel.y,vel.z,1.0f);
                            a_rLights.push_back(light);
                        }
                    }
                    readWhere = read::Name;
                    readCount = readC::One;
                    changed = &pos;
                    objectName = "";
                    buff = "";
                }
                else if (buffer[i] == ';')
                {
                    switch (readWhere)
                    {
                    case read::Name:
                        if (buff.substr(0, buff.find_last_of("@")) == "object")
                        {
                            otype = Type::Object;
                            objectName = "../data/objects/" + buff.substr(buff.find_last_of("@") + 1) + ".object";
                        }
                        else if (buff.substr(0, buff.find_last_of("@")) == "static")
                        {
                            otype = Type::Static;
                            objectName = "../data/objects/" + buff.substr(buff.find_last_of("@") + 1) + ".object";
                        }
                        else if (buff.substr(0, buff.find_last_of("@")) == "item")
                        {
                            otype = Type::Item;
                            objectName = buff.substr(buff.find_last_of("@") + 1);
                        }
                        else if (buff.substr(0, buff.find_last_of("@")) == "light")
                        {
                            otype = Type::Light;
                            objectName = buff.substr(buff.find_last_of("@") + 1);
                        }
                        readWhere = read::Pos;
                        readCount = readC::One;
                        changed = &pos;
                        break;
                    case read::Pos:
                        changed->z = std::stof(buff);
                        readWhere = read::Vel;
                        readCount = readC::One;
                        changed = &vel;
                        break;
                    case read::Vel:
                        changed->z = std::stof(buff);
                        readWhere = read::Rot;
                        readCount = readC::One;
                        changed = &rot;
                        break;
                    }
                    buff = "";
                }
                else if (buffer[i] == ',')
                {
                    if (readWhere == read::Name)
                        buff += buffer[i];
                    else
                    {
                        switch (readCount)
                        {
                        case readC::One:
                            changed->x = std::stof(buff);
                            readCount = readC::Two;
                            break;
                        case readC::Two:
                            changed->y = std::stof(buff);
                            break;
                        }
                        buff = "";
                    }
                }
                else
                    buff += buffer[i];
            }
        }
        else
            return false;
        return true;
    }
}