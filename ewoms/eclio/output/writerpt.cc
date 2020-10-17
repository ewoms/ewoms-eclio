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

#include <ewoms/eclio/output/writerpt.hh>

#include <functional>
#include <unordered_map>

namespace Ewoms {
namespace RptIO {

    using report_function = std::function<void(std::ostream&, unsigned, const Schedule&, const EclipseGrid&, const UnitSystem&, std::size_t)>;

    static const std::unordered_map<std::string, report_function> report_functions {
        { "WELSPECS", workers::write_WELSPECS },
    };

    void write_report(
        std::ostream& os,
        const std::string& report,
        unsigned value,
        const Ewoms::Schedule& schedule,
        const Ewoms::EclipseGrid& grid,
        const Ewoms::UnitSystem& unit_system,
        std::size_t report_step
    ) {
        const auto function { report_functions.find(report) } ;
        if (function != report_functions.end()) {
            function->second(os, value, schedule, grid, unit_system, report_step);
        }
    }
}}
