#ifndef EWOMS_UTILITY_STRING_H
#define EWOMS_UTILITY_STRING_H

#include <algorithm>
#include <cctype>

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

}

#endif //EWOMS_UTILITY_STRING_H
