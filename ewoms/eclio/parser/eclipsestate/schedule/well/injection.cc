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

#include <fnmatch.h>

#include <stdexcept>

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

#include "injection.hh"

namespace Ewoms {
namespace injection {

    double rateToSI(double rawRate, InjectorType wellType, const Ewoms::UnitSystem &unitSystem) {
        switch (wellType) {
        case InjectorType::MULTI:
            // multi-phase controlled injectors are a really funny
            // construct in Eclipse: the quantity controlled for is
            // not physically meaningful, i.e. Eclipse adds up
            // MCFT/day and STB/day.
            throw std::logic_error("There is no generic way to handle multi-phase injectors at this level!");

        case InjectorType::OIL:
        case InjectorType::WATER:
            return unitSystem.to_si( UnitSystem::measure::liquid_surface_rate, rawRate );

        case InjectorType::GAS:
            return unitSystem.to_si( UnitSystem::measure::gas_surface_rate, rawRate );

        default:
            throw std::logic_error("Unknown injector type");
        }
    }

    double rateToSI(double rawRate, Phase wellPhase, const Ewoms::UnitSystem& unitSystem) {
        switch (wellPhase) {
        case Phase::OIL:
        case Phase::WATER:
            return unitSystem.to_si( UnitSystem::measure::liquid_surface_rate, rawRate );

        case Phase::GAS:
            return unitSystem.to_si( UnitSystem::measure::gas_surface_rate, rawRate );

        default:
            throw std::logic_error("Unknown injection phase");
        }
    }
}
}

