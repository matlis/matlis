#ifndef MAGIC_BOXES_INT
#define MAGIC_BOXES_INT

#include "../runtime.h"

namespace magic {

template<class Env>
struct IntImpl : Env::impl_t
{
    const int val;

	IntImpl( int i )
		: val( i ) {}
};

template<class Env>
struct IntBox : Env::box_t
{
	typedef typename Env::Transfer Transfer;
	typedef typename Env::obj_t obj_t;

	IntImpl<Env> impl;

	IntBox( int i )
		: impl( i ) {}

	virtual auto obj_nchildren() const -> size_t { return 0; }
	virtual auto obj_child( size_t i ) const -> const obj_t* { return 0; }
	virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
	virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const { new (p) IntBox( impl.val ); }
};

}	//namespace

#endif
