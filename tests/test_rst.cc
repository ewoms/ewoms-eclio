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

#define BOOST_TEST_MODULE test_rst

#include <boost/test/unit_test.hpp>

#include <vector>

#include <ewoms/eclio/output/writerestarthelpers.hh>
#include <ewoms/eclio/output/aggregatewelldata.hh>
#include <ewoms/eclio/output/aggregateconnectiondata.hh>
#include <ewoms/eclio/output/aggregategroupdata.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>
#include <ewoms/eclio/output/vectoritems/intehead.hh>
#include <ewoms/eclio/output/vectoritems/well.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/output/data/wells.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <ewoms/eclio/io/rst/connection.hh>
#include <ewoms/eclio/io/rst/header.hh>
#include <ewoms/eclio/io/rst/group.hh>
#include <ewoms/eclio/io/rst/segment.hh>
#include <ewoms/eclio/io/rst/well.hh>
#include <ewoms/eclio/io/rst/state.hh>

namespace {
    Ewoms::Deck first_sim()
    {
        // Mostly copy of tests/FIRST_SIM.DATA
        const auto input = std::string {
            R"~(
RUNSPEC
OIL
GAS
WATER
DISGAS
VAPOIL
UNIFOUT
UNIFIN
DIMENS
 10 10 10 /

GRID
DXV
10*0.25 /
DYV
10*0.25 /
DZV
10*0.25 /
TOPS
100*0.25 /

PORO
1000*0.2 /

SOLUTION

START             -- 0
1 NOV 1979 /

SCHEDULE
RPTRST
BASIC=1
/
DATES             -- 1
 10  OKT 2008 /
/
WELSPECS
      'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
      'OP_2'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
/
COMPDAT
      'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
      'OP_2'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 /
      'OP_1'  9  9   3   3 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/
WCONPROD
      'OP_1' 'OPEN' 'ORAT' 20000  4* 1000 /
/
WCONINJE
      'OP_2' 'GAS' 'OPEN' 'RATE' 100 200 400 /
/

DATES             -- 2
 20  JAN 2011 /
/
WELSPECS
      'OP_3'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
/
COMPDAT
      'OP_3'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/
WCONPROD
      'OP_3' 'OPEN' 'ORAT' 20000  4* 1000 /
/
WCONINJE
      'OP_2' 'WATER' 'OPEN' 'RATE' 100 200 400 /
/

DATES             -- 3
 15  JUN 2013 /
/
COMPDAT
      'OP_2'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
      'OP_1'  9  9   7  7 'SHUT' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/

DATES             -- 4
 22  APR 2014 /
/
WELSPECS
      'OP_4'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
/
COMPDAT
      'OP_4'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
      'OP_3'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/
WCONPROD
      'OP_4' 'OPEN' 'ORAT' 20000  4* 1000 /
/

DATES             -- 5
 30  AUG 2014 /
/
WELSPECS
      'OP_5'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
/
COMPDAT
      'OP_5'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/
WCONPROD
      'OP_5' 'OPEN' 'ORAT' 20000  4* 1000 /
/

DATES             -- 6
 15  SEP 2014 /
/
WCONPROD
      'OP_3' 'SHUT' 'ORAT' 20000  4* 1000 /
/

DATES             -- 7
 9  OCT 2014 /
/
WELSPECS
      'OP_6'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
/
COMPDAT
      'OP_6'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/
WCONPROD
      'OP_6' 'OPEN' 'ORAT' 20000  4* 1000 /
/
TSTEP            -- 8
10 /
)~" };

        return Ewoms::Parser{}.parseString(input);
    }
} // namespace

struct SimulationCase
{
    explicit SimulationCase(const Ewoms::Deck& deck)
        : es    { deck }
        , grid  { deck }
        , sched { deck, es}
    {}

    // Order requirement: 'es' must be declared/initialised before 'sched'.
    Ewoms::EclipseState es;
    Ewoms::EclipseGrid  grid;
    Ewoms::Schedule     sched;
};

