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

#include <stdexcept>
#include <iostream>

#define BOOST_TEST_MODULE GeoModifiersTests
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parserkeywords/m.hh>
#include <ewoms/eclio/parser/inputerroraction.hh>
#include <ewoms/eclio/parser/errorguard.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/events.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE( CheckUnsoppertedInSCHEDULE ) {
    const char * deckString =
        "START\n"
        " 10 'JAN' 2000 /\n"
        "RUNSPEC\n"
        "DIMENS\n"
        "  10 10 10 / \n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DY\n"
        "1000*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "SCHEDULE\n"
        "TSTEP -- 1,2\n"
        "   10 10/\n"
        "MULTFLT\n"
        "   'F1' 100 /\n"
        "/\n"
        "MULTFLT\n"
        "   'F2' 77 /\n"
        "/\n"
        "TSTEP  -- 3,4\n"
        "   10 10/\n"
        "\n";

    Parser parser(true);
    ParseContext parseContext;
    ErrorGuard errors;
    auto deck = parser.parseString( deckString, parseContext, errors);
    EclipseGrid grid( deck );
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);

    parseContext.update( ParseContext::UNSUPPORTED_SCHEDULE_GEO_MODIFIER , InputError::IGNORE );
    {
        Runspec runspec ( deck );
        Schedule schedule( deck, grid , fp, runspec , parseContext, errors);
        auto events = schedule.getEvents( );
        BOOST_CHECK_EQUAL( false , events.hasEvent( ScheduleEvents::GEO_MODIFIER , 1 ));
        BOOST_CHECK_EQUAL( true  , events.hasEvent( ScheduleEvents::GEO_MODIFIER , 2 ));
        BOOST_CHECK_EQUAL( false , events.hasEvent( ScheduleEvents::GEO_MODIFIER , 3 ));

        BOOST_CHECK_EQUAL( 0U, schedule.getModifierDeck(1).size() );
        BOOST_CHECK_EQUAL( 0U, schedule.getModifierDeck(3).size() );

        const Deck& multflt_deck = schedule.getModifierDeck(2);
        BOOST_CHECK_EQUAL( 2U , multflt_deck.size());
        BOOST_CHECK( multflt_deck.hasKeyword<ParserKeywords::MULTFLT>() );

        const auto& multflt1 = multflt_deck.getKeyword(0);
        BOOST_CHECK_EQUAL( 1U , multflt1.size( ) );

        const auto& record0 = multflt1.getRecord( 0 );
        BOOST_CHECK_EQUAL( 100.0  , record0.getItem<ParserKeywords::MULTFLT::factor>().get< double >(0));
        BOOST_CHECK_EQUAL( "F1" , record0.getItem<ParserKeywords::MULTFLT::fault>().get< std::string >(0));

        const auto& multflt2 = multflt_deck.getKeyword(1);
        BOOST_CHECK_EQUAL( 1U , multflt2.size( ) );

        const auto& record1 = multflt2.getRecord( 0 );
        BOOST_CHECK_EQUAL( 77.0  , record1.getItem<ParserKeywords::MULTFLT::factor>().get< double >(0));
        BOOST_CHECK_EQUAL( "F2" , record1.getItem<ParserKeywords::MULTFLT::fault>().get< std::string >(0));
    }
}
