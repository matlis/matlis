#include "magic/tests.h"
#include <iostream>

using namespace magic;

auto main( int argc, const char* argv[] ) -> int
{
	magic::run_tests();
	std::cout << "\n** All tests passed **\n";

}