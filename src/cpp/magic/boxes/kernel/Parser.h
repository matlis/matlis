#ifndef MAGIC_BOXES_KERNEL_PARSER
#define MAGIC_BOXES_KERNEL_PARSER

#include "../../runtime.h"

namespace magic {
	
template<class Env>
struct Parser
{
	typedef typename Env::obj_t obj_t;
	
	auto parse( const std::string& s ) -> obj_t*
	{
		return 0;
	}
};

}	//namespace

#endif