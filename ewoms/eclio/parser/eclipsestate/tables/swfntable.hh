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
#ifndef EWOMS_PARSER_SWFN_TABLE_H
#define EWOMS_PARSER_SWFN_TABLE_H

#include "simpletable.hh"

namespace Ewoms {
    class SwfnTable : public SimpleTable {

    public:
        SwfnTable( const DeckItem& item, const bool jfunc );

        const TableColumn& getSwColumn() const;
        const TableColumn& getKrwColumn() const;

        /// this column is p_o - p_w (non-wetting phase pressure minus
        /// wetting phase pressure for a given water saturation)
        const TableColumn& getPcowColumn() const;

        /// use this function if JFUNC is set in the deck
        const TableColumn& getJFuncColumn() const;
    };
}

#endif
