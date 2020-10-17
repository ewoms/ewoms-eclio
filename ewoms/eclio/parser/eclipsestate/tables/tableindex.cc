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
#include "config.h"

#include <ewoms/eclio/parser/eclipsestate/tables/tableindex.hh>

namespace Ewoms {

    TableIndex::TableIndex( size_t index1 , double weight1) :
        m_index1( index1 ),
        m_weight1( weight1 )
    {

    }

    TableIndex::TableIndex(const TableIndex& tableIndex)
        : m_index1( tableIndex.m_index1 ),
          m_weight1(tableIndex.m_weight1 )
    {
    }

    size_t TableIndex::getIndex1( ) const {
        return m_index1;
    }

    size_t TableIndex::getIndex2( ) const {
        return m_index1 + 1;
    }

    double TableIndex::getWeight1( ) const {
        return m_weight1;
    }

    double TableIndex::getWeight2( ) const {
        return 1 - m_weight1;
    }

}
