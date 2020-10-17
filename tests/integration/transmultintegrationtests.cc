// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
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

#define BOOST_TEST_MODULE TransMultTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

using namespace Ewoms;

inline std::string pathprefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

BOOST_AUTO_TEST_CASE(MULTFLT_IN_SCHEDULE) {
    Parser parser;
    std::string scheduleFile(pathprefix() + "TRANS/Deck1");
    ParseContext parseContext;
    auto deck = parser.parseFile(scheduleFile);
    EclipseState state(deck);
    const auto& trans = state.getTransMult();
    Schedule schedule(deck, state);
    const Events& events = schedule.getEvents();

    BOOST_CHECK_EQUAL( 0.10 , trans.getMultiplier( 3,2,0,FaceDir::XPlus ));
    BOOST_CHECK_EQUAL( 0.10 , trans.getMultiplier( 2,2,0,FaceDir::XPlus ));
    BOOST_CHECK( events.hasEvent( ScheduleEvents::GEO_MODIFIER , 3 ) );
    {
        const auto& mini_deck = schedule.getModifierDeck(3);
        state.applyModifierDeck( mini_deck );
    }
    BOOST_CHECK_EQUAL( 2.00 , trans.getMultiplier( 2,2,0,FaceDir::XPlus ));
    BOOST_CHECK_EQUAL( 0.10 , trans.getMultiplier( 3,2,0,FaceDir::XPlus ));
}
