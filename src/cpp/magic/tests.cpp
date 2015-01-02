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
	typedef typename Env::Scheme::Transfer Transfer;

    template<class T>
    using auto_root = typename Allocator::template auto_root<T>;

	struct test_int_t : obj_t
	{
		int val;

		test_int_t( int i ) : val( i ) {}

		virtual auto obj_nchildren() const -> size_t { return 0; }
		virtual auto obj_child( size_t i ) const -> const obj_t* { return 0; }
		virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const { new (p) test_int_t( val ); }
	};

    static void test_objs()
    {TEST
    	Allocator a;
    	
    	auto_root<cons_t> null( a, a.template new_obj<cons_t>() );
    	auto_root<obj_t> i7( a, a.template new_obj<test_int_t>( 7 ) );
    	auto_root<obj_t> cons( a, a.template new_obj<cons_t>( i7, null ) );
    	
    	a.gc();
    	test( a.num_allocated() == 3, "Num allocated incorrect" );
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