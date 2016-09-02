/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#pragma once

class SidUnitTest : public CppUnit::TestFixture
{
  public:
    CPPUNIT_TEST_SUITE(SidUnitTest);
    CPPUNIT_TEST(TestHash);
    CPPUNIT_TEST(TestConcat);
    CPPUNIT_TEST(TestNetBufferFormatFixedSize);
    CPPUNIT_TEST(TestNetBufferFormatCString);
    CPPUNIT_TEST(TestNetBufferFormatStringBuffer);
    CPPUNIT_TEST_SUITE_END();

  public:
    void TestHash();
    void TestConcat();
    void TestNetBufferFormatFixedSize();
    void TestNetBufferFormatCString();
    void TestNetBufferFormatStringBuffer();
};
