#include "include/SpriteReader.h"

namespace sba
{
    CSpriteReader::CSpriteReader(PuRe_IGraphics* a_pGraphics, const char* a_pSpritePath, const char* a_pTextPath)
    {
        this->m_pSprite = new PuRe_Sprite(a_pGraphics, a_pSpritePath);

        //Now read the Sprite Textfile
        FILE* pfile;
        char* pbuffer;
        long lSize;

        fopen_s(&pfile, a_pTextPath, "r");

        if (pfile == NULL)
            printf("File does not exist %s!\n", a_pTextPath);

        // obtain file size:
        fseek(pfile, 0, SEEK_END);
        lSize = ftell(pfile);
        rewind(pfile);

        // allocate memory to contain the whole file:
        pbuffer = (char*)malloc(sizeof(char)*lSize);

        // copy the file into the buffer:
        fread(pbuffer, 1, lSize, pfile);


        int textFilePosition = 0;
        int lastTextFilePosition = 0;
        char readChar = pbuffer[0];
        int readWhat = 0;

        SSpriteData spriteData;
        std::string spriteText;
        char buffer[64] = "";

        //we read our Buffer and set our map
        while (readChar != '\0')
        {
            readChar = pbuffer[textFilePosition];
            int size = textFilePosition - lastTextFilePosition;
            if (readChar == '\n')
            {
                //set memory
                memset(buffer, 0, 64);
                memcpy(buffer, &pbuffer[lastTextFilePosition], size);
                //set our data to our buffer
                spriteData.Size.Y = (float)(atoi(buffer));
                //free memory
                //set what to read next and where to start
                readWhat = 0;
                lastTextFilePosition = textFilePosition + 1;
                //save our Data
                this->m_Reader[spriteText] = spriteData;
            }
            else if (readChar == ' ')
            {
                //set memory
                memset(buffer, 0, 64);
                memcpy(buffer, &pbuffer[lastTextFilePosition], size);
                //set our data to our buffer
                switch (readWhat)
                {
                case 0:
                    spriteText = std::string(buffer, 0, size);
                    break;
                case 2:
                    spriteData.Position.X = (float)(atoi(buffer));
                    break;
                case 3:
                    spriteData.Position.Y = (float)(atoi(buffer));
                    break;
                case 4:
                    spriteData.Size.X = (float)(atoi(buffer));
                    break;
                };
                //set what to read next
                readWhat++;
                lastTextFilePosition = textFilePosition + 1;
            }
            textFilePosition++;
        }


        // terminate
        fclose(pfile);
        free(pbuffer);
    }

    // **************************************************************************
    // **************************************************************************

    CSpriteReader::~CSpriteReader()
    {
        SAFE_DELETE(this->m_pSprite);
    }

    // **************************************************************************
    // **************************************************************************

    void CSpriteReader::Draw(int a_Index, PuRe_Renderer* a_pRenderer, const char* a_pName, PuRe_IMaterial* a_pMaterial, PuRe_Vector3F a_Position, PuRe_Vector3F a_Rotation, int a_RenderIndex, PuRe_Vector2F Size, PuRe_Vector2F UVSize)
    {

        auto got = m_Reader.find(a_pName);

        PuRe_Vector3F origin = PuRe_Vector3F(0.0f, 0.0f, 0.0f);
        //1.0f = real size, so 128x128
        //PuRe_Vector3F size = PuRe_Vector3F(got->second.Size.X*Size.X, got->second.Size.Y*Size.Y, 1.0f);
        PuRe_Vector3F size = PuRe_Vector3F(0.0f,0.0f,1.0f);
        PuRe_Vector2F gsize = this->GetSize();
        size.X = (got->second.Size.X / gsize.X) * Size.X;
        size.Y = (got->second.Size.Y / gsize.Y) * Size.Y;
        PuRe_Vector2F uvsize = PuRe_Vector2F(got->second.Size.X*UVSize.X, got->second.Size.Y*UVSize.Y);
        PuRe_Color color = PuRe_Color(1.0f, 1.0f, 1.0f, 1.0f);
        a_pRenderer->Draw(a_Index, false, this->m_pSprite, a_pMaterial, a_Position, a_Rotation, origin, size, color, uvsize, got->second.Position,a_RenderIndex);

        //this->m_pSprite->Draw(a_pCamera, a_pMaterial, got->second.Position, PuRe_Vector2F(got->second.Size.X*UVSize.X, got->second.Size.Y*UVSize.Y), a_Position, PuRe_Vector3F(got->second.Size.X*Size.X, got->second.Size.Y*Size.Y, 1.0f), a_Rotation, PuRe_Vector3F(0.0f, 0.0f, 0.0f));
    }

    // **************************************************************************
    // **************************************************************************

    PuRe_Vector2F CSpriteReader::GetSize()
    {
        return this->m_pSprite->GetSize();
    }
}