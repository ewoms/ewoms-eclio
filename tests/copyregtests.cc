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

#define BOOST_TEST_MODULE CopyRegTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/parser.hh>

#include <ewoms/eclio/parser/deck/section.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

static Ewoms::Deck createDeckInvalidArray1() {
    const char *deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "SATNUM\n"
        "  1000*1 /\n"
        "COPYREG\n"
        "  MISSING SATNUM 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckInvalidArray2() {
    const char *deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "SATNUM\n"
        "  1000*1 /\n"
        "COPYREG\n"
        "  SATNUM MISSING 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckInvalidTypeMismatch() {
    const char *deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "SATNUM\n"
        "  1000*1 /\n"
        "COPYREG\n"
        "  SATNUM PERMX 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckInvalidRegion() {
    const char *deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "SATNUM\n"
        "  1000*1 /\n"
        "COPYREG\n"
        "  SATNUM FLUXNUM 10 MX / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckUnInitialized() {
    const char *deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "REGIONS\n"
        "COPYREG\n"
        "  SATNUM FLUXNUM 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createValidIntDeck() {
    const char *deckData =
        "RUNSPEC\n"
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
        "FLUXNUM\n"
        "  25*3 /\n"
        "REGIONS\n"
        "SATNUM\n"
        "  25*10 /\n"
        "COPYREG\n"
        "  SATNUM FLUXNUM 1    M / \n"
        "/\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(InvalidArrayThrows1) {
    Ewoms::Deck deck = createDeckInvalidArray1();
    BOOST_CHECK_THROW( new Ewoms::EclipseState(deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(InvalidArrayThrows2) {
    Ewoms::Deck deck = createDeckInvalidArray2();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(InvalidRegionThrows) {
    Ewoms::Deck deck = createDeckInvalidRegion();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(UnInitializedVectorThrows) {
    Ewoms::Deck deck = createDeckUnInitialized();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(TypeMismatchThrows) {
    Ewoms::Deck deck = createDeckInvalidTypeMismatch();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(IntSetCorrectly) {
    Ewoms::Deck deck = createValidIntDeck();
    Ewoms::TableManager tm(deck);
    Ewoms::EclipseGrid eg(deck);
    Ewoms::Eclipse3DProperties props(deck, tm, eg);
    const auto& property = props.getIntGridProperty("FLUXNUM").getData();

    for (size_t j = 0; j < 5; j++)
        for (size_t i = 0; i < 5; i++) {
            std::size_t g = eg.getGlobalIndex(i,j,0);
            if (i < 2)
                BOOST_CHECK_EQUAL( 10 , property[g]);
            else
                BOOST_CHECK_EQUAL( 3 , property[g]);
        }
}
