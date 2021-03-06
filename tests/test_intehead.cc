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

#define BOOST_TEST_MODULE InteHEAD_Vector

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/output/intehead.hh>

#include <ewoms/eclio/output/vectoritems/intehead.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <ewoms/eclio/io/rst/header.hh>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <iterator>
#include <numeric>              // partial_sum()
#include <string>
#include <vector>

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;

namespace {
    std::vector<double> elapsedTime(const Ewoms::TimeMap& tmap)
    {
        auto elapsed = std::vector<double>{};

        elapsed.reserve(tmap.numTimesteps() + 1);
        elapsed.push_back(0.0);

        for (auto nstep = tmap.numTimesteps(),
                  step  = 0*nstep; step < nstep; ++step)
        {
            elapsed.push_back(tmap.getTimeStepLength(step));
        }

        std::partial_sum(std::begin(elapsed), std::end(elapsed),
                         std::begin(elapsed));

        return elapsed;
    }

    void expectDate(const Ewoms::RestartIO::InteHEAD::TimePoint& tp,
                    const int year, const int month, const int day)
    {
        BOOST_CHECK_EQUAL(tp.year  , year);
        BOOST_CHECK_EQUAL(tp.month , month);
        BOOST_CHECK_EQUAL(tp.day   , day);

        BOOST_CHECK_EQUAL(tp.hour        , 0);
        BOOST_CHECK_EQUAL(tp.minute      , 0);
        BOOST_CHECK_EQUAL(tp.second      , 0);
        BOOST_CHECK_EQUAL(tp.microseconds, 0);
    }
} // Anonymous

BOOST_AUTO_TEST_SUITE(Member_Functions)

BOOST_AUTO_TEST_CASE(Dimensions_Individual)
{
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .dimensions(100, 60, 15);

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NX], 100);
    BOOST_CHECK_EQUAL(v[VI::intehead::NY],  60);
    BOOST_CHECK_EQUAL(v[VI::intehead::NZ],  15);
}

BOOST_AUTO_TEST_CASE(Dimensions_Array)
{
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .dimensions({ {100, 60, 15} });

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NX], 100);
    BOOST_CHECK_EQUAL(v[VI::intehead::NY],  60);
    BOOST_CHECK_EQUAL(v[VI::intehead::NZ],  15);
}

BOOST_AUTO_TEST_CASE(NumActive)
{
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .numActive(72390);

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NACTIV], 72390);
}

BOOST_AUTO_TEST_CASE(UnitConventions)
{
    auto ih = Ewoms::RestartIO::InteHEAD{};

    // Metric
    {
        ih.unitConventions(Ewoms::UnitSystem::newMETRIC());

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::UNIT], 1);
    }

    // Field
    {
        ih.unitConventions(Ewoms::UnitSystem::newFIELD());

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::UNIT], 2);
    }

    // Lab
    {
        ih.unitConventions(Ewoms::UnitSystem::newLAB());

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::UNIT], 3);
    }

    // PVT-M
    {
        ih.unitConventions(Ewoms::UnitSystem::newPVT_M());

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::UNIT], 4);
    }
}

BOOST_AUTO_TEST_CASE(WellTableDimensions)
{
    const auto numWells        = 17;
    const auto maxPerf         = 29;
    const auto maxWellsInGroup  =  3;
    const auto maxGroupInField = 14;
    const auto maxWellsInField = 25;

    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .wellTableDimensions({
            numWells, maxPerf, maxWellsInGroup, maxGroupInField, maxWellsInField
        });

    const auto& v = ih.data();
    const auto nwgmax = std::max(maxWellsInGroup, maxGroupInField);

    BOOST_CHECK_EQUAL(v[VI::intehead::NWELLS], numWells);
    BOOST_CHECK_EQUAL(v[VI::intehead::NCWMAX], maxPerf);
    BOOST_CHECK_EQUAL(v[VI::intehead::NWGMAX], nwgmax);
    BOOST_CHECK_EQUAL(v[VI::intehead::NGMAXZ], maxGroupInField + 1);
    //BOOST_CHECK_EQUAL(v[VI::intehead::NWMAXZ], maxWellsInField);
}

BOOST_AUTO_TEST_CASE(CalendarDate)
{
    // 2015-04-09T11:22:33.987654+0000

    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .calendarDate({
            2015, 4, 9, 11, 22, 33, 987654,
        });

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[67 - 1], 2015); // Year
    BOOST_CHECK_EQUAL(v[66 - 1],    4); // Month
    BOOST_CHECK_EQUAL(v[65 - 1],    9); // Day

    BOOST_CHECK_EQUAL(v[207 - 1], 11); // Hour
    BOOST_CHECK_EQUAL(v[208 - 1], 22); // Minute
    BOOST_CHECK_EQUAL(v[411 - 1], 33987654); // Second (in microseconds)
}

