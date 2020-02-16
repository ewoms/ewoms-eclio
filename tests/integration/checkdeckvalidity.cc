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

#define BOOST_TEST_MODULE ParserIntegrationTests
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/checkdeck.hh>

BOOST_AUTO_TEST_CASE( KeywordInCorrectSection ) {
    Ewoms::Parser parser;
    Ewoms::ParseContext parseContext;
    Ewoms::ErrorGuard errorGuard;

    {
        const char *correctDeckString =
            "RUNSPEC\n"
            "DIMENS\n"
            "3 3 3 /\n"
            "GRID\n"
            "DXV\n"
            "1 1 1 /\n"
            "DYV\n"
            "1 1 1 /\n"
            "DZV\n"
            "1 1 1 /\n"
            "TOPS\n"
            "9*100 /\n"
            "BOX\n"
            "1 3 1 3 1 3 /\n"
            "PROPS\n"
            "SOLUTION\n"
            "SCHEDULE\n";

        auto deck = parser.parseString(correctDeckString);
        BOOST_CHECK(Ewoms::checkDeck(deck, parser, parseContext, errorGuard));
    }

    {
        // wrong section ordering
        const char *correctDeckString =
            "GRID\n"
            "RUNSPEC\n"
            "PROPS\n"
            "SOLUTION\n"
            "SCHEDULE\n";

        auto deck = parser.parseString(correctDeckString);
        BOOST_CHECK(!Ewoms::checkDeck(deck, parser, parseContext, errorGuard));
        BOOST_CHECK(Ewoms::checkDeck(deck, parser, parseContext, errorGuard, ~Ewoms::SectionTopology));
    }

    {
        // the BOX keyword is in a section where it's not supposed to be
        const char *incorrectDeckString =
            "RUNSPEC\n"
            "BOX\n"
            "1 3 1 3 1 3 /\n"
            "DIMENS\n"
            "3 3 3 /\n"
            "GRID\n"
            "DXV\n"
            "1 1 1 /\n"
            "DYV\n"
            "1 1 1 /\n"
            "DZV\n"
            "1 1 1 /\n"
            "TOPS\n"
            "9*100 /\n"
            "PROPS\n"
            "SOLUTION\n"
            "SCHEDULE\n";

        auto deck = parser.parseString(incorrectDeckString);
        BOOST_CHECK(!Ewoms::checkDeck(deck, parser, parseContext, errorGuard));

        // this is supposed to succeed as we don't ensure that all keywords are in the
        // correct section
        BOOST_CHECK(Ewoms::checkDeck(deck, parser, parseContext, errorGuard, Ewoms::SectionTopology));

        // this fails because of the incorrect BOX keyword
        BOOST_CHECK(!Ewoms::checkDeck(deck, parser, parseContext, errorGuard, Ewoms::SectionTopology | Ewoms::KeywordSection));
    }
}
