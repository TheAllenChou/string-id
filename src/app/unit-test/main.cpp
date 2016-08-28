/******************************************************************************/
/*
  Author  - Ming-Lun "Allen" Chou
  Web     - http://AllenChou.net
  Twitter - @TheAllenChou
 */
/******************************************************************************/

int main(void)
{
  // Get the top level suite from the registry
  CppUnit::Test *suite = 
      CppUnit::TestFactoryRegistry::getRegistry().makeTest();

  // Adds the test to the list of test to run
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(suite);

  // Change the default outputter to a compiler error format outputter
  runner.setOutputter
  (
    new CppUnit::CompilerOutputter
    (
      &runner.result(),
      std::cerr
    )
  );

  // Run the tests.
  std::cout << "SID UNIT TEST" << std::endl << std::endl;
  bool wasSucessful = runner.run();
  std::cout << std::endl;

  std::system("pause");

  // Return error code 1 if the one of test failed.
  return wasSucessful ? 0 : 1;
}