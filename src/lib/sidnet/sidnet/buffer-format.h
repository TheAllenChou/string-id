#include <vcruntime.h>
/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include <cstddef>
#include <type_traits>

namespace sidnet
{
  
  template<typename... Args>
  struct BufferFormat;

  template <typename T>
  struct BufferFormat<T>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(T);
    }

    static const char *Read(const char *buffer, T &data)
    {
      data = *reinterpret_cast<const T *>(buffer);
      return buffer + sizeof(T);
    }

    static char *Write(char *buffer, const T &data)
    {
      *reinterpret_cast<T *>(buffer) = data;
      return buffer + sizeof(T);
    }
  };

  template <>
  struct BufferFormat<char *>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(size_t);
    }

    static const char *Read(const char *buffer, char *str)
    {
      const size_t strLen = *reinterpret_cast<const size_t *>(buffer);
      std::memcpy(str, buffer + sizeof(size_t), strLen);
      str[strLen] = '\0';
      return buffer + sizeof(size_t) + strLen;
    }

    static char *Write(char *buffer, const char *str)
    {
      const size_t strLen = std::strlen(str);
      *reinterpret_cast<size_t *>(buffer) = strLen;
      std::memcpy(buffer + sizeof(size_t), str, strLen);
      return buffer + sizeof(size_t) + strLen;
    }
  };

  template <>
  struct BufferFormat<const char *>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(size_t);
    }

    static const char *Read(const char *buffer, char *str)
    {
      const size_t strLen = std::strlen(buffer);
      std::memcpy(str, buffer + sizeof(size_t), strLen);
      str[strLen] = '\0';
      return buffer + sizeof(size_t) + strLen;
    }

    static char *Write(char *buffer, const char *str)
    {
      const size_t strLen = std::strlen(str);
      *reinterpret_cast<size_t *>(buffer) = strLen;
      std::memcpy(buffer + sizeof(size_t), str, strLen);
      return buffer + sizeof(size_t) + strLen;
    }
  };
  
  template<typename T, typename... Rest>
  struct BufferFormat<T, Rest...>
  {
    static constexpr size_t GetFixedSize()
    {
      return sizeof(T) + BufferFormat<Rest...>::GetFixedSize();
    }

    template <typename... Rest2>
    static const char *Read(const char *buffer, T &data, Rest2 &... rest)
    {
      const char *nextBuffer = BufferFormat<T>::Read(buffer, data);
      return BufferFormat<Rest...>::Read(nextBuffer, rest...);
    }
    
    template <typename... Rest2>
    static char *Write(char *buffer, const T &data, const Rest2 &... rest)
    {
      char *nextBuffer = BufferFormat<T>::Write(buffer, data);
      return BufferFormat<Rest2...>::Write(nextBuffer, rest...);
    }
  };

}
