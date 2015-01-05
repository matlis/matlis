#ifndef MAGIC_BOXES_APPLY
#define MAGIC_BOXES_APPLY

#include "../runtime.h"

namespace magic {

template<class Env>
struct ApplyImpl : Env::impl_t
{
	typedef typename Env::box_t box_t;

	const box_t* const lhs;
	const box_t* const rhs;

	ApplyImpl( const box_t* l, const box_t* r )
		: lhs( l ), rhs( r ) {}
};

template<class Env>
struct ApplyBox : Env::box_t
{
	typedef typename Env::Transfer Transfer;
	typedef typename Env::obj_t obj_t;
	typedef typename Env::box_t box_t;

	ApplyImpl<Env> impl;

	ApplyBox( const box_t* lhs, const box_t* rhs )
		: impl( lhs, rhs ) {}

	virtual auto obj_nchildren() const -> size_t { return 2; }
	virtual auto obj_child( size_t i ) const -> const obj_t* { return i == 0 ? impl.lhs : impl.rhs; }
	virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
	virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const { new (p) ApplyBox( (box_t*) transfer.at(impl.lhs), (box_t*) transfer.at(impl.rhs) ); }
};

}	//namespace

#endif
