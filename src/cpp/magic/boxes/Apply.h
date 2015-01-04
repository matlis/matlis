#ifndef MAGIC_BOXES_APPLY
#define MAGIC_BOXES_APPLY

#include "../runtime.h"

namespace magic {
	
template<class Env>
struct ApplyImpl : Env::impl_t
{
	typedef typename Env::obj_t obj_t;
	
	const obj_t* const lhs;
	const obj_t* const rhs;
	
	
	ApplyImpl( const obj_t* l, const obj_t* r )
		: lhs( l ), rhs( r ) {}
};

template<class Env>
struct ApplyBox : Env::box_t
{
	typedef typename Env::Transfer Transfer;
	typedef typename Env::obj_t obj_t;
	
	ApplyImpl<Env> _impl;
	
	ApplyBox( const obj_t* lhs, const obj_t* rhs )
		: _impl( lhs, rhs ) {}
	
	virtual auto obj_nchildren() const -> size_t { return 2; }
	virtual auto obj_child( size_t i ) const -> const obj_t* { return i == 0 ? _impl.lhs : _impl.rhs; }
	virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
	virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const { new (p) ApplyBox( transfer.at(_impl.lhs), transfer.at(_impl.rhs) ); }
};

}	//namespace

#endif