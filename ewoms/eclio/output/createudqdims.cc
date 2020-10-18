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

#include <ewoms/eclio/output/aggregateudqdata.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>
#include <ewoms/eclio/output/vectoritems/intehead.hh>

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

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;

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
    std::size_t no_entries = (inteHead[VI::intehead::NO_GROUP_UDQS] > 0) ? inteHead[20] : 0;
    return no_entries;
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
    std::vector<int> udqDims;
    udqDims.resize(13,0);

    udqDims[ 0] = udqCfg.size();
    udqDims[ 1] = entriesPerIUDQ();
    udqDims[ 2] = inteHead[VI::intehead::NO_IUADS];
    udqDims[ 3] = entriesPerIUAD();
    udqDims[ 4] = entriesPerZUDN();
    udqDims[ 5] = entriesPerZUDL();
    udqDims[ 6] = noIGphs(inteHead);
    udqDims[ 7] = inteHead[VI::intehead::NO_IUAPS];
    udqDims[ 8] = inteHead[VI::intehead::NWMAXZ];
    udqDims[ 9] = inteHead[VI::intehead::NO_WELL_UDQS];
    udqDims[10] = inteHead[VI::intehead::NGMAXZ];
    udqDims[11] = inteHead[VI::intehead::NO_GROUP_UDQS];
    udqDims[12] = inteHead[VI::intehead::NO_FIELD_UDQS];

    return udqDims;
}