BOOST_AUTO_TEST_CASE(ActivePhases)
{
    using Ph = Ewoms::RestartIO::InteHEAD::Phases;
    auto  ih = Ewoms::RestartIO::InteHEAD{};

    // Oil
    {
        ih.activePhases(Ph{ 1, 0, 0 });

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::PHASE], 1);
    }

    // Water
    {
        ih.activePhases(Ph{ 0, 1, 0 });

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::PHASE], 2);
    }

    // Gas
    {
        ih.activePhases(Ph{ 0, 0, 1 });

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::PHASE], 4);
    }

    // Oil/Water
    {
        ih.activePhases(Ph{ 1, 1, 0 });

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::PHASE], 3);
    }

    // Oil/Gas
    {
        ih.activePhases(Ph{ 1, 0, 1 });

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::PHASE], 5);
    }

    // Water/Gas
    {
        ih.activePhases(Ph{ 0, 1, 1 });

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::PHASE], 6);
    }

    // Oil/Water/Gas
    {
        ih.activePhases(Ph{ 1, 1, 1 });

        const auto& v = ih.data();

        BOOST_CHECK_EQUAL(v[VI::intehead::PHASE], 7);
    }
}

BOOST_AUTO_TEST_CASE(NWell_Parameters)
{
    const auto ih = Ewoms::RestartIO::InteHEAD{}
      .params_NWELZ(27, 18, 28, 1);

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NIWELZ], 27);
    BOOST_CHECK_EQUAL(v[VI::intehead::NSWELZ], 18);
    BOOST_CHECK_EQUAL(v[VI::intehead::NXWELZ], 28);
    BOOST_CHECK_EQUAL(v[VI::intehead::NZWELZ],  1);
}

BOOST_AUTO_TEST_CASE(NConn_Parameters)
{
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .params_NCON(31, 41, 59);

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NICONZ], 31);
    BOOST_CHECK_EQUAL(v[VI::intehead::NSCONZ], 41);
    BOOST_CHECK_EQUAL(v[VI::intehead::NXCONZ], 59);
}

BOOST_AUTO_TEST_CASE(GroupSize_Parameters)
{
    // https://oeis.org/A001620
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .params_GRPZ({
            { 577, 215, 664, 901 }
        });

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NIGRPZ], 577);
    BOOST_CHECK_EQUAL(v[VI::intehead::NSGRPZ], 215);
    BOOST_CHECK_EQUAL(v[VI::intehead::NXGRPZ], 664);
    BOOST_CHECK_EQUAL(v[VI::intehead::NZGRPZ], 901);
}

BOOST_AUTO_TEST_CASE(Analytic_Aquifer_Parameters)
{
    // https://oeis.org/A001622
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .params_NAAQZ(1, 61, 803, 3988, 74989, 484820, 4586834);

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NCAMAX], 1);
    BOOST_CHECK_EQUAL(v[VI::intehead::NIAAQZ], 61);
    BOOST_CHECK_EQUAL(v[VI::intehead::NSAAQZ], 803);
    BOOST_CHECK_EQUAL(v[VI::intehead::NXAAQZ], 3988);
    BOOST_CHECK_EQUAL(v[VI::intehead::NICAQZ], 74989);
    BOOST_CHECK_EQUAL(v[VI::intehead::NSCAQZ], 484820);
    BOOST_CHECK_EQUAL(v[VI::intehead::NACAQZ], 4586834);
}

BOOST_AUTO_TEST_CASE(Time_and_report_step)
{
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .stepParam(12, 2);

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NUM_SOLVER_STEPS], 12); // TSTEP (1st argument to stepParam)
    BOOST_CHECK_EQUAL(v[VI::intehead::REPORT_STEP],       2); // REP_STEP (2nd argument to stepParam)
}

