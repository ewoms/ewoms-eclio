#include <iterator>
#include <ostream>

#include <ewoms/eclio/parser/utility/stringview.hh>

std::ostream& Ewoms::operator<<( std::ostream& stream, const Ewoms::string_view& view ) {
    std::copy( view.begin(), view.end(), std::ostream_iterator< char >( stream ) );
    return stream;
}
