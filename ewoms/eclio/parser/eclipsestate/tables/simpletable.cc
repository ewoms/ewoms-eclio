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
#include <iostream>

#include <ewoms/eclio/parser/eclipsestate/tables/simpletable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tableschema.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>

namespace Ewoms {

    SimpleTable::SimpleTable( TableSchema schema, const DeckItem& deckItem) :
        m_schema( std::move( schema ) ),
        m_jfunc (false)
    {
        init( deckItem );
    }

    SimpleTable::SimpleTable( TableSchema schema ) :
        m_schema( std::move( schema ) ),
        m_jfunc (false)
    {
        addColumns();
    }

    void SimpleTable::addRow( const std::vector<double>& row) {
        if (row.size() == numColumns()) {
            for (size_t colIndex  = 0; colIndex < numColumns(); colIndex++) {
                auto& col = getColumn( colIndex );
                col.addValue( row[colIndex] );
            }
        } else
            throw std::invalid_argument("Size mismatch");
    }

    void SimpleTable::addColumns() {
        for (size_t colIdx = 0; colIdx < m_schema.size(); ++colIdx) {
            const auto& schemaColumn = m_schema.getColumn( colIdx );
            TableColumn column(schemaColumn); // Some move trickery here ...
            m_columns.insert( std::make_pair( schemaColumn.name() , column ));
        }
    }

    double SimpleTable::get(const std::string& column  , size_t row) const {
        const auto& col = getColumn( column );
        return col[row];
    }

    double SimpleTable::get(size_t column  , size_t row) const {
        const auto& col = getColumn( column );
        return col[row];
    }

    void SimpleTable::init( const DeckItem& deckItem ) {
        this->addColumns();

        if ( (deckItem.data_size() % numColumns()) != 0)
            throw std::runtime_error("Number of columns in the data file is"
                    "inconsistent with the ones specified");

        size_t rows = deckItem.data_size() / numColumns();
        for (size_t colIdx = 0; colIdx < numColumns(); ++colIdx) {
            auto& column = getColumn( colIdx );
            for (size_t rowIdx = 0; rowIdx < rows; rowIdx++) {
                size_t deckItemIdx = rowIdx*numColumns() + colIdx;
                if (deckItem.defaultApplied(deckItemIdx))
                    column.addDefault( );
                else if (m_jfunc) {
                    column.addValue( deckItem.getData<double>()[deckItemIdx] );
                }
                else
                    column.addValue( deckItem.getSIDouble(deckItemIdx) );
            }
            if (colIdx > 0)
                column.applyDefaults(getColumn( 0 ));
        }
    }

    size_t SimpleTable::numColumns() const {
        return m_schema.size();
    }

    size_t SimpleTable::numRows() const {
        return getColumn( 0 ).size();
    }

    const TableColumn& SimpleTable::getColumn( const std::string& name) const {
        if (!this->m_jfunc)
            return m_columns.get( name );

        if (name == "PCOW" || name == "PCOG")
            assertJFuncPressure(false); // this will throw since m_jfunc=true
        return m_columns.get( name );
    }

    const TableColumn& SimpleTable::getColumn( size_t columnIndex )  const {
        return m_columns.iget( columnIndex );
    }

    TableColumn& SimpleTable::getColumn( const std::string& name) {
        if (!this->m_jfunc)
            return m_columns.get( name );

        if (name == "PCOW" || name == "PCOG")
            assertJFuncPressure(false); // this will throw since m_jfunc=true
        return m_columns.get( name );
    }

    TableColumn& SimpleTable::getColumn( size_t columnIndex ) {
        return m_columns.iget( columnIndex );
    }

    bool SimpleTable::hasColumn(const std::string& name) const {
        return m_schema.hasColumn( name );
    }

    double SimpleTable::evaluate(const std::string& columnName, double xPos) const
    {
        const auto& argColumn = getColumn( 0 );
        const auto& valueColumn = getColumn( columnName );

        const auto index = argColumn.lookup( xPos );
        return valueColumn.eval( index );
    }

    void SimpleTable::assertJFuncPressure(const bool jf) const {
        if (jf == m_jfunc)
            return;
        // if we reach here, wrong values are read from the deck! (JFUNC is used
        // incorrectly.)  This function writes to std err for now, but will
        // after a grace period be rewritten to throw (std::invalid_argument).
        if (m_jfunc)
            std::cerr << "Developer warning: Pressure column is read with JFUNC in deck." << std::endl;
        else
            std::cerr << "Developer warning: Raw values from JFUNC column is read, but JFUNC not provided in deck." << std::endl;
    }
}
