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

#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellpolymerproperties.hh>

#include <string>
#include <vector>

namespace Ewoms {

    WellPolymerProperties WellPolymerProperties::serializeObject()
    {
        WellPolymerProperties result;
        result.m_polymerConcentration = 1.0;
        result.m_saltConcentration = 2.0;
        result.m_plymwinjtable = 3;
        result.m_skprwattable = 4;
        result.m_skprpolytable = 5;

        return result;
    }

    bool WellPolymerProperties::operator==(const WellPolymerProperties& other) const {
        if ((m_polymerConcentration == other.m_polymerConcentration) &&
            (m_saltConcentration == other.m_saltConcentration) &&
            (m_plymwinjtable == other.m_plymwinjtable) &&
            (m_skprwattable == other.m_skprwattable) &&
            (m_skprpolytable == other.m_skprpolytable) )
            return true;
        else
            return false;

    }

    void WellPolymerProperties::handleWPOLYMER(const DeckRecord& record) {
        const auto& group_polymer_item = record.getItem("GROUP_POLYMER_CONCENTRATION");
        const auto& group_salt_item = record.getItem("GROUP_SALT_CONCENTRATION");

        if (!group_polymer_item.defaultApplied(0))
            throw std::logic_error("Sorry explicit setting of \'GROUP_POLYMER_CONCENTRATION\' is not supported!");

        if (!group_salt_item.defaultApplied(0))
            throw std::logic_error("Sorry explicit setting of \'GROUP_SALT_CONCENTRATION\' is not supported!");

        this->m_polymerConcentration = record.getItem("POLYMER_CONCENTRATION").get<UDAValue>(0).getSI();
        this->m_saltConcentration = record.getItem("SALT_CONCENTRATION").get<UDAValue>(0).getSI();
    }

    void WellPolymerProperties::handleWPMITAB(const DeckRecord& record) {
        this->m_plymwinjtable = record.getItem("TABLE_NUMBER").get<int>(0);
    }

    void WellPolymerProperties::handleWSKPTAB(const DeckRecord& record) {
        this->m_skprwattable = record.getItem("TABLE_NUMBER_WATER").get<int>(0);
        this->m_skprpolytable = record.getItem("TABLE_NUMBER_POLYMER").get<int>(0);
    }

    bool WellPolymerProperties::operator!=(const WellPolymerProperties& other) const {
        return !(*this == other);
    }
}
