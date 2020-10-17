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
#ifndef EWOMS_TABLE_INDEX_H
#define EWOMS_TABLE_INDEX_H

#include <cstddef>

/*
  Small class used to simplify lookup in tables based on linear
  interpolation; first binear search in an ordered column is used to
  instantiate a TableIndex object, and then subsequently that
  TableIndex instance is used to evaluate the corresponding value over
  another column.
*/

namespace Ewoms {

    class TableIndex {
    public:
        TableIndex( size_t index1 , double weight1);
        TableIndex( const TableIndex& tableIndex);
        size_t getIndex1( ) const;
        size_t getIndex2( ) const;
        double getWeight1( ) const;
        double getWeight2( ) const;
    private:
        size_t m_index1;
        double m_weight1;
    };
}

#endif
