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
#ifndef EWOMS_PARSER_ROCK2D_TABLE_H
#define	EWOMS_PARSER_ROCK2D_TABLE_H

#include <vector>

namespace Ewoms {

    class DeckRecord;

    class Rock2dTable {
    public:
        Rock2dTable();

        static Rock2dTable serializeObject();

        void init(const Ewoms::DeckRecord& record, size_t tableIdx);
        size_t size() const;
        size_t sizeMultValues() const;
        double getPressureValue(size_t index) const;
        double getPvmultValue(size_t pressureIndex, size_t saturationIndex ) const;

        bool operator==(const Rock2dTable& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_pvmultValues);
            serializer(m_pressureValues);
        }

    protected:
        std::vector< std::vector <double> > m_pvmultValues;
        std::vector< double > m_pressureValues;

    };

}

#endif
