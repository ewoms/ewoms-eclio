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

#define BOOST_TEST_MODULE WellSolventTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>

using namespace Ewoms;

static Deck createDeckWithOutSolvent() {
    Ewoms::Parser parser;
    std::string input =
            "GRID\n"
            "PERMX\n"
            "   1000*0.25/\n"
            "COPY\n"
            "  PERMX PERMY /\n"
            "  PERMX PERMZ /\n"
            "/\n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   2   2  1*       \'OIL\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'WATER' 'OPEN' 'BHP' 1 2 3/\n/\n";

    return parser.parseString(input);
}

static Deck createDeckWithGasInjector() {
    Ewoms::Parser parser;
    std::string input =
            "GRID\n"
            "PERMX\n"
            "   1000*0.25/\n"
            "COPY\n"
            "  PERMX PERMY /\n"
            "  PERMX PERMZ /\n"
            "/\n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   1   1  1*       \'GAS\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'GAS' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n";

    return parser.parseString(input);
}

static Deck createDeckWithDynamicWSOLVENT() {
    Ewoms::Parser parser;
    std::string input =
            "START             -- 0 \n"
            "1 JAN 2000 / \n"
            "GRID\n"
            "PERMX\n"
            "   1000*0.25/\n"
            "COPY\n"
            "  PERMX PERMY /\n"
            "  PERMX PERMZ /\n"
            "/\n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   1   1  1*       \'GAS\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'GAS' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "DATES             -- 2\n"
            " 1  MAY 2000 / \n"
            "/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n"
            "DATES             -- 3,4\n"
            " 1  JUL 2000 / \n"
            " 1  AUG 2000 / \n"
            "/\n"
            "WSOLVENT\n"
            "     'W_1'        0 / \n "
            "/\n";

    return parser.parseString(input);
}

static Deck createDeckWithOilInjector() {
    Ewoms::Parser parser;
    std::string input =
            "GRID\n"
            "PERMX\n"
            "   1000*0.25/\n"
            "COPY\n"
            "  PERMX PERMY /\n"
            "  PERMX PERMZ /\n"
            "/\n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   2   2  1*       \'OIL\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'OIL' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n";

    return parser.parseString(input);
}

static Deck createDeckWithWaterInjector() {
    Ewoms::Parser parser;
    std::string input =
            "GRID\n"
            "PERMX\n"
            "   1000*0.25/\n"
            "COPY\n"
            "  PERMX PERMY /\n"
            "  PERMX PERMZ /\n"
            "/\n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   2   2  1*       \'OIL\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'WATER' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n";

    return parser.parseString(input);
}
BOOST_AUTO_TEST_CASE(TestNoSolvent) {
    auto deck = createDeckWithOutSolvent();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
    Runspec runspec(deck);
    Schedule schedule(deck, grid , fp, runspec);
    BOOST_CHECK(!deck.hasKeyword("WSOLVENT"));
}

BOOST_AUTO_TEST_CASE(TestGasInjector) {
    auto deck = createDeckWithGasInjector();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
    Runspec runspec(deck);
    Schedule schedule(deck, grid , fp, runspec);
    BOOST_CHECK(deck.hasKeyword("WSOLVENT"));
}

BOOST_AUTO_TEST_CASE(TestDynamicWSOLVENT) {
    auto deck = createDeckWithDynamicWSOLVENT();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
    Runspec runspec(deck);
    Schedule schedule(deck, grid , fp, runspec);
    BOOST_CHECK(deck.hasKeyword("WSOLVENT"));
    const auto& keyword = deck.getKeyword("WSOLVENT");
    BOOST_CHECK_EQUAL(keyword.size(),1);
    const auto& record = keyword.getRecord(0);
    const std::string& well_name = record.getItem("WELL").getTrimmedString(0);
    BOOST_CHECK_EQUAL(well_name, "W_1");
    BOOST_CHECK_EQUAL(schedule.getWell("W_1", 0).getSolventFraction(),0); //default 0
    BOOST_CHECK_EQUAL(schedule.getWell("W_1", 1).getSolventFraction(),1);
    BOOST_CHECK_EQUAL(schedule.getWell("W_1", 2).getSolventFraction(),1);
    BOOST_CHECK_EQUAL(schedule.getWell("W_1", 3).getSolventFraction(),0);
}

BOOST_AUTO_TEST_CASE(TestOilInjector) {
    auto deck = createDeckWithOilInjector();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
    Runspec runspec(deck);
    BOOST_CHECK_THROW (Schedule(deck , grid , fp, runspec), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(TestWaterInjector) {
    auto deck = createDeckWithWaterInjector();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
    Runspec runspec(deck);
    BOOST_CHECK_THROW (Schedule(deck, grid , fp, runspec), std::invalid_argument);
}
