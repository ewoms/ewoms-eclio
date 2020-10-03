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

#define BOOST_TEST_MODULE ReportConfigTest
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/eclipsestate/schedule/rptconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>

Ewoms::Schedule make_schedule(const std::string& sched_string) {
    std::string deck_string = R"(
RUNSPEC
DIMENS
5 5 5 /
OIL
WATER
TABDIMS
/
WELLDIMS
2  10  3  2 /
GRID
DXV
5*100 /
DYV
5*100 /
DZV
5*5 /
TOPS
25*2500 /
PORO
125*0.15 /
PERMX
125*500 /
COPY
'PERMX' 'PERMY' /
'PERMX' 'PERMZ' /
/
MULTIPLY
'PERMZ' 0.1 /
/
PROPS
SWOF
0 0 1 0
1 1 0 0 /

SCHEDULE
)";
    Ewoms::Parser parser;
    auto deck = parser.parseString(deck_string + sched_string);
    Ewoms::EclipseState ecl_state(deck);
    return Ewoms::Schedule(deck, ecl_state);
}

BOOST_AUTO_TEST_CASE(ReportConfig_INVALID) {
    const std::string sched_string1 = R"(
RPTSCHED
  FIPSOL=X
)";

    const std::string sched_string2 = R"(
RPTSCHED
  FIPSOL=-1
)";

    const std::string sched_string3 = R"(
RPTSCHED
  FIPSOL=2.50
)";

    BOOST_CHECK_THROW(make_schedule(sched_string1), std::invalid_argument);
    BOOST_CHECK_THROW(make_schedule(sched_string2), std::invalid_argument);
    BOOST_CHECK_THROW(make_schedule(sched_string3), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(ReportConfig) {
    const std::string sched_string = R"(
DATES
   1 'JAN' 2000 /
/

RPTSCHED
  FIPSOL FIP=3 /

DATES
   1  'FEB' 2000 /
/

RPTSCHED
  FIPSOL FIP=3 NOTHING /

)";
    auto sched = make_schedule(sched_string);

    // Empty initial report configuration
    {
        auto report_config = sched.report_config(0);
        BOOST_CHECK_EQUAL(report_config.size(), 0U);

        BOOST_CHECK(!report_config.contains("FIPFOAM"));
        BOOST_CHECK_THROW( report_config.at("FIPFOAM"), std::out_of_range);
    }

    // Configuration at step 1
    {
        auto report_config = sched.report_config(1);
        BOOST_CHECK_EQUAL( report_config.size() , 2U);

        for (const auto& p : report_config) {
            if (p.first == "FIPSOL")
                BOOST_CHECK_EQUAL(p.second, 1U);

            if (p.first == "FIP")
                BOOST_CHECK_EQUAL(p.second, 3U);
        }

        BOOST_CHECK(!report_config.contains("FIPFOAM"));
        BOOST_CHECK(report_config.contains("FIP"));
        BOOST_CHECK_EQUAL(report_config.at("FIP"), 3U);
        BOOST_CHECK_EQUAL(report_config.at("FIPSOL"), 1U);
    }

    // Configuration at step 2 - the special 'NOTHING' has cleared everything
    {
        auto report_config = sched.report_config(2);
        BOOST_CHECK_EQUAL(report_config.size(), 0U);

        BOOST_CHECK(!report_config.contains("FIPFOAM"));
        BOOST_CHECK_THROW( report_config.at("FIPFOAM"), std::out_of_range);
    }
}
