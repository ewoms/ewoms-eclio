#ifndef EWOMS_UTILITY_STRING_H
#define EWOMS_UTILITY_STRING_H

#include <algorithm>
#include <cctype>
#include <string>

namespace Ewoms {

template< typename T, typename U >
U& uppercase( const T& src, U& dst ) {
    const auto up = []( char c ) { return std::toupper( c ); };
    std::transform( std::begin( src ), std::end( src ), std::begin( dst ), up );
    return dst;
}

template< typename T >
typename std::decay< T >::type uppercase( T&& x ) {
    typename std::decay< T >::type t( std::forward< T >( x ) );
    return uppercase( t, t );
}

template<typename T>
std::string trim_copy(const T& s) {
{
    auto ret = std::string(s.c_str());

    const auto end = ret.find_last_not_of(" \t\n\r\f\v");
    if (end == std::string::npos)
        return "";

    return ret.substr(0, end + 1);
}
}
}
#endif //EWOMS_UTILITY_STRING_H
