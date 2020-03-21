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

#ifndef EWOMS_COLUMN_SCHEMA_H
#define EWOMS_COLUMN_SCHEMA_H

#include <string>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/tables/tableenums.hh>

namespace Ewoms {

    class ColumnSchema {
    public:
        ColumnSchema();
        ColumnSchema(const std::string& name , Table::ColumnOrderEnum order, Table::DefaultAction defaultAction);
        ColumnSchema(const std::string& name , Table::ColumnOrderEnum order, double defaultValue);
        const std::string& name() const;
        bool validOrder( double value1 , double value2) const;
        bool lookupValid( ) const;
        bool acceptsDefault( ) const;
        bool isIncreasing( ) const;
        bool isDecreasing( ) const;
        Table::DefaultAction getDefaultMode( ) const;
        double getDefaultValue( ) const;

        bool operator==(const ColumnSchema& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_name);
            serializer(m_order);
            serializer(m_defaultAction);
            serializer(m_defaultValue);
        }

    private:
        std::string m_name;
        Table::ColumnOrderEnum m_order;
        Table::DefaultAction m_defaultAction;
        double m_defaultValue;
    };
}

#endif

