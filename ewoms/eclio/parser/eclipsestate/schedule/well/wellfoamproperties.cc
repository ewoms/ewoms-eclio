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

#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellfoamproperties.hh>

#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/udavalue.hh>

Ewoms::WellFoamProperties Ewoms::WellFoamProperties::serializeObject()
{
    Ewoms::WellFoamProperties result;
    result.m_foamConcentration = 1.0;

    return result;
}

void Ewoms::WellFoamProperties::handleWFOAM(const DeckRecord& rec)
{
    this->m_foamConcentration = rec.getItem("FOAM_CONCENTRATION").get<UDAValue>(0).getSI();
}

bool Ewoms::WellFoamProperties::operator==(const WellFoamProperties& other) const
{
    return this->m_foamConcentration == other.m_foamConcentration;
}

bool Ewoms::WellFoamProperties::operator!=(const WellFoamProperties& other) const
{
    return this->m_foamConcentration != other.m_foamConcentration;
}
