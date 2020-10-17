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

#include <ewoms/eclio/parser/eclipsestate/tables/standardcond.hh>
#include <ewoms/eclio/parser/parserkeywords/s.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

namespace Ewoms {

StandardCond::StandardCond() {
    using ST = ParserKeywords::STCOND;
    double input_temp = ST::TEMPERATURE::defaultValue;
    double input_pressure = ST::PRESSURE::defaultValue;
    UnitSystem units( UnitSystem::UnitType::UNIT_TYPE_METRIC );
    this->temperature = units.to_si(UnitSystem::measure::temperature, input_temp);
    this->pressure = units.to_si(UnitSystem::measure::pressure, input_pressure);
}

StandardCond StandardCond::serializeObject()
{
    StandardCond result;
    result.temperature = 1.0;
    result.pressure = 2.0;

    return result;
}

}
