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
#include <sstream>

#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/summaryconfig/summaryconfig.hh>
#include <ewoms/eclio/parser/python/python.hh>

inline void loadDeck( const char * deck_file) {
    Ewoms::Parser parser;
    auto python = std::make_shared<Ewoms::Python>();

    auto deck = parser.parseFile(deck_file);
    Ewoms::EclipseState state( deck);
    Ewoms::Schedule schedule( deck, state, python);
    Ewoms::SummaryConfig summary( deck, schedule, state.getTableManager( ), state.aquifer() );
    {
        std::stringstream ss;

        ss << deck;
        auto deck2 = parser.parseString(ss.str());
        if (deck.size() != deck2.size()) {
            std::cerr << "Deck size mismatch original:" << deck.size() << " new: " << deck2.size( ) << std::endl;
            std::exit( 1 );
        }

        for (size_t index=0; index < deck.size(); index++) {
            const auto& kw1 = deck.getKeyword( index );
            const auto& kw2 = deck2.getKeyword( index );

            if (!kw1.equal( kw2 , true , true)) {
                std::cerr << "Keyword " << index << " different " << kw1.name() << " " << kw2.name() << std::endl;
                std::cerr << kw1 << std::endl;
                std::cerr << std::endl << "-----------------------------------------------------------------" << std::endl;
                std::cerr << kw2 << std::endl;
                std::exit( 1 );
            }
        }
    }
}

int main(int argc, char** argv) {
    for (int iarg = 1; iarg < argc; iarg++)
        loadDeck( argv[iarg] );
}

