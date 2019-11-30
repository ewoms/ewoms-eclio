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
#ifndef EWOMS_PARSER_ENPTVD_TABLE_H
#define	EWOMS_PARSER_ENPTVD_TABLE_H

#include "simpletable.hh"

namespace Ewoms {

    class DeckItem;

    class EnptvdTable : public SimpleTable {
    public:
        EnptvdTable( const DeckItem& item );

        // using this method is strongly discouraged but the current endpoint scaling
        // code makes it hard to avoid
        using SimpleTable::getColumn;

        const TableColumn& getDepthColumn() const;

        /*!
         * \brief Connate water saturation
         */
        const TableColumn& getSwcoColumn() const;

        /*!
         * \brief Critical water saturation
         */
        const TableColumn& getSwcritColumn() const;

        /*!
         * \brief Maximum water saturation
         */
        const TableColumn& getSwmaxColumn() const;

        /*!
         * \brief Connate gas saturation
         */
        const TableColumn& getSgcoColumn() const;

        /*!
         * \brief Critical gas saturation
         */
        const TableColumn& getSgcritColumn() const;

        /*!
         * \brief Maximum gas saturation
         */
        const TableColumn& getSgmaxColumn() const;

        /*!
         * \brief Critical oil-in-water saturation
         */
        const TableColumn& getSowcritColumn() const;

        /*!
         * \brief Critical oil-in-gas saturation
         */
        const TableColumn& getSogcritColumn() const;
    };
}

#endif

