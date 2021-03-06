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
#ifndef EWOMS_TABLE_COLUMN_H
#define EWOMS_TABLE_COLUMN_H

#include <string>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/tables/columnschema.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tableindex.hh>

namespace Ewoms {

    class TableColumn {
    public:
        TableColumn();
        explicit TableColumn( const ColumnSchema& schema );

        TableColumn(const TableColumn& c2) { *this = c2; }

        static TableColumn serializeObject();

        size_t size( ) const;
        const std::string& name() const;
        void assertOrder(double value1 , double value2) const;
        void addValue(double);
        void addDefault();
        void updateValue(size_t index, double value);
        double operator[](size_t index) const;
        bool defaultApplied(size_t index) const;
        bool hasDefault( ) const;
        double front() const;
        double back() const;
        double min() const;
        double max() const;
        bool inRange( double arg ) const;

        /*
           Will extrapolate with constant endpoint values if @argValue
           is out of range.
        */
        TableIndex lookup(double argValue) const;
        double eval( const TableIndex& index) const;
        void applyDefaults( const TableColumn& argColumn );
        void assertUnitRange() const;
        TableColumn& operator= (const TableColumn& other);

        std::vector<double> vectorCopy() const;
        std::vector<double>::const_iterator begin() const;
        std::vector<double>::const_iterator end() const;

        bool operator==(const TableColumn& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            m_schema.serializeOp(serializer);
            serializer(m_name);
            serializer(m_values);
            serializer(m_default);
            serializer(m_defaultCount);
        }

    private:
        void assertUpdate(size_t index, double value) const;
        void assertPrevious(size_t index , double value) const;
        void assertNext(size_t index , double value) const;

        ColumnSchema m_schema;
        std::string m_name;
        std::vector<double> m_values;
        std::vector<bool> m_default;
        size_t m_defaultCount;
    };

}

#endif
