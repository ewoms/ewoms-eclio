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

#define BOOST_TEST_MODULE EventTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/events.hh>
#include <ewoms/eclio/utility/timeservice.hh>

BOOST_AUTO_TEST_CASE(CreateEmpty) {
    std::vector<std::time_t> tp = { Ewoms::asTimeT(Ewoms::TimeStampUTC({2010,1,1})) };

    for (int i = 0; i < 11; i++)
        tp.push_back( Ewoms::asTimeT(Ewoms::TimeStampUTC({2010,1,i+2})));

    Ewoms::TimeMap timeMap(tp);
    Ewoms::Events events( timeMap );
    Ewoms::DynamicVector<double> vector(timeMap , 9.99);

    BOOST_CHECK_EQUAL( false , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 10));

    events.addEvent( Ewoms::ScheduleEvents::NEW_WELL ,  0  );
    BOOST_CHECK_EQUAL( true  , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 0));

    events.addEvent( Ewoms::ScheduleEvents::NEW_WELL , 10 );
    BOOST_CHECK_EQUAL( false , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 9));
    BOOST_CHECK_EQUAL( true  , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 10));
    BOOST_CHECK_EQUAL( false , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 11));

    events.addEvent( Ewoms::ScheduleEvents::NEW_WELL , 10 );
    BOOST_CHECK_EQUAL( false , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 9));
    BOOST_CHECK_EQUAL( true  , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 10));
    BOOST_CHECK_EQUAL( false , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 11));

    events.addEvent( Ewoms::ScheduleEvents::WELL_STATUS_CHANGE ,  9 );
    events.addEvent( Ewoms::ScheduleEvents::WELL_STATUS_CHANGE , 10 );
    BOOST_CHECK_EQUAL( false , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 9));
    BOOST_CHECK_EQUAL( true  , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 10));
    BOOST_CHECK_EQUAL( false , events.hasEvent(Ewoms::ScheduleEvents::NEW_WELL , 11));
    BOOST_CHECK_EQUAL( true  , events.hasEvent(Ewoms::ScheduleEvents::WELL_STATUS_CHANGE , 9));
    BOOST_CHECK_EQUAL( true  , events.hasEvent(Ewoms::ScheduleEvents::WELL_STATUS_CHANGE , 10));
}

BOOST_AUTO_TEST_CASE(TestMultiple) {
    const std::time_t startDate = Ewoms::TimeMap::mkdate(2010, 1, 1);
    Ewoms::TimeMap timeMap( { startDate } );
    Ewoms::DynamicVector<double> vector(timeMap , 9.99);
    Ewoms::Events events( timeMap );

    events.addEvent( Ewoms::ScheduleEvents::NEW_WELL , 0 );
    BOOST_CHECK( events.hasEvent( Ewoms::ScheduleEvents::NEW_WELL | Ewoms::ScheduleEvents::NEW_GROUP , 0 ));
}
