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
#include <utility>

#include <ewoms/eclio/parser/eclipsestate/tables/tableschema.hh>

namespace Ewoms {

    TableSchema TableSchema::serializeObject()
    {
        TableSchema result;
        result.m_columns.insert({"test1", ColumnSchema::serializeObject()});

        return result;
    }

    void TableSchema::addColumn( ColumnSchema column ) {
        m_columns.insert( std::make_pair( column.name(), column ));
    }

    const ColumnSchema& TableSchema::getColumn( const std::string& name ) const {
        return m_columns.get( name );
    }

    const ColumnSchema& TableSchema::getColumn( size_t columnIndex ) const {
        return m_columns.iget( columnIndex );
    }

    size_t TableSchema::size() const {
        return m_columns.size();
    }

    bool TableSchema::hasColumn(const std::string& name) const {
        return m_columns.count( name ) > 0;
    }

    bool TableSchema::operator==(const TableSchema& data) const {
        return this->m_columns == data.m_columns;
    }
}
