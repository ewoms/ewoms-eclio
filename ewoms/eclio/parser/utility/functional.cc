/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "config.h"

#include <ewoms/eclio/parser/utility/functional.hh>

namespace Ewoms {
namespace fun {

    iota::iota( int fst, int lst ) : first( fst ), last( lst ) {}

    iota::iota( int lst ) : iota( 0, lst ) {}

    size_t iota::size() const {
        return this->last - this->first;
    }

    iota::const_iterator iota::begin() const {
        return const_iterator{ first };
    }

    iota::const_iterator iota::end() const {
        return const_iterator{ last };
    }

    int iota::const_iterator::operator*() const {
        return this->value;
    }

    iota::const_iterator& iota::const_iterator::operator++() {
        ++( this->value );
        return *this;
    }

    iota::const_iterator iota::const_iterator::operator++( int ) {
        iota::const_iterator copy( *this );
        this->operator++();
        return copy;
    }

    bool iota::const_iterator::operator==( const const_iterator& rhs ) const {
        return this->value == rhs.value;
    }

    bool iota::const_iterator::operator!=( const const_iterator& rhs ) const {
        return !(*this == rhs );
    }

    iota::const_iterator::const_iterator( int x ) : value( x ) {}

}
}
