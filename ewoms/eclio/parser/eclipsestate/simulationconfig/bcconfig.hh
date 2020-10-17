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
#ifndef EWOMS_BCCONFIG_H
#define EWOMS_BCCONFIG_H

#include <vector>
#include <cstddef>

#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>

namespace Ewoms {

class Deck;
class DeckRecord;

enum class BCType {
     RATE,
     FREE
};

enum class BCComponent {
     OIL,
     GAS,
     WATER,
     SOLVENT,
     POLYMER,
     NONE
};

class BCConfig {
public:

    struct BCFace {
        int i1,i2;
        int j1,j2;
        int k1,k2;
        BCType bctype;
        FaceDir::DirEnum dir;
        BCComponent component;
        double rate;

        BCFace() = default;
        BCFace(const DeckRecord& record);

        static BCFace serializeObject();

        bool operator==(const BCFace& other) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(i1);
            serializer(i2);
            serializer(j1);
            serializer(j2);
            serializer(k1);
            serializer(k2);
            serializer(bctype);
            serializer(dir);
            serializer(component);
            serializer(rate);
        }
    };

    BCConfig() = default;
    explicit BCConfig(const Deck& deck);

    static BCConfig serializeObject();

    std::size_t size() const;
    std::vector<BCFace>::const_iterator begin() const;
    std::vector<BCFace>::const_iterator end() const;
    bool operator==(const BCConfig& other) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.vector(m_faces);
    }

private:
    std::vector<BCFace> m_faces;
};

} //namespace Ewoms

#endif
