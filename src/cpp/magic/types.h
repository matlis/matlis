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

	typedef std::map<const obj_t*,obj_t*> Transfer;

	/* Managed heap objects */
	struct obj_t : ref_t
	{
		virtual auto obj_nchildren() const -> size_t = 0;
		virtual auto obj_child( size_t i ) const -> const obj_t* = 0;
		virtual auto obj_sizeof() const -> size_t = 0;
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const = 0;
		virtual ~obj_t() {}
	};
	
	struct boxset_t;

	/* Boxed types managed and exposed to user code */
	struct box_t : obj_t
	{
		virtual auto dispatch( const boxset_t* args ) -> box_t*;
	};
	
	struct boxset_t : obj_t
	{
		box_t* item;
		boxset_t* next;

		boxset_t( box_t* i, boxset_t* n )
			: item( i ), next( n )
		{
			System::assert( item != 0, "boxset_t item cannot be 0" );
			System::assert( next != 0, "boxset_t next cannot be 0" );
		}
			
		boxset_t()
			: item( 0 ), next( 0 ) {}
	
		virtual auto obj_nchildren() const -> size_t { return is_empty() ? 0 : 2; }
		virtual auto obj_child( size_t i ) const -> const obj_t* { return i == 0 ? item : next; }
		virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const
		{
			if( is_empty() )
				new (p) boxset_t;
			else
				new (p) boxset_t( transfer.at(item), (boxset_t*) transfer.at(next) );
		}
		
		virtual auto is_empty() const -> bool { return next == 0; }
	};
};

}	//namespace

#endif