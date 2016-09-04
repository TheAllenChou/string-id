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
        
      }

      void ReleaseReadLock()
      {
        
      }

      void AcquireWriteLock()
      {
        
      }

      void ReleaseWriteLock()
      {
        
      }

      bool ReadLockPredicate()
      {
        
      }

      bool WriteLockPredicate()
      {
        
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
