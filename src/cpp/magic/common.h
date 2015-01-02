#ifndef HEADER_MAGIC_COMMON_H
#define HEADER_MAGIC_COMMON_H

#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <type_traits>

/** MAGIC - MAthematical Generic Intermediate Code  **/

namespace magic {

using namespace std::string_literals;

struct AnyType;

template<class Type, class SubType=AnyType>
class Error;

template<>
template<class Type>
class Error<Type,AnyType> : public std::exception
{
    std::string _message;
public:
    Error( const std::string& message )
    	: _message( message ) {}

	const char* what() const throw() { return ('\n' + _message).c_str(); }
	const std::string& message() const { return _message; }
};

template<class Type, class SubType>
struct Error : Error<Type>
{
	Error( const std::string& message )
    	: Error<Type>( message ) {}
};

struct Assertion;
struct Test;

inline void fail( const std::string& msg )
{
	throw Error<Test>( msg );
}

inline void pass()
{
	std::cout << '.' << std::flush;
}

inline void test( bool cond, const std::string& msg )
{
	if( !cond )
		fail( msg );

    pass();
}

#define TEST std::cout << "\n" << __FUNCTION__ << ": " << std::flush;

#define ASSERT std::static_assert

inline auto extract_type_name( const char* name ) -> auto
{
    std::string s = name;
    const size_t n = std::min( s.find( ';' ), s.find( ']' ) );
    return s.substr( 33, n-33 );
}

template<typename T>
inline auto type_name() -> auto
{
    static std::string s = extract_type_name( __PRETTY_FUNCTION__ );
    return s;
}

#define TYPENAME( ... ) type_name<__VA_ARGS__>()

template<class T>
auto operator+( const std::string& s, const T& t ) -> std::string
{
    std::ostringstream oss;
    oss << s << t;
    return oss.str();
}

#define here(x) std::cout << "here" << x << std::endl;

}	//namespace

#endif