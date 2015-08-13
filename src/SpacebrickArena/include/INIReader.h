#ifndef _INIREADER_H_
#define _INIREADER_H_

#include <string>
#include <unordered_map>
#include <stdio.h>
#include <assert.h>

namespace sba
{
    class CIniReader
    {
    private:
        std::string m_Path;
        std::unordered_map<std::string, std::string> m_Table;
    public:
        void SetPath(const char* a_pFile);
    public:
        CIniReader(const char* a_pFile);
        ~CIniReader();
    public:
        std::string GetValue(std::string a_Key);
        void SetValue(std::string a_Key, std::string a_Value);
        void Save();
    };
}

#endif /* _INIREADER_H_ */