#include "memory.h"
#include "runtime.h"
#include <map>

using namespace magic;

/** TestAllocator */

template<class System, MetaScheme class SchemeT>
auto TestAllocator<System, SchemeT>::_post_alloc( obj_t* p ) -> obj_t*
{
	this->add_root( &p );
    _shift();
	if( --_reserved == 0 )
		gc();
	this->del_root( &p );
	return p;
}

template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::gc()
{
	_reserved = RESERVED;
    ++this->_gc_count;

    std::set<const obj_t*> all;
    all.swap( _allocated );

    for( auto r : this->_obj_roots )
        _mark( _allocated, *r );

    for( auto p : all )
        if( _allocated.find( p ) == _allocated.end() )
            delete p;
}



template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::_shift()
{
	typedef typename SchemeT<System>::Transfer Transfer;
	
    std::set<const obj_t*> all;
    all.swap( _allocated );

    Transfer old_to_new;

    for( auto p : all )
    {
        auto q = (obj_t*) malloc( p->obj_sizeof() );
        _allocated.insert( q );
        old_to_new[p] = q;
    }

    for( auto p : old_to_new )
    {
        p.first->obj_transfer( p.second, old_to_new );
        free( (void*) p.first );
    }

    for( auto r : this->_obj_roots )
        *r = old_to_new[*r];
}

template<class System, MetaScheme class SchemeT>
void TestAllocator<System, SchemeT>::_mark( std::set<const obj_t*>& live, const obj_t* p )
{
    if( live.find( p ) == live.end() )
    {
        live.insert( p );
        
        auto n = p->obj_nchildren();
        
        for( size_t i = 0; i != n; ++i )
			_mark( live, p->obj_child( i ) );
	}
}

template class TestAllocator< Debug, SimpleScheme >;
