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
#ifndef EWOMS_PARSER_PVTO_TABLE_H
#define EWOMS_PARSER_PVTO_TABLE_H

#include <ewoms/eclio/parser/eclipsestate/tables/pvtxtable.hh>

#include <array>
#include <cstddef>
#include <vector>

namespace Ewoms {

    class DeckKeyword;

    class PvtoTable : public PvtxTable {
    public:
        struct FlippedFVF {
            std::size_t i;
            std::array<double, std::size_t{2}> Rs;
            std::array<double, std::size_t{2}> Bo;
        };

        PvtoTable() = default;
        PvtoTable(const DeckKeyword& keyword, size_t tableIdx);

        static PvtoTable serializeObject();

        bool operator==(const PvtoTable& data) const;

        std::vector<FlippedFVF> nonMonotonicSaturatedFVF() const;
    };
}

#endif
