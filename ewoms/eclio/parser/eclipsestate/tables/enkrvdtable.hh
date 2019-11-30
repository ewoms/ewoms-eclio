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
#ifndef EWOMS_PARSER_ENKRVD_TABLE_H
#define	EWOMS_PARSER_ENKRVD_TABLE_H

#include "simpletable.hh"

namespace Ewoms {

    class DeckItem;

    class EnkrvdTable : public SimpleTable {
    public:

        EnkrvdTable( const DeckItem& item );

        // using this method is strongly discouraged but the current endpoint scaling
        // code makes it hard to avoid
        using SimpleTable::getColumn;

        /*!
         * \brief The datum depth for the remaining columns
         */
        const TableColumn& getDepthColumn() const;

        /*!
         * \brief Maximum relative permeability of water
         */
        const TableColumn& getKrwmaxColumn() const;

        /*!
         * \brief Maximum relative permeability of gas
         */
        const TableColumn& getKrgmaxColumn() const;

        /*!
         * \brief Maximum relative permeability of oil
         */
        const TableColumn& getKromaxColumn() const;

        /*!
         * \brief Relative permeability of water at the critical oil (or gas) saturation
         */
        const TableColumn& getKrwcritColumn() const;

        /*!
         * \brief Relative permeability of gas at the critical oil (or water) saturation
         */
        const TableColumn& getKrgcritColumn() const;

        /*!
         * \brief Oil relative permeability of oil at the critical gas saturation
         */
        const TableColumn& getKrocritgColumn() const;

        /*!
         * \brief Oil relative permeability of oil at the critical water saturation
         */
        const TableColumn& getKrocritwColumn() const;
    };
}

#endif

