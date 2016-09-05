/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "unit-test.h"

#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

#include "sid/sid.h"
#include "siddb/lock.h"
#include "sidnet/buffer-format.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(SidUnitTest);

void SidUnitTest::TestHash()
{
  const StringId s0 = kInvalidStringId;
  const StringId s1("test");
  const StringId s2 = SID("test");

  CPPUNIT_ASSERT_EQUAL(s1, s2);

  switch (s1.GetValue())
  {
    case SID_VAL("test"):
      break;

    default:
      CPPUNIT_FAIL("Switch test failed.");
      break;
  }
}

void SidUnitTest::TestConcat()
{
  CPPUNIT_ASSERT_EQUAL(SID("abcd"), SID("ab").Concat("cd"));
  CPPUNIT_ASSERT_EQUAL(SID("abcd"), SID("a").Concat("b").Concat("c").Concat("d"));
}

void SidUnitTest::TestNetBufferFormatFixedSize()
{
  typedef sidnet::BufferFormat<char, short, int, float, double> Format;

  const size_t refSize = sizeof(char) + sizeof(short) + sizeof(int) + sizeof(float) + sizeof(double);

  CPPUNIT_ASSERT_EQUAL(refSize, Format::GetFixedSize());

  char refBuffer[128];
  char testBuffer[128];

  Format::Write(testBuffer, char(0), short(1), 2, 3.0f, 4.0);

  *reinterpret_cast<char *>(refBuffer) = char(0);
  *reinterpret_cast<short *>(refBuffer + sizeof(char)) = short(1);
  *reinterpret_cast<int *>(refBuffer + sizeof(char) + sizeof(short)) = 2;
  *reinterpret_cast<float *>(refBuffer + sizeof(char) + sizeof(short) + sizeof(int)) = 3.0f;
  *reinterpret_cast<double *>(refBuffer + sizeof(char) + sizeof(short) + sizeof(int) + sizeof(float)) = 4.0;

  CPPUNIT_ASSERT(!memcmp(refBuffer, testBuffer, refSize));

  char c;
  short s;
  int i;
  float f;
  double d;

  Format::Read(refBuffer, c, s, i, f, d);

  CPPUNIT_ASSERT_EQUAL(char(0), c);
  CPPUNIT_ASSERT_EQUAL(short(1), s);
  CPPUNIT_ASSERT_EQUAL(2, i);
  CPPUNIT_ASSERT_EQUAL(3.0f, f);
  CPPUNIT_ASSERT_EQUAL(4.0, d);
}

void SidUnitTest::TestNetBufferFormatCString()
{
  typedef sidnet::BufferFormat<float, char *, double> Format;

  const size_t refFixedSize = sizeof(float) + sizeof(size_t) + sizeof(double);

  CPPUNIT_ASSERT_EQUAL(refFixedSize, Format::GetFixedSize());

  const char *testStr = "test-string";
  const size_t testStrLen = std::strlen(testStr);
  const size_t refSize = refFixedSize + testStrLen;

  char refBuffer[128];
  char testBuffer[128];

  Format::Write(testBuffer, 1.0f, testStr, 2.0); 

  *reinterpret_cast<float *>(refBuffer) = 1.0f;
  *reinterpret_cast<size_t *>(refBuffer + sizeof(float)) = testStrLen;
  std::memcpy(refBuffer + sizeof(float) + sizeof(size_t), testStr, testStrLen);
  *reinterpret_cast<double *>(refBuffer + sizeof(float) + sizeof(size_t) + testStrLen) = 2.0;

  CPPUNIT_ASSERT(!memcmp(refBuffer, testBuffer, testStrLen));

  float f;
  char *s = new char[128];
  double d;

  Format::Read(refBuffer, f, s, d);

  CPPUNIT_ASSERT_EQUAL(1.0f, f);
  CPPUNIT_ASSERT(!std::memcmp(testStr, s, testStrLen));
  CPPUNIT_ASSERT_EQUAL(2.0, d);

  delete s;
}

void SidUnitTest::TestNetBufferFormatStringBuffer()
{
  typedef sidnet::BufferFormat<float, char *, double> Format;

  const size_t refFixedSize = sizeof(float) + sizeof(size_t) + sizeof(double);

  CPPUNIT_ASSERT_EQUAL(refFixedSize, Format::GetFixedSize());

  const char testStr[] = "test-string";
  const size_t testStrLen = std::strlen(testStr);
  const size_t refSize = refFixedSize + testStrLen;

  char refBuffer[128];
  char testBuffer[128];

  Format::Write(testBuffer, 1.0f, testStr, 2.0);

  *reinterpret_cast<float *>(refBuffer) = 1.0f;
  *reinterpret_cast<size_t *>(refBuffer + sizeof(float)) = testStrLen;
  std::memcpy(refBuffer + sizeof(float) + sizeof(size_t), testStr, testStrLen);
  *reinterpret_cast<double *>(refBuffer + sizeof(float) + sizeof(size_t) + testStrLen) = 2.0;

  CPPUNIT_ASSERT(!memcmp(refBuffer, testBuffer, testStrLen));

  float f;
  char s[128];
  double d;

  Format::Read(refBuffer, f, s, d);

  CPPUNIT_ASSERT_EQUAL(1.0f, f);
  CPPUNIT_ASSERT(!std::memcmp(testStr, s, testStrLen));
  CPPUNIT_ASSERT_EQUAL(2.0, d);
}

void SidUnitTest::TestRwLock()
{
  using namespace siddb;

  const size_t numReaders = 10;
  const size_t numWriters = 10;
  const size_t iterations = 2000000;

  bool readValid = true;
  std::mutex readValidMutex;

  RwLock lock;
  size_t data = 0;

  std::vector<std::thread> threads;

  threads.clear();
  for (size_t i = 0; i < numReaders; ++i)
  {
    threads.push_back(std::thread(
      [&, i]()
      {
        ReadAutoLock autoLock(lock);
        for (size_t j = 0; j < iterations; ++j)
        {
          if (data != 0)
          {
            std::unique_lock<std::mutex> readValidMutexLock(readValidMutex);
            readValid = false;
          }
        }
      }
    ));
  }
  
  for (auto &t : threads)
    t.join();

  CPPUNIT_ASSERT(readValid);

  threads.clear();
  for (size_t i = 0; i < numReaders; ++i)
  {
    threads.push_back(std::thread(
      [&, i]()
      {
        ReadAutoLock autoLock(lock);
        for (size_t j = 0; j < iterations; ++j)
        {
          if (data != data)
          {
            std::unique_lock<std::mutex> readValidMutexLock(readValidMutex);
            readValid = false;
          }
        }
      }
    ));
  }
  for (size_t i = 0; i < numWriters; ++i)
  {
    threads.push_back(std::thread(
      [&, i]()
      {
        WriteAutoLock autoLock(lock);
        for (size_t j = 0; j < iterations; ++j)
        {
          data = data + 1;
        }
      }
    ));
  }

  for (auto &t : threads)
    t.join();

  CPPUNIT_ASSERT(readValid);
  CPPUNIT_ASSERT_EQUAL(iterations * numWriters, data);
}
