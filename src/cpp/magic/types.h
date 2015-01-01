#ifndef HEADER_MAGIC_TYPES
#define HEADER_MAGIC_TYPES

#include "common.h"
#include <map>

namespace magic {
	
#define MetaScheme template<class System>
	
template<class System>
struct SimpleScheme
{
	/* All reference types */
	struct ref_t {};

	/* All implementation types (unmanaged) */
	struct impl_t : ref_t {};
	
	struct obj_t;

	typedef std::map<const obj_t*,const obj_t*> Transfer;

	/* Managed heap objects */
	struct obj_t : ref_t
	{
		virtual auto obj_nchildren() const -> size_t = 0;
		virtual auto obj_child( size_t i ) const -> const obj_t* = 0;
		virtual auto obj_sizeof() const -> size_t = 0;
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const = 0;
		virtual void obj_destroy() = 0;
	};
	
	static auto null() -> const obj_t* { return 0; }

	/* Boxed types managed and exposed to user code */
	struct box_t : obj_t
	{
		virtual auto dispatch( const box_t* ) -> box_t*;
	};

	struct list_t : obj_t
	{
		const obj_t* head;
		const obj_t* tail;
	
		virtual auto obj_nchildren() const -> size_t { return 2; }
		virtual auto obj_child( size_t i ) const -> const obj_t* { return i == 0 ? head : tail; }
		virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const { new (p) list_t{ transfer.at(head), transfer.at(tail) }; }
		virtual void obj_destroy() {}
	};
};

}	//namespace

#endif