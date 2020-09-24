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

#ifndef EWOMS_OUTPUT_SUMMARY_H
#define EWOMS_OUTPUT_SUMMARY_H

#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Ewoms {
    class EclipseGrid;
    class EclipseState;
    class Schedule;
    class SummaryConfig;
    class SummaryState;
} // namespace Ewoms

namespace Ewoms { namespace data {
    class WellRates;
    class GroupAndNetworkValues;
}} // namespace Ewoms::data

namespace Ewoms { namespace out {

class Summary {
public:
    using GlobalProcessParameters = std::map<std::string, double>;
    using RegionParameters = std::map<std::string, std::vector<double>>;
    using BlockValues = std::map<std::pair<std::string, int>, double>;

    Summary(const EclipseState&  es,
            const SummaryConfig& sumcfg,
            const EclipseGrid&   grid,
            const Schedule&      sched,
            const std::string&   basename = "");

    ~Summary();

    void add_timestep(const SummaryState& st, const int report_step);

    void eval(SummaryState&                      summary_state,
              const int                          report_step,
              const double                       secs_elapsed,
              const EclipseState&                es,
              const Schedule&                    schedule,
              const data::WellRates&             well_solution,
              const data::GroupAndNetworkValues& group_and_nwrk_solution,
              GlobalProcessParameters            single_values,
              const RegionParameters&            region_values = {},
              const BlockValues&                 block_values  = {}) const;

    void write() const;

private:
    class SummaryImplementation;
    std::unique_ptr<SummaryImplementation> pImpl_;
};

}} // namespace Ewoms::out

#endif //EWOMS_OUTPUT_SUMMARY_H
