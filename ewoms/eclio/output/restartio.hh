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
#ifndef RESTART_IO_H
#define RESTART_IO_H

#include <ewoms/eclio/output/restartvalue.hh>

#include <string>
#include <utility>
#include <vector>

namespace Ewoms {

    class EclipseGrid;
    class EclipseState;
    class Schedule;
    class UDQState;
    class SummaryState;

} // namespace Ewoms

namespace Ewoms { namespace EclIO { namespace OutputStream {

    class Restart;

}}}

namespace Ewoms { namespace Action {

    class State;

}}

/*
  The two free functions RestartIO::save() and RestartIO::load() can
  be used to save and load reservoir and well state from restart
  files. Observe that these functions 'just do it', i.e. the checking
  of which report step to load from, if output is enabled at all and
  so on is handled by an outer scope.

  If the filename corresponds to unified eclipse restart file,
  i.e. UNRST the functions will seek correctly to the correct report
  step, and truncate in the case of save. For any other filename the
  functions will start reading and writing from file offset zero. If
  the input filename does not correspond to a unified restart file
  there is no consistency checking between filename and report step;
  i.e. these calls:

     load("CASE.X0010" , 99 , ...)
     save("CASE.X0010" , 99 , ...)

   will read from and write to the file "CASE.X0010" - completely ignoring
   the report step argument '99'.
*/
namespace Ewoms { namespace RestartIO {

    void save(EclIO::OutputStream::Restart& rstFile,
              int                           report_step,
              double                        seconds_elapsed,
              RestartValue                  value,
              const EclipseState&           es,
              const EclipseGrid&            grid,
              const Schedule&               schedule,
              const Action::State&          action_state,
              const SummaryState&           sumState,
              const UDQState&               udqState,
              bool                          write_double = false);

    RestartValue load(const std::string&             filename,
                      int                            report_step,
                      Action::State&                 action_state,
                      SummaryState&                  summary_state,
                      const std::vector<RestartKey>& solution_keys,
                      const EclipseState&            es,
                      const EclipseGrid&             grid,
                      const Schedule&                schedule,
                      const std::vector<RestartKey>& extra_keys = {});

}} // namespace Ewoms::RestartIO

#endif  // RESTART_IO_H
