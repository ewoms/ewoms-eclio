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
#ifndef EWOMS_PARSER_SOLVENTDENSITY_TABLE_H
#define	EWOMS_PARSER_SOLVENTDENSITY_TABLE_H

namespace Ewoms {

    class DeckItem;

    class SolventDensityTable {
    public:
        SolventDensityTable();
        SolventDensityTable(const std::vector<double>& tableValues);

        void init(const Ewoms::DeckRecord& record);
        const std::vector<double>& getSolventDensityColumn() const;

        bool operator==(const SolventDensityTable& data) const;

        std::vector<double>::const_iterator begin() const {
            return m_tableValues.begin();
        }

        std::vector<double>::const_iterator end() const {
            return m_tableValues.end();
        }

        std::size_t size() const {
            return this->m_tableValues.size();
        }

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_tableValues);
        }

    private:
        std::vector<double> m_tableValues;
    };
}

#endif
