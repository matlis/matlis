#include "tests.h"
#include "memory.h"
#include "runtime.h"

namespace magic {

template<class Env>
struct Tester
{
    typedef typename Env::Allocator Allocator;
    typedef typename Env::Scheme::obj_t obj_t;
    typedef typename Env::Scheme::cons_t cons_t;

    template<class T>
    using auto_root_ref = typename Allocator::template auto_root_ref<T>;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

    static void test_objs()
    {TEST
    	Allocator a;
    	
    	auto null = a.template new_obj<cons_t>();
    	auto cons = a.template new_obj<cons_t>( null, null );
    }
    
    static void run_tests()
    {
    	test_objs();
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, NullInterpreter > >::run_tests();
}

}	//namespace