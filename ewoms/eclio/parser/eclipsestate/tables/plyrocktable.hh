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
#ifndef EWOMS_PARSER_PLYROCK_TABLE_H
#define	EWOMS_PARSER_PLYROCK_TABLE_H

#include "simpletable.hh"

namespace Ewoms {

    class DeckReckord;

    class PlyrockTable : public SimpleTable {
    public:

        // This is not really a table; every column has only one element.
        PlyrockTable( const DeckRecord& record );

        // since this keyword is not necessarily monotonic, it cannot be evaluated!
        //using SimpleTable::evaluate;

        const TableColumn& getDeadPoreVolumeColumn() const;
        const TableColumn& getResidualResistanceFactorColumn() const;
        const TableColumn& getRockDensityFactorColumn() const;

        // is column is actually an integer, but this is not yet
        // supported by ewoms-eclio (yet?) as it would require quite a
        // few changes in the table support classes (read: it would
        // probably require a lot of template vodoo) and some in the
        // JSON-to-C conversion code. In the meantime, the index is
        // just a double which can be converted to an integer in the
        // calling code. (Make sure, that you don't interpolate
        // indices, though!)
        const TableColumn& getAdsorbtionIndexColumn() const;
        const TableColumn& getMaxAdsorbtionColumn() const;
    };
}

#endif
