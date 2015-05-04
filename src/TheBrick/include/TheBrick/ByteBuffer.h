#ifndef _BYTEBUFFER_H_
#define _BYTEBUFFER_H_

#include <string.h>

namespace TheBrick
{
  /// @brief Holds a memory buffer for bytes of data.
  ///
  class CByteBuffer
  {
    // ************************************************************************
    // PUBLIC MEMBERS
    // ************************************************************************

  public:

    /// @brief Creates an instance of ByteBuffer with specified size.
    ///
    /// @param size The size in bytes.
    ///
      CByteBuffer(int size);

    /// @brief Destructor
    ///
      ~CByteBuffer();

    /// @brief Gets the buffer data.
    ///
    /// @returns A void pointer to the data.
    ///
    void* GetData();

    /// @brief Set 1 byte in the buffer.
    ///
    /// @params idx The index for the data.
    /// @param value The data to set.
    ///
    void Set1B(int idx, const unsigned char& value);

    /// @brief Set 2 bytes in the buffer.
    ///
    /// @params idx The index for the data.
    /// @param value The data to set.
    ///
    void Set2B(int idx, const unsigned short& value);

    /// @brief Set 4 bytes in the buffer.
    ///
    /// @params idx The index for the data.
    /// @param value The data to set.
    ///
    void Set4B(int idx, const unsigned int& value);

    /// @brief Get 1 byte from the buffer.
    ///
    /// @params idx The idx to fetch from.
    ///
    /// @returns The byte as a char.
    ///
    unsigned char Get1B(int idx) const;

    /// @brief Get 2 bytes from the buffer.
    ///
    /// @params idx The idx to fetch from.
    ///
    /// @returns The bytes as a short.
    ///
    unsigned short Get2B(int idx) const;

    /// @brief Get 4 bytes from the buffer.
    ///
    /// @params idx The idx to fetch from.
    ///
    /// @returns The bytes as an int.
    ///
    unsigned int Get4B(int idx) const;

    unsigned char operator[](const int& idx)
    {
      return this->buffer[idx];
    }


    // ************************************************************************
    // PRIVATE MEMBERS
    // ************************************************************************

  private:

    /// @brief The buffer of bytes.
    ///
    unsigned char* buffer;

    /// @brief The size / capacity of the buffer in bytes.
    ///
    int size;

  };
}

#endif /* _BYTEBUFFER_H_ */