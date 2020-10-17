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
#ifndef EWOMS_PARSER_SKPRPOLY_TABLE_H
#define EWOMS_PARSER_SKPRPOLY_TABLE_H

#include <ewoms/eclio/parser/eclipsestate/tables/polyinjtable.hh>
namespace Ewoms {

    class DeckKeyword;

    class SkprpolyTable : public PolyInjTable {
    public:
        SkprpolyTable() = default;
        explicit SkprpolyTable(const DeckKeyword& table);

        static SkprpolyTable serializeObject();

        double referenceConcentration() const;

        const std::vector<std::vector<double>>& getSkinPressures() const;

        bool operator==(const SkprpolyTable& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            this->PolyInjTable::serializeOp(serializer);
            serializer(m_ref_polymer_concentration);
        }

    private:
        double m_ref_polymer_concentration;

    };

}

#endif //EWOMS_PARSER_SKPRPOLY_TABLE_H
