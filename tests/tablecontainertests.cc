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

#define BOOST_TEST_MODULE TableContainerTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablecontainer.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/swoftable.hh>

#include <string>
#include <memory>

inline Ewoms::Deck createSWOFDeck() {
    const char *deckData =
        "TABDIMS\n"
        " 2 /\n"
        "\n"
        "SWOF\n"
        " 1 2 3 4\n"
        " 5 6 7 8 /\n"
        " 9 10 11 12 /\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData);
}

BOOST_AUTO_TEST_CASE( CreateContainer ) {
    std::vector<std::string> columnNames{"A", "B", "C", "D"};
    auto deck = createSWOFDeck();
    Ewoms::TableContainer container(10);
    BOOST_CHECK( container.empty() );
    BOOST_CHECK_EQUAL( 0 , container.size() );
    BOOST_CHECK_EQUAL( false , container.hasTable( 1 ));

    std::shared_ptr<Ewoms::SimpleTable> table = std::make_shared<Ewoms::SwofTable>( deck.getKeyword("SWOF").getRecord(0).getItem(0), false );
    BOOST_CHECK_THROW( container.addTable( 10 , table ), std::invalid_argument );
    container.addTable( 6 , table );
    BOOST_CHECK_EQUAL( 1 , container.size() );

    BOOST_CHECK_EQUAL( table.get() , &(container[6]));
    BOOST_CHECK_EQUAL( table.get() , &(container[9]));

    BOOST_CHECK_THROW( container[5] , std::invalid_argument );
    BOOST_CHECK_THROW( container[10] , std::invalid_argument );
}

