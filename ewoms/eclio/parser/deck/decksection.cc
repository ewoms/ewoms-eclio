// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
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

#include <iostream>
#include <exception>
#include <algorithm>
#include <cassert>
#include <set>
#include <string>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>

namespace Ewoms {

    static bool isSectionDelimiter( const DeckKeyword& keyword ) {
        const auto& name = keyword.name();
        for( const auto& x : { "RUNSPEC", "GRID", "EDIT", "PROPS",
                               "REGIONS", "SOLUTION", "SUMMARY", "SCHEDULE" } )
            if( name == x ) return true;

        return false;
    }

    static std::pair< DeckView::const_iterator, DeckView::const_iterator >
    find_section( const Deck& deck, const std::string& keyword ) {

        const auto fn = [&keyword]( const DeckKeyword& kw ) {
            return kw.name() == keyword;
        };

        auto first = std::find_if( deck.begin(), deck.end(), fn );

        if( first == deck.end() )
            return { first, first };

        auto last = std::find_if( first + 1, deck.end(), isSectionDelimiter );

        if( last != deck.end() && last->name() == keyword )
            throw std::invalid_argument( std::string( "Deck contains the '" ) + keyword + "' section multiple times" );

        return { first, last };
    }

    DeckSection::DeckSection( const Deck& deck, const std::string& section )
        : DeckView( find_section( deck, section ) ),
          section_name( section ),
          units( deck.getActiveUnitSystem() )
    {}

    const std::string& DeckSection::name() const {
        return this->section_name;
    }

    const UnitSystem& DeckSection::unitSystem() const {
        return this->units;
    }

    bool DeckSection::hasRUNSPEC(const Deck& deck) { return deck.hasKeyword( "RUNSPEC" ); }
    bool DeckSection::hasGRID(const Deck& deck) { return deck.hasKeyword( "GRID" ); }
    bool DeckSection::hasEDIT(const Deck& deck) { return deck.hasKeyword( "EDIT" ); }
    bool DeckSection::hasPROPS(const Deck& deck) { return deck.hasKeyword( "PROPS" ); }
    bool DeckSection::hasREGIONS(const Deck& deck) { return deck.hasKeyword( "REGIONS" ); }
    bool DeckSection::hasSOLUTION(const Deck& deck) { return deck.hasKeyword( "SOLUTION" ); }
    bool DeckSection::hasSUMMARY(const Deck& deck) { return deck.hasKeyword( "SUMMARY" ); }
    bool DeckSection::hasSCHEDULE(const Deck& deck) { return deck.hasKeyword( "SCHEDULE" ); }

}
