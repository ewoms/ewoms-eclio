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

#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellbrineproperties.hh>

#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/udavalue.hh>

Ewoms::WellBrineProperties Ewoms::WellBrineProperties::serializeObject()
{
    Ewoms::WellBrineProperties result;
    result.m_saltConcentration = 1.0;

    return result;
}

void Ewoms::WellBrineProperties::handleWSALT(const DeckRecord& rec)
{
    this->m_saltConcentration = rec.getItem("CONCENTRATION").get<UDAValue>(0).get<double>();
}

bool Ewoms::WellBrineProperties::operator!=(const WellBrineProperties& other) const
{
    return this->m_saltConcentration != other.m_saltConcentration;
}

bool Ewoms::WellBrineProperties::operator==(const WellBrineProperties& other) const
{
    return this->m_saltConcentration == other.m_saltConcentration;
}
