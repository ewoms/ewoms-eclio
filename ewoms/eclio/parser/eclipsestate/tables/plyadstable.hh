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
#ifndef EWOMS_PARSER_PLYADS_TABLE_H
#define	EWOMS_PARSER_PLYADS_TABLE_H

#include "simpletable.hh"

namespace Ewoms {

    class DeckItem;

    class PlyadsTable : public SimpleTable {
        public:
            PlyadsTable( const DeckItem& item );

            const TableColumn& getPolymerConcentrationColumn() const;
            const TableColumn& getAdsorbedPolymerColumn() const;
    };
}

#endif