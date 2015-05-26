#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include <PuReEngine/Core.h>
#include <PuReEngine/Defines.h>

#include <stdio.h>

namespace TheBrick
{
    class CSerializer
    {
    public:

    private:
        FILE* m_pFile;

    public:
        CSerializer();
        ~CSerializer();

        bool OpenRead(const char* a_pFile);
        bool OpenWrite(const char* a_pFile);
        void Close();
        void Read(void* a_pBuffer, size_t a_Size);
        void Write(const void* a_pData, size_t a_Size);

        bool ReadBool();
        char ReadChar();
        short ReadShort();
        int ReadInt();
        unsigned char ReadCharUnsigned();
        unsigned short ReadShortUnsigned();
        unsigned int ReadIntUnsigned();
        float ReadFloat();
        PuRe_Vector2F ReadVector2();
        PuRe_Vector3F ReadVector3();
        PuRe_Vector4F ReadVector4();
        PuRe_QuaternionF ReadQuaternion();
        PuRe_MatrixF ReadMatrix();

        void Write(bool a_Data);
        void Write(char a_Data);
        void Write(short a_Data);
        void Write(int a_Data);
        void Write(unsigned char a_Data);
        void Write(unsigned short a_Data);
        void Write(unsigned int a_Data);
        void Write(float a_Data);
        void Write(PuRe_Vector2F  a_Data);
        void Write(PuRe_Vector3F  a_Data);
        void Write(PuRe_Vector4F  a_Data);
        void Write(PuRe_QuaternionF a_Data);
        void Write(PuRe_MatrixF a_Data);
    };
}

#endif /* _SERIALIZER_H_ */