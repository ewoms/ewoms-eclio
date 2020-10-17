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
#ifndef DIMENSION_H
#define DIMENSION_H

#include <string>

namespace Ewoms {

    class Dimension {
    public:
        Dimension();
        Dimension(double SIfactor,
                  double SIoffset = 0.0);

        static Dimension serializeObject();

        double getSIScaling() const;
        double getSIOffset() const;

        double convertRawToSi(double rawValue) const;
        double convertSiToRaw(double siValue) const;

        bool equal(const Dimension& other) const;
        bool isCompositable() const;

        bool operator==( const Dimension& ) const;
        bool operator!=( const Dimension& ) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_SIfactor);
            serializer(m_SIoffset);
        }

    private:
        double m_SIfactor;
        double m_SIoffset;
    };
}

#endif

