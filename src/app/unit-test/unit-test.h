/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#ifndef UNIT_TEST_H
#define UNIT_TEST_H


class SidUnitTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE(SidUnitTest);
    CPPUNIT_TEST(TestHash);
    CPPUNIT_TEST(TestConcat);
    CPPUNIT_TEST(TestDatabase);
    CPPUNIT_TEST(TestNetBufferFormatFixedSize);
    CPPUNIT_TEST(TestNetBufferFormatCString);
    CPPUNIT_TEST(TestNetBufferFormatStringBuffer);
    CPPUNIT_TEST(TestRwLock);
    CPPUNIT_TEST_SUITE_END();

  public:
    void TestHash();
    void TestConcat();
    void TestDatabase();
    void TestNetBufferFormatFixedSize();
    void TestNetBufferFormatCString();
    void TestNetBufferFormatStringBuffer();
    void TestRwLock();
};


#endif
