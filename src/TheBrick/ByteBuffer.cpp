#include "include/TheBrick/ByteBuffer.h"
namespace TheBrick
{
    // **************************************************************************
    // **************************************************************************
    CByteBuffer::CByteBuffer(int size)
    {
        this->size = size;

        this->buffer = new unsigned char[this->size];

        memset(this->buffer, 0, this->size);
    }

    // **************************************************************************
    // **************************************************************************
    CByteBuffer::~CByteBuffer()
    {
        if (this->buffer != nullptr)
            delete[] this->buffer;
    }

    // **************************************************************************
    // **************************************************************************
    void CByteBuffer::Set1B(int idx, const unsigned char& value)
    {
        this->buffer[idx] = value;
    }

    // **************************************************************************
    // **************************************************************************
    void CByteBuffer::Set2B(int idx, const unsigned short& value)
    {
        this->buffer[idx] = value >> 8 & 0xff;
        this->buffer[idx + 1] = value & 0xff;
    }

    // **************************************************************************
    // **************************************************************************
    void CByteBuffer::Set4B(int idx, const unsigned int& value)
    {
        this->buffer[idx] = value >> 24 & 0xff;
        this->buffer[idx + 1] = value >> 16 & 0xff;
        this->buffer[idx + 2] = value >> 8 & 0xff;
        this->buffer[idx + 3] = value & 0xff;
    }

    // **************************************************************************
    // **************************************************************************
    unsigned char CByteBuffer::Get1B(int idx) const
    {
        return this->buffer[idx];
    }

    // **************************************************************************
    // **************************************************************************
    unsigned short CByteBuffer::Get2B(int idx) const
    {
        return (unsigned short)this->buffer[idx] << 8 | (unsigned short)this->buffer[idx + 1];
    }

    // **************************************************************************
    // **************************************************************************
    unsigned int CByteBuffer::Get4B(int idx) const
    {
        return (unsigned short)this->buffer[idx] << 24 | (unsigned short)this->buffer[idx + 1] << 16 |
            (unsigned short)this->buffer[idx + 2] << 8 | (unsigned short)this->buffer[idx + 3];
    }

    // **************************************************************************
    // **************************************************************************
    void* CByteBuffer::GetData()
    {
        return this->buffer;
    }
}