BOOST_AUTO_TEST_CASE(Tuning_param)
{
    const auto newtmx	= 17;
    const auto newtmn	=  5;
    const auto litmax	= 102;
    const auto litmin	= 20;
    const auto mxwsit	= 8;
    const auto mxwpit	= 6;
    const auto wseg_max_restart   = 49;

    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .tuningParam({
            newtmx, newtmn, litmax, litmin, mxwsit, mxwpit, wseg_max_restart
        });

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NEWTMX], newtmx);
    BOOST_CHECK_EQUAL(v[VI::intehead::NEWTMN], newtmn);
    BOOST_CHECK_EQUAL(v[VI::intehead::LITMAX], litmax);
    BOOST_CHECK_EQUAL(v[VI::intehead::LITMIN], litmin);
    BOOST_CHECK_EQUAL(v[VI::intehead::MXWSIT], mxwsit);
    BOOST_CHECK_EQUAL(v[VI::intehead::MXWPIT], mxwpit);
    BOOST_CHECK_EQUAL(v[VI::intehead::WSEGITR_IT2], wseg_max_restart);
}

BOOST_AUTO_TEST_CASE(Various_Parameters)
{
    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .variousParam(2015, 100);

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::VERSION], 2015); // VERSION
    BOOST_CHECK_EQUAL(v[VI::intehead::IPROG], 100);    // IPROG
    BOOST_CHECK_EQUAL(v[ 76],   5); // IH_076
    BOOST_CHECK_EQUAL(v[101],   1); // IH_101
    BOOST_CHECK_EQUAL(v[103],   1); // IH_103
}

BOOST_AUTO_TEST_CASE(wellSegDimensions)
{
    const auto nsegwl = 3;
    const auto nswlmx = 4;
    const auto nsegmx = 5;
    const auto nlbrmx = 6;
    const auto nisegz = 7;
    const auto nrsegz = 8;
    const auto nilbrz = 9;

    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .wellSegDimensions({
            nsegwl, nswlmx, nsegmx, nlbrmx, nisegz, nrsegz, nilbrz
        });

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NSEGWL], nsegwl);
    BOOST_CHECK_EQUAL(v[VI::intehead::NSWLMX], nswlmx);
    BOOST_CHECK_EQUAL(v[VI::intehead::NSEGMX], nsegmx);
    BOOST_CHECK_EQUAL(v[VI::intehead::NLBRMX], nlbrmx);
    BOOST_CHECK_EQUAL(v[VI::intehead::NISEGZ], nisegz);
    BOOST_CHECK_EQUAL(v[VI::intehead::NRSEGZ], nrsegz);
    BOOST_CHECK_EQUAL(v[VI::intehead::NILBRZ], nilbrz);
}

BOOST_AUTO_TEST_CASE(regionDimensions)
{
    const auto ntfip  = 12;
    const auto nmfipr = 22;
    const auto nrfreg = 5;
    const auto ntfreg = 6;
    const auto nplmix = 7;

    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .regionDimensions({
            ntfip, nmfipr, nrfreg, ntfreg, nplmix
        });

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NTFIP], ntfip);
    BOOST_CHECK_EQUAL(v[VI::intehead::NMFIPR], nmfipr);
}

BOOST_AUTO_TEST_CASE(ngroups)
{
    const auto ngroup  = 8;

    const auto ih = Ewoms::RestartIO::InteHEAD{}
        .ngroups({ ngroup });

    const auto& v = ih.data();

    BOOST_CHECK_EQUAL(v[VI::intehead::NGRP], ngroup);
}
BOOST_AUTO_TEST_CASE(SimulationDate)
{
    const auto input = std::string { R"(
RUNSPEC

START
  1 JAN 2000
/

SCHEDULE

DATES
  1 'JAN' 2001 /
/

TSTEP
--Advance the simulater for TEN years:
  10*365.0D0 /
)"  };

    const auto tmap = ::Ewoms::TimeMap {
        ::Ewoms::Parser{}.parseString(input)
    };

    const auto start   = tmap.getStartTime(0);
    const auto elapsed = elapsedTime(tmap);

    auto checkDate = [start, &elapsed]
        (const std::vector<double>::size_type i,
         const std::array<int, 3>&            expectYMD) -> void
    {
        using ::Ewoms::RestartIO::getSimulationTimePoint;

        expectDate(getSimulationTimePoint(start, elapsed[i]),
                   expectYMD[0], expectYMD[1], expectYMD[2]);
    };

    // START
    checkDate(0, {{ 2000, 1, 1 }});  // Start   == 2000-01-01

    // DATES (2000 being leap year is immaterial)
    checkDate(1, {{ 2001, 1, 1 }});  // RStep 1 == 2000-01-01 -> 2001-01-01

    // TSTEP
    checkDate(2, {{ 2002, 1, 1 }});  // RStep 2 == 2001-01-01 -> 2002-01-01
    checkDate(3, {{ 2003, 1, 1 }});  // RStep 3 == 2002-01-01 -> 2003-01-01
    checkDate(4, {{ 2004, 1, 1 }});  // RStep 4 == 2003-01-01 -> 2004-01-01

    // Leap year: 2004
    checkDate(5, {{ 2004, 12, 31 }}); // RStep 5 == 2004-01-01 -> 2004-12-31
    checkDate(6, {{ 2005, 12, 31 }}); // RStep 6 == 2004-12-31 -> 2005-12-31
    checkDate(7, {{ 2006, 12, 31 }}); // RStep 7 == 2005-12-31 -> 2006-12-31
    checkDate(8, {{ 2007, 12, 31 }}); // RStep 8 == 2006-12-31 -> 2007-12-31

    // Leap year: 2008
    checkDate( 9, {{ 2008, 12, 30 }}); // RStep  9 == 2007-12-31 -> 2008-12-30
    checkDate(10, {{ 2009, 12, 30 }}); // RStep 10 == 2008-12-30 -> 2009-12-30
    checkDate(11, {{ 2010, 12, 30 }}); // RStep 11 == 2009-12-30 -> 2010-12-30
}

