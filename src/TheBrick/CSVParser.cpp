#include "include\TheBrick\CSVParser.h"

namespace TheBrick
{
    CCSVParser::CCSVParser(const char* a_pFile)
    {
        FILE* pFile;
        long size = 0;
        char* buffer = nullptr;
        bool readKey = true;
        std::string Key;
        std::string Value;

        //open file
        pFile = fopen(a_pFile, "rb");
        if (pFile != NULL)
        {
            //Get file size
            fseek(pFile, 0, SEEK_END);
            size = ftell(pFile);
            rewind(pFile);

            //allocate buffer and copy file data into it
            buffer = (char*)malloc(sizeof(char)*size);
            fread(buffer, 1, size, pFile);
            //close file
            fclose(pFile);

            //handle buffer
            for (int i = 0; i < size; i++)
            {
                if (buffer[i] == ';')
                    readKey = false;
                else if (buffer[i] == '\r')
                {
                    i++;
                    //insert
                    if (Key != ""&&Value != "")
                        this->m_Table[Key] = Value;
                    //reset strings
                    Key.clear();
                    Value.clear();

                    readKey = true;
                }
                else
                {
                    if (readKey)
                        Key += buffer[i];
                    else
                        Value += buffer[i];
                }
            }
            //deallocate buffer
            free(buffer);
        }
    }

    CCSVParser::~CCSVParser()
    {
    }

    std::string CCSVParser::GetValue(std::string a_Key)
    {
        return this->m_Table[a_Key];
    }

}