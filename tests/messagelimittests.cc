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

#define BOOST_TEST_MODULE MessageLimitTests

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/messagelimits.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE(MESSAGES) {
    Ewoms::Parser parser;
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "RUNSPEC\n"
            "MESSAGES\n"
            "  5* 10 /\n"
             "GRID\n"
            "MESSAGES\n"
            "  5* 77 /\n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'P1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'P2'       'OP'   5   5 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'I'       'OP'   1   1 1*     'WATER' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'P1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'P2'  5  5   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P2'  5  5   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'I'  1  1   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "MESSAGES\n"
            "  1 2 /\n"
            "DATES             -- 2\n"
            " 15  OKT 2008 / \n"
            "/\n"
            "MESSAGES\n"
            "  10 /\n"
        ;

    auto deck = parser.parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule(deck, grid, fp, runspec);
    const MessageLimits limits = schedule.getMessageLimits();

    BOOST_CHECK_EQUAL( limits.getBugPrintLimit( 0 ) , 77 );   // The pre Schedule initialization

    BOOST_CHECK_EQUAL( limits.getMessagePrintLimit( 1 ) , 1 );
    BOOST_CHECK_EQUAL( limits.getCommentPrintLimit( 1 ) , 2 );
    BOOST_CHECK_EQUAL( limits.getBugPrintLimit( 1 ) , 77 );

    BOOST_CHECK_EQUAL( limits.getMessagePrintLimit( 2 ) , 10 );
    BOOST_CHECK_EQUAL( limits.getCommentPrintLimit( 2 ) , 2  );
    BOOST_CHECK_EQUAL( limits.getBugPrintLimit( 2 ) , 77 );
}
