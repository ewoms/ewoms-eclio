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
#ifndef EWOMS_PARSER_ROCK2DTR_TABLE_H
#define	EWOMS_PARSER_ROCK2DTR_TABLE_H

#include <vector>

namespace Ewoms {

    class DeckRecord;

    class Rock2dtrTable {
    public:
        Rock2dtrTable();
        Rock2dtrTable(const std::vector<std::vector<double>>& transMultValues,
                      const std::vector<double>& pressureValues);
        void init(const Ewoms::DeckRecord& record, size_t tableIdx);
        size_t size() const;
        size_t sizeMultValues() const;
        double getPressureValue(size_t index) const;
        double getTransMultValue(size_t pressureIndex, size_t saturationIndex ) const;

        const std::vector<std::vector<double>>& transMultValues() const
        { return m_transMultValues; }

        const std::vector<double>& pressureValues() const
        { return m_pressureValues; }

        bool operator==(const Rock2dtrTable& data) const;

    protected:
        std::vector< std::vector <double> > m_transMultValues;
        std::vector< double > m_pressureValues;

    };

}

#endif