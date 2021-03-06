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

#define BOOST_TEST_MODULE UDQ-ACTIONX_Data

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well2.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>

#include <ewoms/eclio/output/aggregateudqdata.hh>
#include <ewoms/eclio/output/aggregateactionxdata.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/output/intehead.hh>
#include <ewoms/eclio/output/vectoritems/intehead.hh>
#include <ewoms/eclio/output/doubhead.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqinput.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqactive.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionx.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/units/units.hh>

#include <ewoms/eclio/io/outputstream.hh>

#include <stdexcept>
#include <utility>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

    Ewoms::Deck first_sim(std::string fname) {
        return Ewoms::Parser{}.parseFile(fname);
    }
}

Ewoms::SummaryState sum_state_TEST1()
    {
        auto state = Ewoms::SummaryState{std::chrono::system_clock::now()};
        state.update_well_var("OPU01", "WWPR", 21.);
        state.update_well_var("OPU02", "WWPR", 22.);
        state.update_well_var("OPL01", "WWPR", 23.);
        state.update_well_var("OPL02", "WWPR", 24.);

        state.update_well_var("OPU01", "WGPR", 230.);
        state.update_well_var("OPU02", "WGPR", 231.);
        state.update_well_var("OPL01", "WGPR", 232.);
        state.update_well_var("OPL02", "WGPR", 233.);

        state.update_group_var("UPPER", "GWPR", 36.);
        state.update_group_var("LOWER", "GWPR", 37.);
        state.update_group_var("TEST",  "GWPR", 73.);

        state.update_group_var("UPPER", "GGPR", 460.);
        state.update_group_var("LOWER", "GGPR", 461.);
        state.update_group_var("TEST",  "GGPR", 821.);

        state.update_group_var("TEST",  "GMWPR", 4);

        state.update("FWPR", 73.);

        state.update("FMWPR", 4);

        return state;
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

BOOST_AUTO_TEST_SUITE(Aggregate_Actionx)

// test constructed UDQ-Actionx restart data
BOOST_AUTO_TEST_CASE (Declared_Actionx_data)
{
    const auto simCase = SimulationCase{first_sim("UDQ_ACTIONX_TEST1.DATA")};

    Ewoms::EclipseState es = simCase.es;
    Ewoms::Runspec rspec   = es.runspec();
    Ewoms::SummaryState st = sum_state_TEST1();
    Ewoms::Schedule     sched = simCase.sched;
    Ewoms::EclipseGrid  grid = simCase.grid;
    const auto& ioConfig = es.getIOConfig();
    //const auto& restart = es.cfg().restart();

    // Report Step 1: 2008-10-10 --> 2011-01-20
    const auto rptStep = std::size_t{1};
    std::string outputDir = "./";
    std::string baseName = "UDQ_ACTIONX_TEST1";
    Ewoms::EclIO::OutputStream::Restart rstFile {
    Ewoms::EclIO::OutputStream::ResultSet { outputDir, baseName },
    rptStep,
    Ewoms::EclIO::OutputStream::Formatted { ioConfig.getFMTOUT() },
	  Ewoms::EclIO::OutputStream::Unified   { ioConfig.getUNIFOUT() }
        };

    double secs_elapsed = 3.1536E07;
    const auto ih = Ewoms::RestartIO::Helpers::createInteHead(es, grid, sched,
                                                secs_elapsed, rptStep, rptStep);
    //set dummy value for next_step_size
    const double next_step_size= 0.1;
    const auto dh = Ewoms::RestartIO::Helpers::createDoubHead(es, sched, rptStep,
                                                            secs_elapsed, next_step_size);

    const auto udqDims = Ewoms::RestartIO::Helpers::createUdqDims(sched, rptStep, ih);
    auto  udqData = Ewoms::RestartIO::Helpers::AggregateUDQData(udqDims);
    udqData.captureDeclaredUDQData(sched, rptStep, st, ih);

    const auto actDims = Ewoms::RestartIO::Helpers::createActionxDims(rspec, sched, rptStep);
    auto  actionxData = Ewoms::RestartIO::Helpers::AggregateActionxData(actDims);
    actionxData.captureDeclaredActionxData(sched, st, actDims, rptStep);

    /*rstFile.write("INTEHEAD", ih);
    rstFile.write("IUDQ", udqData.getIUDQ());
    rstFile.write("IUAD", udqData.getIUAD());
    rstFile.write("IGPH", udqData.getIGPH());
    rstFile.write("IUAP", udqData.getIUAP());
    rstFile.write("ZUDN", udqData.getZUDN());
    rstFile.write("ZUDL", udqData.getZUDL());

    rstFile.write("IACT", actionxData.getIACT());
    rstFile.write("SACT", actionxData.getSACT());
    rstFile.write("ZACT", actionxData.getZACT());
    rstFile.write("ZLACT", actionxData.getZLACT());
    rstFile.write("ZACN", actionxData.getZACN());
    rstFile.write("IACN", actionxData.getIACN());
    rstFile.write("SACN", actionxData.getSACN());  */

    {
        /*
        Check of InteHEAD and DoubHEAD data for UDQ variables

                INTEHEAD

                Intehead[156]  -  The number of ACTIONS
                Intehead[157]  -  The max number of lines of schedule data including ENDACTIO keyword for any ACTION

                ---------------------------------------------------------------------------------------------------------------------]

        */
        const auto rptStep_1 = std::size_t{0};
        const auto ih_1 = Ewoms::RestartIO::Helpers::createInteHead(es, grid, sched,
                                                secs_elapsed, rptStep, rptStep_1);

        BOOST_CHECK_EQUAL(ih_1[156] ,       2);
        BOOST_CHECK_EQUAL(ih_1[157] ,       7);

        const auto rptStep_2 = std::size_t{1};
        const auto ih_2 = Ewoms::RestartIO::Helpers::createInteHead(es, grid, sched,
                                                secs_elapsed, rptStep, rptStep_2);
        BOOST_CHECK_EQUAL(ih_2[156] ,       3);
        BOOST_CHECK_EQUAL(ih_2[157] ,       7);

        const auto rptStep_3 = std::size_t{2};
        const auto ih_3 = Ewoms::RestartIO::Helpers::createInteHead(es, grid, sched,
                                                secs_elapsed, rptStep, rptStep_3);

        BOOST_CHECK_EQUAL(ih_3[156] ,       3);
        BOOST_CHECK_EQUAL(ih_3[157] ,      10);

    }

    {
        /*
        IACT
        --length is equal to 9*the number of ACTIONX keywords
            //item [0]: is unknown, (=0)
            //item [1]: The number of lines of schedule data including ENDACTIO
            //item [2]: is unknown, (=1)
            //item [3]: is unknown, (=7)
            //item [4]: is unknown, (=0)
            //item [5]: The number of times the action is triggered
            //item [6]: is unknown, (=0)
            //item [7]: is unknown, (=0)
            //item [8]: The number of times the action is triggered
        */

        const auto& iAct = actionxData.getIACT();

        auto start = 0*actDims[1];
        BOOST_CHECK_EQUAL(iAct[start + 0] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 1] ,  4);
        BOOST_CHECK_EQUAL(iAct[start + 2] ,  1);
        BOOST_CHECK_EQUAL(iAct[start + 3] ,  7);
        BOOST_CHECK_EQUAL(iAct[start + 4] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 5] , 10);
        BOOST_CHECK_EQUAL(iAct[start + 6] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 7] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 8] ,  3);

        start = 1*actDims[1];
        BOOST_CHECK_EQUAL(iAct[start + 0] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 1] ,  7);
        BOOST_CHECK_EQUAL(iAct[start + 2] ,  1);
        BOOST_CHECK_EQUAL(iAct[start + 3] ,  7);
        BOOST_CHECK_EQUAL(iAct[start + 4] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 5] , 11);
        BOOST_CHECK_EQUAL(iAct[start + 6] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 7] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 8] ,  3);

        start = 2*actDims[1];
        BOOST_CHECK_EQUAL(iAct[start + 0] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 1] ,  4);
        BOOST_CHECK_EQUAL(iAct[start + 2] ,  1);
        BOOST_CHECK_EQUAL(iAct[start + 3] ,  7);
        BOOST_CHECK_EQUAL(iAct[start + 4] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 5] , 13);
        BOOST_CHECK_EQUAL(iAct[start + 6] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 7] ,  0);
        BOOST_CHECK_EQUAL(iAct[start + 8] ,  3);

    }

    {
        /*
        ZACT
        --length 4 times 8-chars pr ACTIONX keyword

        Name of action 4 times 8 chars (up to 8 chars for name)

        */

        const auto& zAct = actionxData.getZACT();

        auto start = 0*actDims[3];
        BOOST_CHECK_EQUAL(zAct[start + 0].c_str() ,   "ACT01   ");

        start = 1*actDims[3];
        BOOST_CHECK_EQUAL(zAct[start + 0].c_str() ,   "ACT02   ");

        start = 2*actDims[3];
        BOOST_CHECK_EQUAL(zAct[start + 0].c_str() ,   "ACT03   ");

    }

    {
     /*
     ZLACT
        -- length = ACTDIMS_item3*(max-over-action of number of lines of data pr ACTION)

        */

        const auto& zLact = actionxData.getZLACT();

        //First action
        auto start_a = 0*actDims[4];
        auto start = start_a + 0*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "WELOPEN ");

        start = start_a + 1*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "   '?' '");
        BOOST_CHECK_EQUAL(zLact[start + 1].c_str() ,   "SHUT' 0 ");
        BOOST_CHECK_EQUAL(zLact[start + 2].c_str() ,   "0 0 /   ");

        start = start_a + 2*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "/       ");

        start = start_a +  3*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "ENDACTIO");

        //Second action
        start_a = 1*actDims[4];
        start = start_a + 0*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "WELOPEN ");

        start = start_a + 1*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "   '?' '");
        BOOST_CHECK_EQUAL(zLact[start + 1].c_str() ,   "SHUT' 0 ");
        BOOST_CHECK_EQUAL(zLact[start + 2].c_str() ,   "0 0 /   ");

        start = start_a + 2*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "/       ");

        start = start_a + 3*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "WELOPEN ");

        start = start_a + 4*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "   'OPL0");
        BOOST_CHECK_EQUAL(zLact[start + 1].c_str() ,   "1' 'OPEN");
        BOOST_CHECK_EQUAL(zLact[start + 2].c_str() ,   "' /     ");

        start = start_a + 5*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "/       ");

        start = start_a +  6*actDims[8];
        BOOST_CHECK_EQUAL(zLact[start + 0].c_str() ,   "ENDACTIO");

    }

        {
     /*
     ZACN
        //(Max number of conditions pr ACTIONX) * ((max no characters pr line = 104) / (8 - characters pr string)(104/8 = 13)

        */

        const auto& zAcn = actionxData.getZACN();

        //First action
        auto start_a = 0*actDims[5];
        auto start = start_a + 0*13;
        BOOST_CHECK_EQUAL(zAcn[start + 0].c_str() ,   "WWPR    ");
        BOOST_CHECK_EQUAL(zAcn[start + 1].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 2].c_str() ,   ">       ");
        BOOST_CHECK_EQUAL(zAcn[start + 3].c_str() ,   "OP*     ");

        start = start_a + 1*13;
        BOOST_CHECK_EQUAL(zAcn[start + 0].c_str() ,   "GMWPR   ");
        BOOST_CHECK_EQUAL(zAcn[start + 1].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 2].c_str() ,   ">       ");
        BOOST_CHECK_EQUAL(zAcn[start + 3].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 4].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 5].c_str() ,   "T*      ");

        start = start_a + 2*13;
        BOOST_CHECK_EQUAL(zAcn[start + 0].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 1].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 2].c_str() ,   ">       ");
        BOOST_CHECK_EQUAL(zAcn[start + 3].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 4].c_str() ,   "        ");

        //Second action
        start_a = 1*actDims[5];
        start = start_a + 0*13;
        BOOST_CHECK_EQUAL(zAcn[start + 0].c_str() ,   "FMWPR   ");
        BOOST_CHECK_EQUAL(zAcn[start + 1].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 2].c_str() ,   ">       ");
        BOOST_CHECK_EQUAL(zAcn[start + 3].c_str() ,   "        ");

        start = start_a + 1*13;
        BOOST_CHECK_EQUAL(zAcn[start + 0].c_str() ,   "WGPR    ");
        BOOST_CHECK_EQUAL(zAcn[start + 1].c_str() ,   "GGPR    ");
        BOOST_CHECK_EQUAL(zAcn[start + 2].c_str() ,   ">       ");
        BOOST_CHECK_EQUAL(zAcn[start + 3].c_str() ,   "OPL02   ");
        BOOST_CHECK_EQUAL(zAcn[start + 4].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 5].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 6].c_str() ,   "LOWER   ");

        start = start_a + 2*13;
        BOOST_CHECK_EQUAL(zAcn[start + 0].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 1].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 2].c_str() ,   ">       ");
        BOOST_CHECK_EQUAL(zAcn[start + 3].c_str() ,   "        ");
        BOOST_CHECK_EQUAL(zAcn[start + 4].c_str() ,   "        ");
    }

        {
        /*
        IACN
        26*Max number of conditions pr ACTIONX

        */

        const auto& iAcn = actionxData.getIACN();

        auto start_a = 0*actDims[6];
        auto start = start_a + 0*26;
        BOOST_CHECK_EQUAL(iAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  2] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  4] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  5] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  6] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  7] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  8] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  9] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 10] ,  2);
        BOOST_CHECK_EQUAL(iAcn[start + 11] ,  8);
        BOOST_CHECK_EQUAL(iAcn[start + 12] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 13] ,  2);
        BOOST_CHECK_EQUAL(iAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 15] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 16] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 17] ,  0);

        start = start_a + 1*26;
        BOOST_CHECK_EQUAL(iAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  2] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  4] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  5] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  6] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  7] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  8] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  9] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 10] ,  3);
        BOOST_CHECK_EQUAL(iAcn[start + 11] ,  8);
        BOOST_CHECK_EQUAL(iAcn[start + 12] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 13] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 15] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 16] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 17] ,  0);

        start_a = 1*actDims[6];
        start = start_a + 0*26;
        BOOST_CHECK_EQUAL(iAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  2] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  4] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  5] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  6] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  7] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  8] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  9] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 10] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 11] ,  8);
        BOOST_CHECK_EQUAL(iAcn[start + 12] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 13] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 15] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 16] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 17] ,  0);

        start = start_a + 1*26;
        BOOST_CHECK_EQUAL(iAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  2] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  4] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  5] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  6] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  7] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  8] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  9] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 10] ,  2);
        BOOST_CHECK_EQUAL(iAcn[start + 11] ,  3);
        BOOST_CHECK_EQUAL(iAcn[start + 12] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 13] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 15] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 16] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 17] ,  0);

        start = start_a + 2*26;
        BOOST_CHECK_EQUAL(iAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  2] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  4] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  5] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  6] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  7] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  8] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start +  9] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 10] , 11);
        BOOST_CHECK_EQUAL(iAcn[start + 11] ,  8);
        BOOST_CHECK_EQUAL(iAcn[start + 12] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 13] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 15] ,  0);
        BOOST_CHECK_EQUAL(iAcn[start + 16] ,  1);
        BOOST_CHECK_EQUAL(iAcn[start + 17] ,  1);

    }

    {
        /*
        SACN
        26*Max number of conditions pr ACTIONX

        */

        const auto& sAcn = actionxData.getSACN();

        auto start_a = 0*actDims[6];
        auto start = start_a + 0*16;
        BOOST_CHECK_EQUAL(sAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  2] , 17);
        BOOST_CHECK_EQUAL(sAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  4] , 24);
        BOOST_CHECK_EQUAL(sAcn[start +  5] , 17);
        BOOST_CHECK_EQUAL(sAcn[start +  6] , 24);
        BOOST_CHECK_EQUAL(sAcn[start +  7] , 17);
        BOOST_CHECK_EQUAL(sAcn[start +  8] , 24);
        BOOST_CHECK_EQUAL(sAcn[start +  9] , 17);
        BOOST_CHECK_EQUAL(sAcn[start + 10] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 11] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 12] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 13] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 15] ,  0);

        start = start_a + 1*16;
        BOOST_CHECK_EQUAL(sAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  2] , 14);
        BOOST_CHECK_EQUAL(sAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  4] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  5] , 14);
        BOOST_CHECK_EQUAL(sAcn[start +  6] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  7] , 14);
        BOOST_CHECK_EQUAL(sAcn[start +  8] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  9] , 14);
        BOOST_CHECK_EQUAL(sAcn[start + 10] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 11] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 12] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 13] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 15] ,  0);

        start = start_a + 2*16;
        BOOST_CHECK_EQUAL(sAcn[start +  0] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  1] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  2] ,  3);
        BOOST_CHECK_EQUAL(sAcn[start +  3] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start +  4] ,  1.E+20);
        BOOST_CHECK_EQUAL(sAcn[start +  5] ,  1.E+20);
        BOOST_CHECK_EQUAL(sAcn[start +  6] ,  1.E+20);
        BOOST_CHECK_EQUAL(sAcn[start +  7] ,  1.E+20);
        BOOST_CHECK_EQUAL(sAcn[start +  8] ,  1.E+20);
        BOOST_CHECK_EQUAL(sAcn[start +  9] ,  1.E+20);
        BOOST_CHECK_EQUAL(sAcn[start + 10] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 11] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 12] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 13] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 14] ,  0);
        BOOST_CHECK_EQUAL(sAcn[start + 15] ,  0);

    }

}

BOOST_AUTO_TEST_SUITE_END()
