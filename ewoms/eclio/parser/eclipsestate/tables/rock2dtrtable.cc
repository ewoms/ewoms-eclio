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
#include <ewoms/eclio/parser/eclipsestate/tables/rock2dtrtable.hh>

namespace Ewoms {

        Rock2dtrTable::Rock2dtrTable()
        {
        }

        Rock2dtrTable::Rock2dtrTable(const std::vector<std::vector<double>>& transMultValues,
                                     const std::vector<double>& pressureValues)
            : m_transMultValues(transMultValues)
            , m_pressureValues(pressureValues)
        {
        }

        void Rock2dtrTable::init(const DeckRecord& record, size_t /* tableIdx */)
        {
            m_pressureValues.push_back(record.getItem("PRESSURE").getSIDoubleData()[0]);
            m_transMultValues.push_back(record.getItem("TRANSMULT").getSIDoubleData());
        }

        size_t Rock2dtrTable::size() const
        {
            return m_pressureValues.size();
        }

        size_t Rock2dtrTable::sizeMultValues() const
        {
            return m_transMultValues[0].size();
        }

        double Rock2dtrTable::getPressureValue(size_t index) const
        {
            return m_pressureValues[index];
        }

        double Rock2dtrTable::getTransMultValue(size_t pressureIndex, size_t saturationIndex) const
        {
            return m_transMultValues[pressureIndex][saturationIndex];
        }

        bool Rock2dtrTable::operator==(const Rock2dtrTable& data) const
        {
              return this->m_transMultValues == data.m_transMultValues &&
                     this->m_pressureValues == data.m_pressureValues;
        }

}

