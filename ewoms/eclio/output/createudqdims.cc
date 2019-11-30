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

#include <ewoms/eclio/output/aggregateudqdata.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/output/intehead.hh>
#include <ewoms/eclio/output/doubhead.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqinput.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqactive.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/units/units.hh>

#include <chrono>
#include <cstddef>
#include <vector>

namespace {

std::size_t entriesPerIUDQ()
{
    std::size_t no_entries = 3;
    return no_entries;
}

std::size_t entriesPerIUAD()
{
    std::size_t no_entries = 5;
    return no_entries;
}

std::size_t entriesPerZUDN()
{
    std::size_t no_entries = 2;
    return no_entries;
}

std::size_t entriesPerZUDL()
{
    std::size_t no_entries = 16;
    return no_entries;
}

std::size_t noIGphs(const std::vector<int>& inteHead)
{
    std::size_t no_entries = inteHead[20];
    return no_entries;
}

// maximum number of wells
std::size_t nwmaxz(const std::vector<int>& inteHead)
{
        return inteHead[163];
}

// maximum number of groups
std::size_t ngmaxz(const std::vector<int>& inteHead)
{
    return inteHead[20];
}

int noWellUdqs(const Ewoms::Schedule& sched,
               const std::size_t    simStep)
{
    const auto& udqCfg = sched.getUDQConfig(simStep);
    std::size_t i_wudq = 0;
    for (const auto& udq_input : udqCfg.input()) {
        if (udq_input.var_type() ==  Ewoms::UDQVarType::WELL_VAR) {
            i_wudq++;
        }
    }
    return i_wudq;
}

int noGroupUdqs(const Ewoms::Schedule& sched,
               const std::size_t    simStep)
{
    const auto& udqCfg = sched.getUDQConfig(simStep);
    const auto& input = udqCfg.input();
    return std::count_if(input.begin(), input.end(), [](const Ewoms::UDQInput inp) { return (inp.var_type() == Ewoms::UDQVarType::GROUP_VAR); });

}

int noFieldUdqs(const Ewoms::Schedule& sched,
               const std::size_t    simStep)
{
    const auto& udqCfg = sched.getUDQConfig(simStep);
    std::size_t i_fudq = 0;
    for (const auto& udq_input : udqCfg.input()) {
        if (udq_input.var_type() ==  Ewoms::UDQVarType::FIELD_VAR) {
            i_fudq++;
        }
    }
    return i_fudq;
}

} // Anonymous

// #####################################################################
// Public Interface (createUdqDims()) Below Separator
// ---------------------------------------------------------------------

std::vector<int>
Ewoms::RestartIO::Helpers::
createUdqDims(const Schedule&     		sched,
              const std::size_t        	lookup_step,
              const std::vector<int>&   inteHead)
{
    const auto& udqCfg = sched.getUDQConfig(lookup_step);
    const auto& udqActive = sched.udqActive(lookup_step);
    std::vector<int> udqDims;
    udqDims.resize(13,0);

    udqDims[ 0] = udqCfg.size();
    udqDims[ 1] = entriesPerIUDQ();
    udqDims[ 2] = udqActive.IUAD_size();
    udqDims[ 3] = entriesPerIUAD();
    udqDims[ 4] = entriesPerZUDN();
    udqDims[ 5] = entriesPerZUDL();
    udqDims[ 6] = noIGphs(inteHead);
    udqDims[ 7] = udqActive.IUAP_size();
    udqDims[ 8] = nwmaxz(inteHead);
    udqDims[ 9] = noWellUdqs(sched, lookup_step);
    udqDims[10] = ngmaxz(inteHead);
    udqDims[11] = noGroupUdqs(sched, lookup_step);
    udqDims[12] = noFieldUdqs(sched, lookup_step);

    return udqDims;
}
