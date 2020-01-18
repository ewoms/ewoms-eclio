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

#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
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