// =====================================================================
BOOST_AUTO_TEST_CASE(group_test) {
    const auto simCase = SimulationCase{first_sim()};
    const auto& units = simCase.es.getUnits();
    // Report Step 2: 2011-01-20 --> 2013-06-15
    const auto rptStep = std::size_t{2};
    const auto sim_step = rptStep - 1;
    Ewoms::SummaryState sumState(std::chrono::system_clock::now());

    const auto ih = Ewoms::RestartIO::Helpers::createInteHead(simCase.es,
                                                            simCase.grid,
                                                            simCase.sched,
                                                            0,
                                                            sim_step,
                                                            sim_step,
                                                            sim_step);

    const auto lh = Ewoms::RestartIO::Helpers::createLogiHead(simCase.es);

    const auto dh = Ewoms::RestartIO::Helpers::createDoubHead(simCase.es,
                                                            simCase.sched,
                                                            sim_step,
                                                            0, 0);

    auto groupData = Ewoms::RestartIO::Helpers::AggregateGroupData(ih);
    groupData.captureDeclaredGroupData(simCase.sched, units, sim_step, sumState, ih);

    const auto& igrp = groupData.getIGroup();
    const auto& sgrp = groupData.getSGroup();
    const auto& xgrp = groupData.getXGroup();
    const auto& zgrp8 = groupData.getZGroup();

    Ewoms::UnitSystem unit_system(Ewoms::UnitSystem::UnitType::UNIT_TYPE_METRIC);
    std::vector<std::string> zgrp;
    for (const auto& s8: zgrp8)
        zgrp.push_back(s8.c_str());

    Ewoms::RestartIO::RstHeader header(ih,lh,dh);
    for (int ig=0; ig < header.ngroup; ig++) {
        std::size_t zgrp_offset = ig * header.nzgrpz;
        std::size_t igrp_offset = ig * header.nigrpz;
        std::size_t sgrp_offset = ig * header.nsgrpz;
        std::size_t xgrp_offset = ig * header.nxgrpz;

        Ewoms::RestartIO::RstGroup group(unit_system,
                                       zgrp.data() + zgrp_offset,
                                       igrp.data() + igrp_offset,
                                       sgrp.data() + sgrp_offset,
                                       xgrp.data() + xgrp_offset);
    }
}

BOOST_AUTO_TEST_CASE(State_test) {
    const auto simCase = SimulationCase{first_sim()};
    const auto& units = simCase.es.getUnits();
    // Report Step 2: 2011-01-20 --> 2013-06-15
    const auto rptStep = std::size_t{4};
    const auto sim_step = rptStep - 1;
    Ewoms::SummaryState sumState(std::chrono::system_clock::now());

    const auto ih = Ewoms::RestartIO::Helpers::createInteHead(simCase.es,
                                                            simCase.grid,
                                                            simCase.sched,
                                                            0,
                                                            sim_step,
                                                            sim_step,
                                                            sim_step);

    const auto lh = Ewoms::RestartIO::Helpers::createLogiHead(simCase.es);

    const auto dh = Ewoms::RestartIO::Helpers::createDoubHead(simCase.es,
                                                            simCase.sched,
                                                            sim_step,
                                                            0, 0);

    auto wellData = Ewoms::RestartIO::Helpers::AggregateWellData(ih);
    wellData.captureDeclaredWellData(simCase.sched, units, sim_step, sumState, ih);
    wellData.captureDynamicWellData(simCase.sched, sim_step, {} , sumState);

    auto connectionData = Ewoms::RestartIO::Helpers::AggregateConnectionData(ih);
    connectionData.captureDeclaredConnData(simCase.sched, simCase.grid, units, {} , sim_step);

    auto groupData = Ewoms::RestartIO::Helpers::AggregateGroupData(ih);
    groupData.captureDeclaredGroupData(simCase.sched, units, sim_step, sumState, ih);

    const auto& iwel = wellData.getIWell();
    const auto& swel = wellData.getSWell();
    const auto& xwel = wellData.getXWell();
    const auto& zwel8 = wellData.getZWell();

    const auto& icon = connectionData.getIConn();
    const auto& scon = connectionData.getSConn();
    const auto& xcon = connectionData.getXConn();

    const auto& zgrp8 = groupData.getZGroup();
    const auto& igrp = groupData.getIGroup();
    const auto& sgrp = groupData.getSGroup();
    const auto& xgrp = groupData.getXGroup();

    std::vector<std::string> zwel;
    for (const auto& s8: zwel8)
        zwel.push_back(s8.c_str());

    std::vector<std::string> zgrp;
    for (const auto& s8: zgrp8)
        zgrp.push_back(s8.c_str());

    Ewoms::RestartIO::RstState state(units,
                                   ih, lh, dh,
                                   zgrp, igrp, sgrp, xgrp,
                                   zwel, iwel, swel, xwel,
                                   icon, scon, xcon);

    const auto& well = state.get_well("OP_3");
    BOOST_CHECK_THROW(well.segment(10), std::invalid_argument);
}
