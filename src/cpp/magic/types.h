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
		virtual void obj_destroy() const = 0;
	};

	/* Boxed types managed and exposed to user code */
	struct box_t : obj_t
	{
		virtual auto dispatch( const box_t* ) -> box_t*;
	};
	
	class cons_t : public obj_t
	{
		obj_t* _car;
		obj_t* _cdr;
	public:
		cons_t( obj_t* car, obj_t* cdr )
			: _car( car ), _cdr( cdr )
		{
			System::assert( car != 0, "cons_t car cannot be 0" );
			System::assert( cdr != 0, "cons_t cdr cannot be 0" );
		}
			
		cons_t()
			: _car( 0 ), _cdr( 0 ) {}
	
		virtual auto obj_nchildren() const -> size_t { return is_null() ? 0 : 2; }
		virtual auto obj_child( size_t i ) const -> const obj_t* { return i == 0 ? _car : _cdr; }
		virtual auto obj_sizeof() const -> size_t { return sizeof(*this); }
		virtual void obj_transfer( obj_t* p, const Transfer& transfer ) const
		{
			if( is_null() )
				new (p) cons_t;
			else
				new (p) cons_t( transfer.at(_car), transfer.at(_cdr) );
		}
		virtual void obj_destroy() const {}
		
		virtual auto is_null() const -> bool { return _car == 0; }
	};
};

}	//namespace

#endif