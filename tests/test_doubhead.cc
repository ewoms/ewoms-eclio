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

#define BOOST_TEST_MODULE DoubHEAD_Vector

#include <boost/test/unit_test.hpp>
#include <ewoms/eclio/parser/units/units.hh>

#include <ewoms/eclio/output/doubhead.hh>
#include <ewoms/eclio/output/vectoritems/doubhead.hh>

#include <ewoms/eclio/output/intehead.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <chrono>
#include <ctime>
#include <initializer_list>
#include <numeric>              // partial_sum()
#include <ratio>
#include <vector>

namespace {

    Ewoms::Deck first_sim(std::string fname) {
        return Ewoms::Parser{}.parseFile(fname);
    }
}

//int main(int argc, char* argv[])
struct SimulationCase
{
    explicit SimulationCase(const Ewoms::Deck& deck)
        : es   { deck }
        , grid { deck }
        , sched{ deck, es }
    {}

    // Order requirement: 'es' must be declared/initialised before 'sched'.
    Ewoms::EclipseState es;
    Ewoms::EclipseGrid  grid;
    Ewoms::Schedule     sched;

};

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

    double getTimeConv(const ::Ewoms::UnitSystem& us)
    {
        switch (us.getType()) {
        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_METRIC:
            return static_cast<double>(Ewoms::Metric::Time);

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_FIELD:
            return static_cast<double>(Ewoms::Field::Time);

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_LAB:
            return static_cast<double>(Ewoms::Lab::Time);

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_PVT_M:
            return static_cast<double>(Ewoms::PVT_M::Time);

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_INPUT:
            throw std::invalid_argument {
                "Cannot Run Simulation With Non-Standard Units"
            };
        }

        throw std::invalid_argument("Unknown unit type specified");
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

BOOST_AUTO_TEST_CASE(Wsegiter)
{
    const auto simCase = SimulationCase{first_sim("0A4_GRCTRL_LRAT_LRAT_GGR_BASE_MODEL2_MSW_ALL.DATA")};

    Ewoms::EclipseState es    = simCase.es;
    Ewoms::Schedule     sched = simCase.sched;

    const auto& usys  = es.getDeckUnitSystem();
    const auto  tconv = getTimeConv(usys);

    const std::size_t lookup_step = 1;

    const auto dh = Ewoms::RestartIO::DoubHEAD{}
        .tuningParameters(sched.getTuning(lookup_step), tconv);

    const auto& v = dh.data();

    namespace VI = Ewoms::RestartIO::Helpers::VectorItems;

    BOOST_CHECK_EQUAL(v[VI::WsegRedFac], 0.3);
    BOOST_CHECK_EQUAL(v[VI::WsegIncFac], 2.0);

}

BOOST_AUTO_TEST_SUITE_END()
