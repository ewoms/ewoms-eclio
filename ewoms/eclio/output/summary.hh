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
#ifndef EWOMS_OUTPUT_SUMMARY_H
#define EWOMS_OUTPUT_SUMMARY_H

#include <ewoms/eclio/parser/eclipsestate/schedule/well/pavgcalculatorcollection.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/output/data/aquifer.hh>

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Ewoms {
    class EclipseGrid;
    class EclipseState;
    class Schedule;
    class SummaryConfig;
    class SummaryState;
    class Inplace;
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
              const data::WellRates&             well_solution,
              const data::GroupAndNetworkValues& group_and_nwrk_solution,
              GlobalProcessParameters            single_values,
              const Inplace&                     initial_inplace,
              const Inplace&                     inplace,
              const PAvgCalculatorCollection&    ,
              const RegionParameters&            region_values = {},
              const BlockValues&                 block_values  = {},
              const data::Aquifers&              aquifers_values = {}) const;

    void write() const;

    PAvgCalculatorCollection wbp_calculators(std::size_t report_step) const;

private:
    class SummaryImplementation;
    std::unique_ptr<SummaryImplementation> pImpl_;
};

}} // namespace Ewoms::out

#endif //EWOMS_OUTPUT_SUMMARY_H
