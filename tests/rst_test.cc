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
#include <iostream>
#include <vector>
#include <unordered_map>

#include <iostream>

#include <ewoms/eclio/io/rst/state.hh>
#include <ewoms/eclio/io/erst.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/errorguard.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/summaryconfig/summaryconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <ewoms/eclio/opmlog/opmlog.hh>
#include <ewoms/eclio/opmlog/streamlog.hh>
#include <ewoms/eclio/opmlog/logutil.hh>

void initLogging() {
    std::shared_ptr<Ewoms::StreamLog> cout_log = std::make_shared<Ewoms::StreamLog>(std::cout, Ewoms::Log::DefaultMessageTypes);
    Ewoms::OpmLog::addBackend( "COUT" , cout_log);
}

/*
  This is a small test application which can be used to check that the Schedule
  object is correctly initialized from a restart file. The program can take
  either one or two commandline arguments:

     rst_test  RESTART_CASE.DATA

  We just verify that the Schedule object can be initialized from
  RESTART_CASE.DATA.

      rst_test CASE.DATA RESTART_CASE.DATA

  The Schedule object initialized from the restart file and the Schedule object
  initialized from the normal case are compared. The restart time configured in
  the second .DATA file must be within the time range covered by the first .DATA
  file.

  In both cases the actual restart file pointed to by the RESTART_CASE.DATA file
  must also be present.
*/

Ewoms::Schedule load_schedule(const std::string& fname, int& report_step) {
    Ewoms::Parser parser;
    auto deck = parser.parseFile(fname);
    Ewoms::EclipseState state(deck);

    const auto& init_config = state.getInitConfig();
    if (init_config.restartRequested()) {
        report_step = init_config.getRestartStep();
        const auto& rst_filename = state.getIOConfig().getRestartFileName( init_config.getRestartRootName(), report_step, false );
        Ewoms::EclIO::ERst rst_file(rst_filename);

        const auto& rst = Ewoms::RestartIO::RstState::load(rst_file, report_step);
        return Ewoms::Schedule(deck, state, &rst);
    } else
        return Ewoms::Schedule(deck, state);
}

Ewoms::Schedule load_schedule(const std::string& fname) {
    int report_step;
    return load_schedule(fname, report_step);
}

int main(int argc, char ** argv) {
    initLogging();
    if (argc == 2)
        load_schedule(argv[1]);
    else {
        int report_step;
        const auto& sched = load_schedule(argv[1]);
        const auto& rst_sched = load_schedule(argv[2], report_step);

        if (Ewoms::Schedule::cmp(sched, rst_sched, report_step) ) {
            std::cout << "Schedule objects were equal!" << std::endl;
            std::exit( EXIT_SUCCESS );
        } else {
            std::cout << "Differences were encountered between the Schedule objects" << std::endl;
            std::exit( EXIT_FAILURE );
        }
    }
}
