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

#include <vector>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/solventdensitytable.hh>

namespace Ewoms {
        SolventDensityTable SolventDensityTable::serializeObject()
        {
            SolventDensityTable result;
            result.m_tableValues = {1.0, 2.0, 3.0};

            return result;
        }

        void SolventDensityTable::init(const Ewoms::DeckRecord& record )
        {
            m_tableValues = record.getItem("SOLVENT_DENSITY").getSIDoubleData();
        }

        const std::vector<double>& SolventDensityTable::getSolventDensityColumn() const
        {
            return m_tableValues;
        }

        bool SolventDensityTable::operator==(const SolventDensityTable& data) const
        {
            return m_tableValues == data.m_tableValues;
        }

}

