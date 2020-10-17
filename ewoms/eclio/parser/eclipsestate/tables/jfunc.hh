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
#ifndef EWOMS_JFUNC_HH_
#define EWOMS_JFUNC_HH_

#include <ewoms/eclio/parser/deck/deck.hh>

namespace Ewoms {

class JFunc
{
public:

    enum class Flag { BOTH, WATER, GAS };
    enum class Direction { XY, X, Y, Z };

    JFunc();
    explicit JFunc(const Deck& deck);

    static JFunc serializeObject();

    double alphaFactor() const;
    double betaFactor() const;
    double goSurfaceTension() const;
    double owSurfaceTension() const;
    const Flag& flag() const;
    const Direction& direction() const;

    bool operator==(const JFunc& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(m_flag);
        serializer(m_owSurfaceTension);
        serializer(m_goSurfaceTension);
        serializer(m_alphaFactor);
        serializer(m_betaFactor);
        serializer(m_direction);
    }

private:
    Flag       m_flag;             // JFUNC flag: WATER, GAS, or BOTH.  Default BOTH
    double     m_owSurfaceTension; // oil-wat surface tension.  Required if flag is BOTH or WATER
    double     m_goSurfaceTension; // gas-oil surface tension.  Required if flag is BOTH or GAS
    double     m_alphaFactor;      // alternative porosity term. Default 0.5
    double     m_betaFactor;       // alternative permeability term. Default 0.5
    Direction  m_direction;        // XY, X, Y, Z.  Default XY
};
} // Ewoms::

#endif /* EWOMS_JFUNC_HH_ */