BOOST_AUTO_TEST_SUITE_END() // Member_Functions

// =====================================================================

BOOST_AUTO_TEST_SUITE(Transfer_Protocol)

BOOST_AUTO_TEST_CASE(TestHeader) {
    using Ph = Ewoms::RestartIO::InteHEAD::Phases;

    const auto nx = 10;
    const auto ny = 11;
    const auto nz = 12;
    const auto nactive = 1345;
    const auto numWells        = 17;
    const auto maxPerf         = 29;
    const auto maxWellsInGroup  =  3;
    const auto maxGroupInField = 14;
    const auto maxWellsInField = 25;
    const auto year = 2010;
    const auto month = 1;
    const auto mday = 27;
    const auto hour = 11;
    const auto minute = 22;
    const auto seconds = 30;
    const auto mseconds = 1234;
    const auto total_mseconds = seconds * 1000000 + mseconds;
    const auto phase_sum = 7;
    const auto niwelz = 10;
    const auto nswelz = 11;
    const auto nxwelz = 12;
    const auto nzwelz = 13;
    const auto niconz = 77;
    const auto nsconz = 88;
    const auto nxconz = 99;
    const auto nigrpz = 21;
    const auto nsgrpz = 22;
    const auto nxgrpz = 23;
    const auto nzgrpz = 24;
    const auto ncamax = 1;
    const auto niaaqz = 11;
    const auto nsaaqz = 111;
    const auto nxaaqz = 11111;
    const auto nicaqz = 111111;
    const auto nscaqz = 1111111;
    const auto nacaqz = 11111111;
    const auto tstep  = 78;
    const auto report_step = 12;
    const auto newtmx	= 17;
    const auto newtmn	=  5;
    const auto litmax	= 102;
    const auto litmin	= 20;
    const auto mxwsit	= 8;
    const auto mxwpit	= 6;
    const auto version = 2015;
    const auto iprog = 100;
    const auto nsegwl = 3;
    const auto nswlmx = 4;
    const auto nsegmx = 5;
    const auto nlbrmx = 6;
    const auto nisegz = 7;
    const auto nrsegz = 8;
    const auto nilbrz = 9;
    const auto ntfip  = 12;
    const auto nmfipr = 22;
    const auto ngroup  = 8;

    auto unit_system = Ewoms::UnitSystem::newMETRIC();
    auto ih = Ewoms::RestartIO::InteHEAD{}
         .dimensions(nx, ny, nz)
         .numActive(nactive)
         .unitConventions(unit_system)
         .wellTableDimensions({ numWells, maxPerf, maxWellsInGroup, maxGroupInField, maxWellsInField})
         .calendarDate({year, month, mday, hour, minute, seconds, mseconds})
         .activePhases(Ph{1,1,1})
         .params_NWELZ(niwelz, nswelz, nxwelz, nzwelz)
         .params_NCON(niconz, nsconz, nxconz)
         .params_GRPZ({{nigrpz, nsgrpz, nxgrpz, nzgrpz}})
         .params_NAAQZ(ncamax, niaaqz, nsaaqz, nxaaqz, nicaqz, nscaqz, nacaqz)
         .stepParam(tstep, report_step)
         .tuningParam({newtmx, newtmn, litmax, litmin, mxwsit, mxwpit, 0})
         .variousParam(version, iprog)
         .wellSegDimensions({nsegwl, nswlmx, nsegmx, nlbrmx, nisegz, nrsegz, nilbrz})
         .regionDimensions({ntfip, nmfipr, 0,0,0})
         .ngroups({ngroup});

    Ewoms::RestartIO::RstHeader header(unit_system, ih.data(), std::vector<bool>(100), std::vector<double>(1000));
    BOOST_CHECK_EQUAL(header.nx, nx);
    BOOST_CHECK_EQUAL(header.ny, ny);
    BOOST_CHECK_EQUAL(header.nactive, nactive);
    BOOST_CHECK_EQUAL(header.num_wells, numWells);
    BOOST_CHECK_EQUAL(header.ncwmax, maxPerf);
    BOOST_CHECK_EQUAL(header.max_wells_in_group, std::max(maxWellsInGroup , maxGroupInField));
    BOOST_CHECK_EQUAL(header.max_groups_in_field, maxGroupInField + 1);
    BOOST_CHECK_EQUAL(header.max_wells_in_field, maxWellsInField);
    BOOST_CHECK_EQUAL(header.year, year);
    BOOST_CHECK_EQUAL(header.month, month);
    BOOST_CHECK_EQUAL(header.mday, mday);
    BOOST_CHECK_EQUAL(header.hour, hour);
    BOOST_CHECK_EQUAL(header.minute, minute);
    BOOST_CHECK_EQUAL(header.microsecond, total_mseconds);
    BOOST_CHECK_EQUAL(header.phase_sum , phase_sum);
    BOOST_CHECK_EQUAL(header.niwelz, niwelz);
    BOOST_CHECK_EQUAL(header.nswelz, nswelz);
    BOOST_CHECK_EQUAL(header.nxwelz, nxwelz);
    BOOST_CHECK_EQUAL(header.nzwelz, nzwelz);
    BOOST_CHECK_EQUAL(header.niconz, niconz);
    BOOST_CHECK_EQUAL(header.nsconz, nsconz);
    BOOST_CHECK_EQUAL(header.nxconz, nxconz);
    BOOST_CHECK_EQUAL(header.nigrpz, nigrpz);
    BOOST_CHECK_EQUAL(header.nsgrpz, nsgrpz);
    BOOST_CHECK_EQUAL(header.nxgrpz, nxgrpz);
    BOOST_CHECK_EQUAL(header.nzgrpz, nzgrpz);
    BOOST_CHECK_EQUAL(header.ncamax, ncamax);
    BOOST_CHECK_EQUAL(header.niaaqz, niaaqz);
    BOOST_CHECK_EQUAL(header.nsaaqz, nsaaqz);
    BOOST_CHECK_EQUAL(header.nxaaqz, nxaaqz);
    BOOST_CHECK_EQUAL(header.nicaqz, nicaqz);
    BOOST_CHECK_EQUAL(header.nscaqz, nscaqz);
    BOOST_CHECK_EQUAL(header.nacaqz, nacaqz);
    BOOST_CHECK_EQUAL(header.tstep, tstep);
    BOOST_CHECK_EQUAL(header.report_step, report_step);
    BOOST_CHECK_EQUAL(header.newtmx, newtmx);
    BOOST_CHECK_EQUAL(header.newtmn, newtmn);
    BOOST_CHECK_EQUAL(header.litmax, litmax);
    BOOST_CHECK_EQUAL(header.litmin, litmin);
    BOOST_CHECK_EQUAL(header.mxwsit, mxwsit);
    BOOST_CHECK_EQUAL(header.mxwpit, mxwpit);
    BOOST_CHECK_EQUAL(header.version, version);
    BOOST_CHECK_EQUAL(header.iprog, iprog);
    BOOST_CHECK_EQUAL(header.nsegwl, nsegwl);
    BOOST_CHECK_EQUAL(header.nswlmx, nswlmx);
    BOOST_CHECK_EQUAL(header.nsegmx, nsegmx);
    BOOST_CHECK_EQUAL(header.nlbrmx, nlbrmx);
    BOOST_CHECK_EQUAL(header.nisegz, nisegz);
    BOOST_CHECK_EQUAL(header.nilbrz, nilbrz);
    BOOST_CHECK_EQUAL(header.ntfip, ntfip);
    BOOST_CHECK_EQUAL(header.nmfipr, nmfipr);
    BOOST_CHECK_EQUAL(header.ngroup, ngroup);
}

BOOST_AUTO_TEST_SUITE_END() // Transfer_Protocol
