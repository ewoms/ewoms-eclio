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

#define BOOST_TEST_MODULE DoubHEAD_Vector

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/output/doubhead.hh>

#include <ewoms/eclio/output/intehead.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>

#include <chrono>
#include <ctime>
#include <initializer_list>
#include <numeric>              // partial_sum()
#include <ratio>
#include <vector>

namespace {
    using Day = std::chrono::duration<double,
        std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>
    >;

    std::chrono::time_point<std::chrono::system_clock> startSimulation()
    {
        // 2015-04-09T00:00:00+0000
        auto timePoint = std::tm{};

        timePoint.tm_year = 115;     // 2015
        timePoint.tm_mon  =   4 - 1; // April
        timePoint.tm_mday =   9;     // 9th

        return std::chrono::system_clock::from_time_t(
            ::Ewoms::RestartIO::makeUTCTime(timePoint));
    }

    std::chrono::duration<double, std::chrono::seconds::period> tstep_123()
    {
        return Day{ 123 };
    }

    Ewoms::RestartIO::DoubHEAD::TimeStamp
    makeTStamp(std::chrono::time_point<std::chrono::system_clock>          start,
               std::chrono::duration<double, std::chrono::seconds::period> elapsed)
    {
        return { start, elapsed };
    }
} // Anonymous

BOOST_AUTO_TEST_SUITE(Member_Functions)

BOOST_AUTO_TEST_CASE(Time_Stamp)
{
    const auto dh = Ewoms::RestartIO::DoubHEAD{}
        .timeStamp(makeTStamp(startSimulation(), tstep_123()));

    const auto& v = dh.data();

    // Start + elapsed = current (in days)
    BOOST_CHECK_CLOSE(v[1 - 1] + v[161 - 1], v[162 - 1], 1.0e-10);

    // Elapsed time in days.
    BOOST_CHECK_CLOSE(v[1 - 1], 123.0, 1.0e-10);

    // DateNum(startSimulation()) ==
    //     floor(365.25 * 2015) + day_of_year(=99)
    BOOST_CHECK_CLOSE(v[161 - 1], 736077.0, 1.0e-10);

    // Start + elapsed (days)
    BOOST_CHECK_CLOSE(v[162 - 1], 736200.0, 1.0e-10);
}

BOOST_AUTO_TEST_SUITE_END()
