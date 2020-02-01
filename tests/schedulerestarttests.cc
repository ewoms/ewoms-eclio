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

#define BOOST_TEST_MODULE ScheduleTests

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/rftconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/oilvaporizationproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellconnections.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/io/rst/state.hh>
#include <ewoms/eclio/io/erst.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE(LoadRestartSim) {
    Parser parser;
    auto deck = parser.parseFile("SPE1CASE2.DATA");
    auto restart_deck = parser.parseFile("SPE1CASE2_RESTART.DATA");
    EclIO::ERst rst_file("SPECASE2.X0060");
    auto rst_state = RestartIO::RstState::load(rst_file, 60);

    EclipseState ecl_state(deck);
    Schedule sched(deck, ecl_state);
    Schedule restart_sched(deck, ecl_state, &rst_state);
}
