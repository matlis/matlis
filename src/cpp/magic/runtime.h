#ifndef MAGIC_RUNTIME_H
#define MAGIC_RUNTIME_H

#include "interpreter.h"

namespace magic {

struct Runtime;

template<bool AssertF=false>
struct Params
{
    static const bool AssertFlag = AssertF;

    template<bool flag> struct Assert : Params<flag> {};
};

template<class Params>
struct System
{
    static void assert( bool cond, const std::string& msg )
    {
        if( Params::AssertFlag && !cond )
            throw Error<Assertion>( msg );
    }
    
    static auto name() -> std::string
    {
    	return "assert="s + (Params::AssertFlag ? "on" : "off" );
    }
};

typedef System< Params<>::Assert<false> > Debug;

template<
    class Sys,
    MetaScheme class SchemeT,
    MetaAllocator class AllocatorT,
    MetaInterpreter class InterpreterT
>
struct Env
{
	using System = Sys;
    using Scheme = SchemeT<System>;
    using Allocator = AllocatorT<System, SchemeT>;
    using Interpreter = InterpreterT<System, SchemeT, AllocatorT>;

	using Transfer = typename Scheme::Transfer;
	using impl_t = typename Scheme::impl_t;
    using obj_t = typename Scheme::obj_t;
    using box_t = typename Scheme::box_t;
};

}   //namespace

#endif