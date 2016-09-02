/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include <cstddef>

namespace sidnet
{
  
  template<typename... Args>
  struct BufferFormat;


  // single type
  template <typename T>
  struct BufferFormat<T>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(T);
    }

    static size_t Read(const char *buffer, T &data)
    {
      data = *reinterpret_cast<const T *>(buffer);
      return sizeof(T);
    }

    static size_t Write(char *buffer, const T &data)
    {
      *reinterpret_cast<T *>(buffer) = data;
      return sizeof(T);
    }
  };


  // specialization: C-style string
  template <>
  struct BufferFormat<char *>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(size_t);
    }

    static size_t Read(const char *buffer, char *str)
    {
      const size_t strLen = *reinterpret_cast<const size_t *>(buffer);
      std::memcpy(str, buffer + sizeof(size_t), strLen);
      str[strLen] = '\0';
      return sizeof(size_t) + strLen;
    }

    static size_t Write(char *buffer, const char *str)
    {
      const size_t strLen = std::strlen(str);
      *reinterpret_cast<size_t *>(buffer) = strLen;
      std::memcpy(buffer + sizeof(size_t), str, strLen);
      return sizeof(size_t) + strLen;
    }
  };


  // specialization: C-style string (const)
  template <>
  struct BufferFormat<const char *>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(size_t);
    }

    static size_t Read(const char *buffer, char *str)
    {
      const size_t strLen = *reinterpret_cast<const size_t *>(buffer);
      std::memcpy(str, buffer + sizeof(size_t), strLen);
      str[strLen] = '\0';
      return sizeof(size_t) + strLen;
    }

    static size_t Write(char *buffer, const char *str)
    {
      const size_t strLen = std::strlen(str);
      *reinterpret_cast<size_t *>(buffer) = strLen;
      std::memcpy(buffer + sizeof(size_t), str, strLen);
      return sizeof(size_t) + strLen;
    }
  };
  

  // general
  template<typename T, typename... Rest>
  struct BufferFormat<T, Rest...>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(T) + BufferFormat<Rest...>::GetFixedSize();
    }

    // default
    template <typename T, typename... Rest2>
    static size_t Read(const char *buffer, T &data, Rest2 &... rest)
    {
      const size_t offset = BufferFormat<T>::Read(buffer, data);
      return BufferFormat<Rest2...>::Read(buffer + offset, rest...);
    }

    // specialization: raw buffer
    template <size_t N, typename... Rest2>
    static size_t Read(const char *buffer, char (&data) [N], Rest2 &... rest)
    {
      char *pData = data;
      const size_t offset = BufferFormat<char *>::Read(buffer, pData);
      return BufferFormat<Rest2...>::Read(buffer + offset, rest...);
    }
    
    // default
    template <typename T, typename... Rest2>
    static size_t Write(char *buffer, const T &data, const Rest2 &... rest)
    {
      const size_t offset = BufferFormat<T>::Write(buffer, data);
      return BufferFormat<Rest2...>::Write(buffer + offset, rest...);
    }

    // specialization: raw buffer / string literal
    template <size_t N, typename... Rest2>
    static size_t Write(char *buffer, const char (&data) [N], const Rest2 &... rest)
    {
      const size_t offset = BufferFormat<const char *>::Write(buffer, data);
      return BufferFormat<Rest2...>::Write(buffer + offset, rest...);
    }
  };

}

