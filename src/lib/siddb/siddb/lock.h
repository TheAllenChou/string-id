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
        while (!ReadLockPredicate())
          m_cv.wait(lock);
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
        m_counter |= kWriterFlag;
        while (!WriteLockPredicate())
          m_cv.wait(lock);
      }

      void ReleaseWriteLock()
      {
        std::unique_lock<std::mutex> lock(m_counterMutex);
        m_counter &= ~kWriterFlag;
        m_cv.notify_all();
      }

      bool ReadLockPredicate()
      {
        return !(m_counter & kWriterFlag) && m_counter < kMaxReaders;
      }

      bool WriteLockPredicate()
      {
        return m_counter == kWriterFlag;
      }

      static const size_t kWriterFlag = size_t(1) <<  (sizeof(size_t) * 8 - 1);
      static const size_t kMaxReaders = kWriterFlag - 1;
      std::mutex m_counterMutex;
      std::condition_variable m_cv;
      size_t m_counter;
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
