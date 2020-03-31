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

#ifndef EWOMS_WRITE_RESTART_HELPERS_H
#define EWOMS_WRITE_RESTART_HELPERS_H

#include <vector>

// Forward declarations

namespace Ewoms {

    class Runspec;
    class EclipseGrid;
    class EclipseState;
    class Schedule;
    class Well;
    class UnitSystem;
    class UDQActive;

} // namespace Ewoms

namespace Ewoms { namespace RestartIO { namespace Helpers {

    std::vector<double>
    createDoubHead(const EclipseState& es,
                   const Schedule&     sched,
                   const std::size_t   lookup_step,
                   const double        simTime,
                   const double        nextTimeStep);

    std::vector<int>
    createInteHead(const EclipseState& es,
                   const EclipseGrid&  grid,
                   const Schedule&     sched,
                   const double        simTime,
                   const int           num_solver_steps,
                   const int           report_step,
                   const int           lookup_step);

    std::vector<bool>
    createLogiHead(const EclipseState& es);

    std::vector<int>
    createUdqDims(const Schedule&     		sched,
                  const std::size_t       lookup_step,
                  const std::vector<int>& inteHead);

    std::vector<int>
    createActionxDims(  const Runspec&      rspec,
                        const Schedule&     sched,
                        const std::size_t   simStep);

}}} // Ewoms::RestartIO::Helpers

#endif  // EWOMS_WRITE_RESTART_HELPERS_H
