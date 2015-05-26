#ifndef _CSVPARSER_H_
#define _CSVPARSER_H_

#include <string>
#include <unordered_map>
#include <stdio.h>

namespace TheBrick
{
    class CCSVParser
    {
    private:
        std::unordered_map<std::string, std::string> m_Table;
    public:
        CCSVParser(const char* a_pFile);
        ~CCSVParser();
    public:
        std::string GetValue(std::string a_Key);
    };
}

#endif /* _CSVPARSER_H_ */