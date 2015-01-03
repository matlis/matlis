#ifndef HEADER_MAGIC_MEMORY
#define HEADER_MAGIC_MEMORY

#include "types.h"
#include <set>

namespace magic {
	
struct OutOfMemory {};

#define MetaAllocator template<class System, template<class> class SchemeT>

/** Auto register roots with allocator instance **/

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class AllocatorBase
{
protected:
	typedef SchemeT<System> Scheme;
	typedef AllocatorT<System, SchemeT> Allocator;
	typedef typename Scheme::obj_t obj_t;
	
	size_t _gc_count;
	std::set<obj_t**> _obj_roots;

	AllocatorBase()
		: _gc_count( 0 ) {}
public:
    auto gc_count() const -> size_t { return _gc_count; }

    void add_root( obj_t** root ) { _obj_roots.insert( root ); }
	void del_root( obj_t** root ) { _obj_roots.erase( root ); }

    template<class T>
    struct auto_root_ptr_ref
    {
        Allocator& alloc;
        T* ptr;
    
        auto operator->() const -> T* { return ptr; }
		auto operator*() const -> T& { return *ptr; }
		operator T*() const { return this->ptr; }

        auto_root_ptr_ref( Allocator& a, T* root ) : ptr( root ), alloc( a ) {}
    };

    template<class T>
    class auto_root_ptr : public auto_root_ptr_ref<T>
    {
        explicit auto_root_ptr( const auto_root_ptr& );
        auto_root_ptr& operator=( const auto_root_ptr& );
    public:
        typedef auto_root_ptr_ref<T> ref;

        auto_root_ptr( Allocator& alloc, T* root=0 ) : ref( alloc, root ) { this->alloc.add_root( (obj_t**) &this->ptr ); }
        template<class S>
        auto_root_ptr( const auto_root_ptr_ref<S>& r ) : ref( r.alloc, r.ptr ) { this->alloc.add_root( (obj_t**) &this->ptr ); }
        ~auto_root_ptr() { this->alloc.del_root( (obj_t**) &this->ptr ); }
        auto_root_ptr& operator=( T* p ) { this->_ptr = p; return *this; }
		operator T*() const { return this->ptr; }
    };
    
    ~AllocatorBase()
	{
		if( !_obj_roots.empty() )
		{
			std::cerr << "Objects remain on allocator destruction" << std::endl;
			throw Error<Assertion>( "Objects remain on allocator destruction" );
		}
	}
};

/**
    TestAllocator
    Simple but inefficient allocator for testing.
    It shifts all the objects on each memory allocation to test that the roots are correctly defined
**/

template<class System, MetaScheme class SchemeT>
struct TestAllocator : AllocatorBase< System, SchemeT, TestAllocator >
{
    typedef SchemeT<System> Scheme;
    typedef typename Scheme::obj_t obj_t;
    typedef AllocatorBase< System, SchemeT, TestAllocator > Base;
    template<class T>
    using auto_root_ptr_ref = typename Base::template auto_root_ptr_ref<T>;
private:
	static const size_t RESERVED = 4096;
	
    std::set<const obj_t*> _allocated;
    size_t _reserved;

	auto _post_alloc( obj_t* p ) -> obj_t*;
    static void _mark( std::set<const obj_t*>& live, const obj_t* p );
    void _shift();
    static void _destroy( const obj_t* p ) { p->~obj_t(); free( (void*) p ); }
public:
	static auto name() -> std::string { return "test"; }
	
    TestAllocator()
        : AllocatorBase<System, SchemeT, TestAllocator>(), _allocated(), _reserved( RESERVED ) {}

	~TestAllocator()
	{
		for( auto p : _allocated )
			_destroy( p );
	}

	void gc();
	
	template<class T>
	auto new_obj() -> auto_root_ptr_ref<T>
    {
		return auto_root_ptr_ref<T>( *this, (T*) _post_alloc( new (malloc(sizeof(T))) T ) );
    }
    
    template<class T, class T1>
	auto new_obj( const T1& t1 ) -> auto_root_ptr_ref<T>
    {
		return auto_root_ptr_ref<T>( *this, (T*) _post_alloc( new (malloc(sizeof(T))) T( t1 ) ) );
    }
	
	template<class T, class T1, class T2>
	auto new_obj( const T1& t1, const T2& t2 ) -> auto_root_ptr_ref<T>
    {
		return auto_root_ptr_ref<T>( *this, (T*) _post_alloc( new (malloc(sizeof(T))) T( t1, t2 ) ) );
    }

    auto num_allocated() const -> size_t
    {
        return _allocated.size();
    }
    
    auto num_total() const -> size_t
    {
    	return _allocated.size();
    }
};

}	//namespace

#endif