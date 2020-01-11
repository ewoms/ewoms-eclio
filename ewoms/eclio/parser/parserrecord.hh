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

#ifndef PARSERRECORD_H
#define PARSERRECORD_H

#include <iosfwd>
#include <vector>
#include <memory>

#include <ewoms/eclio/parser/parseritem.hh>

namespace Ewoms {

    class Deck;
    class DeckRecord;
    class ParseContext;
    class ParserItem;
    class RawRecord;
    class ErrorGuard;
    class UnitSystem;

    class ParserRecord {
    public:
        ParserRecord();
        size_t size() const;
        void addItem( ParserItem );
        void addDataItem( ParserItem item );
        const ParserItem& get(size_t index) const;
        const ParserItem& get(const std::string& itemName) const;
        DeckRecord parse( const ParseContext&, ErrorGuard&, RawRecord&, UnitSystem& active_unitsystem, UnitSystem& default_unitsystem, const std::string& keyword, const std::string& filename) const;
        bool isDataRecord() const;
        bool equal(const ParserRecord& other) const;
        bool hasDimension() const;
        bool hasItem(const std::string& itemName) const;
        std::vector< ParserItem >::const_iterator begin() const;
        std::vector< ParserItem >::const_iterator end() const;

        bool operator==( const ParserRecord& ) const;
        bool operator!=( const ParserRecord& ) const;
        bool rawStringRecord() const;
        const std::string& end_string() const;

    private:
        bool m_dataRecord;
        std::vector< ParserItem > m_items;
        bool raw_string_record = false;
        std::string record_end = "/";
    };

std::ostream& operator<<( std::ostream&, const ParserRecord& );
}

#endif  /* PARSERRECORD_HH */

