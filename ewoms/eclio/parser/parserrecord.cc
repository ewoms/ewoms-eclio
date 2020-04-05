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

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/parserrecord.hh>
#include <ewoms/eclio/parser/parseritem.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <ewoms/eclio/parser/rawdeck/rawrecord.hh>

namespace Ewoms {

namespace {
    struct name_eq {
        name_eq( const std::string& x ) : name( x ) {}
        const std::string& name;
        bool operator()( const ParserItem& x ) const {
            return x.name() == this->name;
        }
    };
}

    ParserRecord::ParserRecord()
        : m_dataRecord( false )
    {
    }

    size_t ParserRecord::size() const {
        return m_items.size();
    }

    bool ParserRecord::rawStringRecord() const {
        return this->raw_string_record;
    }

    void ParserRecord::addItem( ParserItem item ) {
        if (m_dataRecord)
            throw std::invalid_argument("Record is already marked as DataRecord - can not add items");

        auto itr = std::find_if( this->m_items.begin(),
                                 this->m_items.end(),
                                 name_eq( item.name() ) );

        if( itr != this->m_items.end() )
            throw std::invalid_argument("Itemname: " + item.name() + " already exists.");

        if (item.parseRaw())
            this->raw_string_record = true;

        this->m_items.push_back( std::move( item ) );
    }

    void ParserRecord::addDataItem( ParserItem item ) {
        if (m_items.size() > 0)
            throw std::invalid_argument("Record already contains items - can not add Data Item");

        this->addItem( std::move( item) );
        m_dataRecord = true;
    }

    std::vector< ParserItem >::const_iterator ParserRecord::begin() const {
        return m_items.begin();
    }

    std::vector< ParserItem >::const_iterator ParserRecord::end() const {
        return m_items.end();
    }

    bool ParserRecord::hasDimension() const {
        return std::any_of( this->begin(), this->end(),
                            []( const ParserItem& x ) { return x.dimensions().size() > 0; } );
    }

    const ParserItem& ParserRecord::get(size_t index) const {
        return this->m_items.at( index );
    }

    bool ParserRecord::hasItem( const std::string& name ) const {
        return std::any_of( this->m_items.begin(),
                            this->m_items.end(),
                            name_eq( name ) );
    }

    const ParserItem& ParserRecord::get( const std::string& name ) const {
        auto itr = std::find_if( this->m_items.begin(),
                                 this->m_items.end(),
                                 name_eq( name ) );

        if( itr == this->m_items.end() )
            throw std::out_of_range( "No item '" + name + "'" );

        return *itr;
    }

    DeckRecord ParserRecord::parse(const ParseContext& parseContext , ErrorGuard& errors , RawRecord& rawRecord, UnitSystem& active_unitsystem, UnitSystem& default_unitsystem, const std::string& keyword, const std::string& filename) const {
        std::vector< DeckItem > items;
        items.reserve( this->size() + 20 );
        for( const auto& parserItem : *this )
            items.emplace_back( parserItem.scan( rawRecord, active_unitsystem, default_unitsystem ) );

        if (rawRecord.size() > 0) {
            std::string msg = "The RawRecord for keyword \""  + keyword + "\" in file\"" + filename + "\" contained " +
                std::to_string(rawRecord.size()) +
                " too many items according to the spec. RawRecord was: " + rawRecord.getRecordString();
            parseContext.handleError(ParseContext::PARSE_EXTRA_DATA , msg, errors);
        }

        return { std::move( items ) };
    }

    bool ParserRecord::equal(const ParserRecord& other) const {
        bool equal_ = true;
        if (size() == other.size()) {
           size_t itemIndex = 0;
           while (true) {
               if (itemIndex == size())
                   break;
               {
                   const auto& item = get(itemIndex);
                   const auto& otherItem = other.get(itemIndex);

                   if (item != otherItem ) {
                       equal_ = false;
                       break;
                   }
               }
               itemIndex++;
            }
        } else
            equal_ = false;
        return equal_;
    }

    bool ParserRecord::isDataRecord() const {
        return m_dataRecord;
    }

    bool ParserRecord::operator==( const ParserRecord& rhs ) const {
        return this->equal( rhs );
    }

    bool ParserRecord::operator!=( const ParserRecord& rhs ) const {
        return !( *this == rhs );
    }

    std::ostream& operator<<( std::ostream& stream, const ParserRecord& rec ) {
        stream << "  ParserRecord { " << std::endl;

        for( const auto& item : rec )
            stream << "      " << item << std::endl;

        return stream << "    }";
    }

    const std::string& ParserRecord::end_string() const {
        return this->record_end;
    }

}
