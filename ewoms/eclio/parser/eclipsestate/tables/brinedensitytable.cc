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

#include <vector>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/brinedensitytable.hh>

namespace Ewoms {
        BrineDensityTable BrineDensityTable::serializeObject()
        {
            BrineDensityTable result;
            result.m_tableValues = {1.0, 2.0, 3.0};

            return result;
        }

        void BrineDensityTable::init(const Ewoms::DeckRecord& record )
        {
            m_tableValues = record.getItem("BRINE_DENSITY").getSIDoubleData();
        }

        const std::vector<double>& BrineDensityTable::getBrineDensityColumn() const
        {
            return m_tableValues;
        }

        bool BrineDensityTable::operator==(const BrineDensityTable& data) const
        {
            return m_tableValues == data.m_tableValues;
        }

}

