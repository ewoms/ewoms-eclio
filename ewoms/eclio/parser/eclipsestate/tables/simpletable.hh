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
#ifndef EWOMS_PARSER_SIMPLE_TABLE_H
#define	EWOMS_PARSER_SIMPLE_TABLE_H

#include <ewoms/eclio/parser/eclipsestate/tables/tablecolumn.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tableschema.hh>
#include <ewoms/eclio/parser/eclipsestate/util/orderedmap.hh>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Ewoms {

    class DeckItem;

    class SimpleTable {

    public:
        SimpleTable() = default;
        SimpleTable(TableSchema, const DeckItem& deckItem);
        SimpleTable(const TableSchema& schema,
                    const OrderedMap<std::string, TableColumn>& columns,
                    bool jfunc);
        explicit SimpleTable( TableSchema );

        void addColumns();
        void init(const DeckItem& deckItem );
        size_t numColumns() const;
        size_t numRows() const;
        void addRow( const std::vector<double>& row);
        const TableColumn& getColumn(const std::string &name) const;
        const TableColumn& getColumn(size_t colIdx) const;
        bool hasColumn(const std::string& name) const;

        TableColumn& getColumn(const std::string &name);
        TableColumn& getColumn(size_t colIdx);

        double get(const std::string& column  , size_t row) const;
        double get(size_t column  , size_t row) const;
        /*!
         * \brief Evaluate a column of the table at a given position.
         *
         * This method uses linear interpolation and always uses the first column as the
         * X coordinate.
         */
        double evaluate(const std::string& columnName, double xPos) const;

        /// throws std::invalid_argument if jf != m_jfunc
        void assertJFuncPressure(const bool jf) const;

        bool operator==(const SimpleTable& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            m_schema.serializeOp(serializer);
            m_columns.serializeOp(serializer);
            serializer(m_jfunc);
        }

    protected:
        TableSchema m_schema;
        OrderedMap<std::string, TableColumn> m_columns;
        bool m_jfunc = false;
    };
}

#endif
