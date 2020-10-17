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
#ifndef EWOMS_TABLE_SCHEMA_H
#define EWOMS_TABLE_SCHEMA_H

#include <string>

#include <ewoms/eclio/parser/eclipsestate/tables/columnschema.hh>
#include <ewoms/eclio/parser/eclipsestate/util/orderedmap.hh>

namespace Ewoms {

    class TableSchema {
    public:
        static TableSchema serializeObject();

        void addColumn( ColumnSchema );
        const ColumnSchema& getColumn( const std::string& name ) const;
        const ColumnSchema& getColumn( size_t columnIndex ) const;
        bool hasColumn(const std::string&) const;

        /* Number of columns */
        size_t size() const;

        bool operator==(const TableSchema& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            m_columns.serializeOp(serializer);
        }

    private:
        OrderedMap<std::string, ColumnSchema> m_columns;
    };
}

#endif

