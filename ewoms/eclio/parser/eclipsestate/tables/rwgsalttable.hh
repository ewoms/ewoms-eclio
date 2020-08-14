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
#ifndef EWOMS_PARSER_RWGSALT_TABLE_H
#define	EWOMS_PARSER_RWGSALT_TABLE_H

#include <vector>

namespace Ewoms {

    class DeckKeyword;

    class RwgsaltTable {
    public:
        RwgsaltTable();

        static RwgsaltTable serializeObject();

        void init(const Ewoms::DeckRecord& record1);
        size_t size() const;
        std::vector<double> getPressureColumn() const;
        std::vector<double> getSaltConcentrationColumn() const;
        std::vector<double> getVaporizedWaterGasRatioColumn() const;
        const std::vector<double>& getTableValues() const;

        bool operator==(const RwgsaltTable& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_tableValues);
        }

    protected:

        std::vector <double> m_tableValues;

    };

}

#endif
