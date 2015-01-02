#include "magic/tests.h"
#include <iostream>

auto main( int argc, const char* argv[] ) -> int
{
	magic::run_tests();
	std::cout << "\n** All tests passed **\n";
}