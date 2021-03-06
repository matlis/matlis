#include "tests.h"
#include "memory.h"
#include "runtime.h"
#include "boxes/Int.h"
#include "boxes/Apply.h"
#include "boxes/Parser.h"

namespace magic {

template<class Env>
struct Tester
{
    typedef typename Env::Allocator Allocator;
    typedef typename Env::Scheme::obj_t obj_t;
    typedef typename Env::Scheme::box_t box_t;
	typedef typename Env::Scheme::Transfer Transfer;

    template<class T>
    using auto_root_ptr = typename Allocator::template auto_root_ptr<T>;

    template<class T>
    using auto_root_ptr_ref = typename Allocator::template auto_root_ptr_ref<T>;

	struct test_int_t : obj_t
	{
		int val;

		static int& live_count() { static int i = 0; return i; }

		test_int_t( int i ) : val( i ) { live_count() += 1; }

		virtual auto obj_nchildren() const -> size_t { return 0; }
		virtual auto obj_child( size_t i ) const -> const obj_t* { return 0; }
		virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const
		{
			new (p) test_int_t( val );
			live_count() -= 1;
		}
		virtual ~test_int_t() { live_count() -= 1; }
	};

	struct test_pair_t : obj_t
	{
		obj_t* first;
		obj_t* second;

		test_pair_t( obj_t* f, obj_t* s )
			: first( f ), second( s ) {}

		virtual auto obj_nchildren() const -> size_t { return 2; }
		virtual auto obj_child( size_t i ) const -> const obj_t* { return i == 0 ? first : second; }
		virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const
		{
			new (p) test_pair_t( transfer.at(first), transfer.at(second) );
		}
	};

    static void test_objs()
    {TEST
    	Allocator a;

    	{
    		auto_root_ptr<obj_t> i0 = a.template new_obj<test_int_t>(0);
    		{
    			auto_root_ptr<test_int_t> i7 = a.template new_obj<test_int_t>( 7 );
    			test( i7->val == 7, "test int has wrong value" );
    			test( test_int_t::live_count() == 2, "Wrong live count after allocation" );
    			auto_root_ptr<test_pair_t> pair = a.template new_obj<test_pair_t>( i7, i0 );

    			a.gc();
    			test( a.num_allocated() == 3, "Num allocated incorrect after gc (1)" );
    		}

    		a.gc();
    		test( a.num_allocated() == 1, "Num allocated incorrect after gc (2)" );
    		test( test_int_t::live_count() == 1, "Wrong live count after gc" );
    	}

    	a.gc();
    	test( a.num_allocated() == 0, "Num allocated incorrect after gc (3)" );
    	test( test_int_t::live_count() == 0, "Wrong live count after final gc" );
    }

    static void test_boxes()
    {TEST
        //Need to check types when implemented

    	Allocator a;

    	typedef IntBox<Env> Int;
    	typedef ApplyBox<Env> Apply;

    	auto_root_ptr<box_t> root( a );
    	{
    		auto_root_ptr<Int> i8 = a.template new_obj<Int>(8);

    		test( i8->impl.val == 8, "Int has wrong value" );

    		root = a.template new_obj<Apply>( i8, i8 );
    	}

    	a.gc();

    	test( a.num_allocated() == 2, "Wrong number of boxes after gc" );
    }

    static void run_tests()
    {
    	test_objs();
    	test_boxes();
    }
};

void run_tests()
{
    Tester< Env< Debug, SimpleScheme, TestAllocator, NullInterpreter > >::run_tests();
}

}	//namespace
