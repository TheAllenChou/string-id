/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

#include "unit-test.h"

#include "sid/sid.h"

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
