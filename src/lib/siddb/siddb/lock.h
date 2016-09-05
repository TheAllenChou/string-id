/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

#include <condition_variable>
#include <mutex>

namespace siddb
{
  
  class RwLock
  {
    friend class ReadAutoLock;
    friend class WriteAutoLock;

    public:
      RwLock() : m_counter(0) { }

    private:
      void AcquireReadLock()
      {
        std::unique_lock<std::mutex> lock(m_counterMutex);
        m_cv.wait(lock, [&]() { return !(m_counter & kWriterBit) && m_counter < kMaxReaders; });
        ++m_counter;
      }

      void ReleaseReadLock()
      {
        std::unique_lock<std::mutex> lock(m_counterMutex);
        --m_counter;
        m_cv.notify_one();
      }

      void AcquireWriteLock()
      {
        std::unique_lock<std::mutex> lock(m_counterMutex);
        m_cv.wait(lock, [&]() { return m_counter == 0; });
        m_counter |= kWriterBit;
      }

      void ReleaseWriteLock()
      {
        std::unique_lock<std::mutex> lock(m_counterMutex);
        m_counter &= ~kWriterBit;
        m_cv.notify_all();
      }

      static const size_t kWriterBit = size_t(1) <<  (sizeof(size_t) * 8 - 1);
      static const size_t kMaxReaders = kWriterBit - 1;
      std::mutex m_counterMutex;
      std::condition_variable m_cv;
      size_t m_counter; // MSB, i.e. writer bit, acts as a "mutex"
  };

  class ReadAutoLock
  {
    public:
      ReadAutoLock(RwLock &lock) : m_lock(lock) { m_lock.AcquireReadLock(); }
      ~ReadAutoLock() { m_lock.ReleaseReadLock(); }
    private:
      RwLock &m_lock;
  };

  class WriteAutoLock
  {
    public:
      WriteAutoLock(RwLock &lock) : m_lock(lock) { m_lock.AcquireWriteLock(); }
      ~WriteAutoLock() { m_lock.ReleaseWriteLock(); }
    private:
      RwLock &m_lock;
  };

}
