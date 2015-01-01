#ifndef HEADER_MAGIC_INTERPRETER_H
#define HEADER_MAGIC_INTERPRETER_H

#include "types.h"
#include "memory.h"

namespace magic
{

#define MetaInterpreter template<class System, template<class> class SchemeT, template<class System, template<class> class SchemeT> class AllocatorT>

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class InterpreterBase
{
    typedef AllocatorT<System, SchemeT> Allocator;

    Allocator _allocator;
public:
    InterpreterBase( size_t maxmem )
        : _allocator( maxmem ) {}

    Allocator& allocator() { return _allocator; }
};

template<class System, MetaScheme class SchemeT, MetaAllocator class AllocatorT>
class NullInterpreter : public InterpreterBase<System, SchemeT, AllocatorT>
{
public:
    NullInterpreter( size_t maxmem )
        : InterpreterBase<System, SchemeT, AllocatorT>( maxmem ) {}
};

}   //namespace

#endif