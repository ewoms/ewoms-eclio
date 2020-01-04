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

#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>
#include <cstdio>

#define BOOST_TEST_MODULE EqualRegTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/units/units.hh>

#include <ewoms/eclio/parser/parser.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

static Ewoms::Deck createDeckInvalidArray() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "ADDREG\n"
        "  MISSING 10 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckInvalidRegion() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "ADDREG\n"
        "  MISSING 10 10 MX / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckInvalidValue() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "MULTNUM \n"
        " 1000*1 /\n"
        "ADDREG\n"
        "  SATNUM 0.2 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckUnInitializedRegion() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "SATNUM \n"
        " 1000*1 /\n"
        "REGIONS\n"
        "ADDREG\n"
        "  SATNUM 2 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckUnInitializedVector() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "MULTNUM \n"
        " 1000*1 /\n"
        "REGIONS\n"
        "ADDREG\n"
        "  SATNUM 2 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createValidIntDeck() {
    const char* deckData =
        "RUNSPEC\n"
        "GRIDOPTS\n"
        "  'YES'  2 /\n"
        "\n"
        "DIMENS\n"
        " 5 5 1 /\n"
        "GRID\n"
        "DX\n"
        "25*0.25 /\n"
        "DY\n"
        "25*0.25 /\n"
        "DZ\n"
        "25*0.25 /\n"
        "TOPS\n"
        "25*0.25 /\n"
        "MULTNUM \n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "/\n"
        "SATNUM\n"
        " 25*1 \n"
        "/\n"
        "ADDREG\n"
        "  SATNUM 11 1    M / \n"
        "  SATNUM 20 2      / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createValidPERMXDeck() {
    const char* deckData =
        "RUNSPEC\n"
        "GRIDOPTS\n"
        "  'YES'  2 /\n"
        "\n"
        "DIMENS\n"
        " 5 5 1 /\n"
        "GRID\n"
        "DX\n"
        "25*0.25 /\n"
        "DY\n"
        "25*0.25 /\n"
        "DZ\n"
        "25*0.25 /\n"
        "TOPS\n"
        "25*0.25 /\n"
        "MULTNUM \n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "1  1  2  2 2\n"
        "/\n"
        "BOX\n"
        "  1 2  1 5 1 1 / \n"
        "PERMZ\n"
        "  10*1 /\n"
        "ENDBOX\n"
        "BOX\n"
        "  3 5  1 5 1 1 / \n"
        "PERMZ\n"
        "  15*2 /\n"
        "ENDBOX\n"
        "PERMX\n"
        "25*1 /\n"
        "ADDREG\n"
        "  PERMX 1 1     / \n"
        "  PERMX 3 2     / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(InvalidArrayThrows) {
    Ewoms::Deck deck = createDeckInvalidArray();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(InvalidRegionThrows) {
    Ewoms::Deck deck = createDeckInvalidRegion();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(ExpectedIntThrows) {
    Ewoms::Deck deck = createDeckInvalidValue();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(UnInitializedRegionThrows) {
    Ewoms::Deck deck = createDeckUnInitializedRegion();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(UnInitializedVectorThrows) {
    Ewoms::Deck deck = createDeckUnInitializedVector();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(IntSetCorrectly) {
    Ewoms::Deck deck = createValidIntDeck();
    Ewoms::TableManager tm(deck);
    Ewoms::EclipseGrid eg(deck);
    Ewoms::Eclipse3DProperties props(deck, tm, eg);
    const auto& property_data = props.getIntGridProperty("SATNUM").getData();

    for (size_t j = 0; j < 5; j++)
        for (size_t i = 0; i < 5; i++) {
            if (i < 2)
                BOOST_CHECK_EQUAL( 12 , property_data[eg.getGlobalIndex(i,j,0)]);
            else
                BOOST_CHECK_EQUAL( 21 , property_data[eg.getGlobalIndex(i,j,0)]);
        }

}

BOOST_AUTO_TEST_CASE(UnitAppliedCorrectly) {
    Ewoms::Deck deck = createValidPERMXDeck();
    Ewoms::TableManager tm(deck);
    Ewoms::EclipseGrid eg(deck);
    Ewoms::Eclipse3DProperties props(deck, tm, eg);
    const auto& permx_data = props.getDoubleGridProperty("PERMX").getData();

    for (size_t j=0; j< 5; j++)
        for (size_t i = 0; i < 5; i++) {
            if (i < 2)
                BOOST_CHECK_CLOSE( 2 * Ewoms::Metric::Permeability , permx_data[eg.getGlobalIndex(i,j,0)], 0.0001);
            else
                BOOST_CHECK_CLOSE( 4 * Ewoms::Metric::Permeability , permx_data[eg.getGlobalIndex(i,j,0)], 0.0001);
        }
}
