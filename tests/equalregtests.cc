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

#include <ewoms/eclio/parser/parser.hh>

#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

static Ewoms::Deck createDeckInvalidArray() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "EQUALREG\n"
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
        "EQUALREG\n"
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
        "EQUALREG\n"
        "  SATNUM 0.2 10 M / \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckUnInitialized() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "REGIONS\n"
        "EQUALREG\n"
        "  SATNUM 2 10 M / \n"
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

BOOST_AUTO_TEST_CASE(UnInitializedVectorThrows) {
    Ewoms::Deck deck = createDeckUnInitialized();
    BOOST_CHECK_THROW( new Ewoms::EclipseState( deck) , std::invalid_argument );
}

