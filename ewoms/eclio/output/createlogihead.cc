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

#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/output/logihead.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>

#include <vector>

// #####################################################################
// Public Interface (createLogiHead()) Below Separator
// ---------------------------------------------------------------------

std::vector<bool>
Ewoms::RestartIO::Helpers::
createLogiHead(const EclipseState& es)
{
    const auto& rspec = es.runspec();
    const auto& tabMgr = es.getTableManager();
    const auto& phases = rspec.phases();
    const auto& wsd   = rspec.wellSegmentDimensions();
    const auto& hystPar = rspec.hysterPar();

    auto pvt = ::Ewoms::RestartIO::LogiHEAD::PVTModel{};

    pvt.isLiveOil = phases.active(::Ewoms::Phase::OIL) &&
        !tabMgr.getPvtoTables().empty();

    pvt.isWetGas = phases.active(::Ewoms::Phase::GAS) &&
        !tabMgr.getPvtgTables().empty();

    pvt.constComprOil = phases.active(::Ewoms::Phase::OIL) &&
        !(pvt.isLiveOil ||
          tabMgr.hasTables("PVDO") ||
          tabMgr.getPvcdoTable().empty());

    const auto lh = LogiHEAD{}
        .variousParam(false, false, wsd.maxSegmentedWells(), hystPar.active())
	.pvtModel(pvt)
        ;

    return lh.data();
}
