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

#define BOOST_TEST_MODULE ResinsightIntegrationTests
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/section.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/errorguard.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/parserkeywords/f.hh>
#include <ewoms/eclio/parser/parserkeywords/g.hh>
#include <ewoms/eclio/parser/parserkeywords/s.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultcollection.hh>

using namespace Ewoms;

std::string deck_string = R"(
SPECGRID
 20  20 10 /

IGNORED
Have no clue /
how to /
parse This keyword/
/

GRID

FAULTS
  'F1'  1  1  1  4   1  4  'X' /
  'F2'  5  5  1  4   1  4  'X-' /
/

And then comes more crap??!

-- And a valid keyword:
TABDIMS
 1 2 3 /

And it ends with crap?!

)";

BOOST_AUTO_TEST_CASE( test_parse ) {
    Parser parser(false);
    ParseContext parseContext;
    ErrorGuard errors;

    parseContext.update( ParseContext::PARSE_UNKNOWN_KEYWORD , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_TEXT , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_SLASH , InputError::IGNORE );

    parser.addKeyword<ParserKeywords::SPECGRID>();
    parser.addKeyword<ParserKeywords::FAULTS>();

    auto deck = parser.parseString(deck_string, parseContext, errors);

    BOOST_CHECK( deck.hasKeyword<ParserKeywords::SPECGRID>() );
    BOOST_CHECK( deck.hasKeyword<ParserKeywords::FAULTS>() );

    BOOST_CHECK_NO_THROW( GridDims{deck} );
    BOOST_CHECK_NO_THROW( GridDims{deck}.getCartesianSize() );
}

BOOST_AUTO_TEST_CASE( test_state ) {
    Parser parser(false);
    ParseContext parseContext;
    ErrorGuard errors;

    parseContext.update( ParseContext::PARSE_UNKNOWN_KEYWORD , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_TEXT , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_SLASH , InputError::IGNORE );

    parser.addKeyword<ParserKeywords::SPECGRID>();
    parser.addKeyword<ParserKeywords::FAULTS>();
    parser.addKeyword<ParserKeywords::GRID>();
    auto deck = parser.parseString(deck_string, parseContext, errors);

    GridDims grid(deck);
    GRIDSection gsec(deck);
    FaultCollection faults(gsec, grid);
    BOOST_CHECK_EQUAL( grid.getNX(), 20U);
    BOOST_CHECK_EQUAL( grid.getNY(), 20U);
    BOOST_CHECK_EQUAL( grid.getNZ(), 10U);
    BOOST_CHECK_EQUAL( faults.size(), 2U);
}
