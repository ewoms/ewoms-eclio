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

#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rtempvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

namespace Ewoms {

    template< typename T >
    static std::function< std::vector< T >( size_t ) > constant( T val ) {
        return [=]( size_t size ) { return std::vector< T >( size, val ); };
    }

    template< typename T >
    static std::function< void( const std::vector<bool>&, std::vector< T >& ) > noop() {
        return []( const std::vector<bool>&, std::vector< T >& ) { return; };
    }

    template< typename T >
    GridPropertySupportedKeywordInfo< T >::GridPropertySupportedKeywordInfo(
            const std::string& name,
            std::function< std::vector< T >( size_t ) > init,
            std::function< void( const std::vector<bool>& defaulted, std::vector< T >& ) > post,
            const std::string& dimString,
            bool defaultInitializable ) :
        m_keywordName( name ),
        m_initializer( init ),
        m_postProcessor( post ),
        m_dimensionString( dimString ),
        m_defaultInitializable ( defaultInitializable )
    {}

    template< typename T >
    GridPropertySupportedKeywordInfo< T >::GridPropertySupportedKeywordInfo(
            const std::string& name,
            std::function< std::vector< T >( size_t ) > init,
            const std::string& dimString,
            bool defaultInitializable ) :
        m_keywordName( name ),
        m_initializer( init ),
        m_postProcessor( noop< T >() ),
        m_dimensionString( dimString ),
        m_defaultInitializable ( defaultInitializable )
    {}

    template< typename T >
    GridPropertySupportedKeywordInfo< T >::GridPropertySupportedKeywordInfo(
            const std::string& name,
            const T defaultValue,
            const std::string& dimString,
            bool defaultInitializable ) :
        m_keywordName( name ),
        m_initializer( constant( defaultValue ) ),
        m_postProcessor( noop< T >() ),
        m_dimensionString( dimString ),
        m_defaultInitializable ( defaultInitializable )
    {}

    template< typename T >
    GridPropertySupportedKeywordInfo< T >::GridPropertySupportedKeywordInfo(
            const std::string& name,
            const T defaultValue,
            std::function< void( const std::vector<bool>&, std::vector< T >& ) > post,
            const std::string& dimString,
            bool defaultInitializable ) :
        m_keywordName( name ),
        m_initializer( constant( defaultValue ) ),
        m_postProcessor( post ),
        m_dimensionString( dimString ),
        m_defaultInitializable ( defaultInitializable )
    {}

    template< typename T >
    const std::string& GridPropertySupportedKeywordInfo< T >::getKeywordName() const {
        return this->m_keywordName;
    }

    template< typename T >
    const std::string& GridPropertySupportedKeywordInfo< T >::getDimensionString() const {
        return this->m_dimensionString;
    }

    template< typename T >
    const std::function< std::vector< T >( size_t ) >& GridPropertySupportedKeywordInfo< T >::initializer() const {
        return this->m_initializer;
    }

    template< typename T >
    const std::function< void( const std::vector<bool>&, std::vector< T >& ) >&
    GridPropertySupportedKeywordInfo< T >::postProcessor() const
    {
        return this->m_postProcessor;
    }

    template<typename T>
    bool GridPropertySupportedKeywordInfo< T >::isDefaultInitializable() const {
        return m_defaultInitializable;
    }

    template< typename T >
    GridProperty< T >::GridProperty( size_t nx, size_t ny, size_t nz, const SupportedKeywordInfo& kwInfo ) :
        m_nx( nx ),
        m_ny( ny ),
        m_nz( nz ),
        m_kwInfo( kwInfo ),
        m_data( kwInfo.initializer()( nx * ny * nz ) ),
        m_defaulted( nx * ny * nz, true ),
        m_hasRunPostProcessor( false )
    {}

    template< typename T >
    size_t GridProperty< T >::getCartesianSize() const {
        return m_data.size();
    }

    template< typename T >
    size_t GridProperty< T >::getNX() const {
        return m_nx;
    }

    template< typename T >
    size_t GridProperty< T >::getNY() const {
        return m_ny;
    }

    template< typename T >
    size_t GridProperty< T >::getNZ() const {
        return m_nz;
    }

    template< typename T >
    bool GridProperty<T>::deckAssigned() const {
        return this->assigned;
    }

    template< typename T >
    const std::vector< bool >& GridProperty< T >::wasDefaulted() const {
        return this->m_defaulted;
    }

    template< typename T >
    const std::vector< T >& GridProperty< T >::getData() const {
        return m_data;
    }

    template< typename T >
    void GridProperty< T >::assignData(std::vector<T>&& data) {
        this->m_data = std::move(data);
    }

    template< typename T >
    void GridProperty< T >::assignData(const std::vector<T>& data) {
        this->m_data = data;
    }

    template< typename T >
    void GridProperty< T >::multiplyWith( const GridProperty< T >& other ) {
        if ((m_nx == other.m_nx) && (m_ny == other.m_ny) && (m_nz == other.m_nz)) {
            for (size_t g=0; g < m_data.size(); g++)
                m_data[g] *= other.m_data[g];
        } else
            throw std::invalid_argument("Size mismatch between properties in mulitplyWith.");
    }

    template< typename T >
    void GridProperty< T >::multiplyValueAtIndex(size_t index, T factor) {
        m_data[index] *= factor;
    }

    template< typename T >
    void GridProperty< T >::maskedSet( T value, const std::vector< bool >& mask ) {
        for (size_t g = 0; g < getCartesianSize(); g++) {
            if (mask[g])
                this->setElement(g, value);
        }
        this->assigned = true;
    }

    template< typename T >
    void GridProperty< T >::maskedMultiply( T value, const std::vector<bool>& mask ) {
        for (size_t g = 0; g < getCartesianSize(); g++) {
            if (mask[g])
                m_data[g] *= value;
        }
    }

    template< typename T >
    void GridProperty< T >::maskedAdd( T value, const std::vector<bool>& mask ) {
        for (size_t g = 0; g < getCartesianSize(); g++) {
            if (mask[g])
                m_data[g] += value;
        }
    }

    template< typename T >
    void GridProperty< T >::maskedCopy( const GridProperty< T >& other, const std::vector< bool >& mask) {
        for (size_t g = 0; g < getCartesianSize(); g++) {
            if (mask[g])
                this->setElement(g, other.m_data[g], other.m_defaulted[g]);
        }
        this->assigned = other.deckAssigned();
    }

    template< typename T >
    void GridProperty< T >::initMask( T value, std::vector< bool >& mask ) const {
        mask.resize(getCartesianSize());
        for (size_t g = 0; g < getCartesianSize(); g++) {
            if (m_data[g] == value)
                mask[g] = true;
            else
                mask[g] = false;
        }
    }

    template< typename T >
    void GridProperty< T >::loadFromDeckKeyword( const DeckKeyword& deckKeyword, bool multiply ) {
        const auto& deckItem = getDeckItem(deckKeyword);
        const auto size = deckItem.data_size();
        for (size_t dataPointIdx = 0; dataPointIdx < size; ++dataPointIdx) {
            if (!deckItem.defaultApplied(dataPointIdx)) {
                if (multiply)
                    mulDataPoint(dataPointIdx, dataPointIdx, deckItem);
                else
                    setDataPoint(dataPointIdx, dataPointIdx, deckItem);
            }
        }

        this->assigned = true;
    }

    template< typename T >
    void GridProperty< T >::loadFromDeckKeyword( const Box& inputBox, const DeckKeyword& deckKeyword, bool multiply) {
        if (inputBox.isGlobal())
            loadFromDeckKeyword( deckKeyword, multiply );
        else {
            const auto& deckItem = getDeckItem(deckKeyword);
            const std::vector<size_t>& indexList = inputBox.getIndexList();
            if (indexList.size() == deckItem.data_size()) {
                for (size_t sourceIdx = 0; sourceIdx < indexList.size(); sourceIdx++) {
                    size_t targetIdx = indexList[sourceIdx];
                    if (sourceIdx < deckItem.data_size()
                        && !deckItem.defaultApplied(sourceIdx))
                        {
                            if (multiply)
                                mulDataPoint(sourceIdx, targetIdx, deckItem);
                            else
                                setDataPoint(sourceIdx, targetIdx, deckItem);
                        }
                }
            } else {
                std::string boxSize = std::to_string(static_cast<long long>(indexList.size()));
                std::string keywordSize = std::to_string(static_cast<long long>(deckItem.data_size()));

                throw std::invalid_argument("Size mismatch: Box:" + boxSize + "  DeckKeyword:" + keywordSize);
            }
        }
    }

    template< typename T >
    void GridProperty< T >::copyFrom( const GridProperty< T >& src, const Box& inputBox, const UnitSystem* unitSystem ) {
        std::function<double(double)> convertFn([](double x) { return x; });

        if (unitSystem && src.getDimensionString() != getDimensionString()) {
            // this deals with assignments of fields that exhibit different units. after the
            // COPY operation the grid property ought to exhibit the same values in terms of
            // the unit system used by the deck, which do not necessarily correspond to
            // identical values in SI units.
            const auto& srcDim = unitSystem->parse(src.getDimensionString());
            const auto& dstDim = unitSystem->parse(getDimensionString());

            convertFn = [srcDim, dstDim](double x) { return dstDim.convertRawToSi(srcDim.convertSiToRaw(x)); };
        }

        if (inputBox.isGlobal())
            for (size_t i = 0; i < src.getCartesianSize(); ++i)
                this->setElement(i, convertFn(src.m_data[i]), src.m_defaulted[i]);
        else
            for (const auto& i : inputBox.getIndexList())
                this->setElement(i, convertFn(src.m_data[i]), src.m_defaulted[i]);
        this->assigned = src.deckAssigned();

    }

    template< typename T >
    void GridProperty< T >::maxvalue( T value, const Box& inputBox ) {
        if (inputBox.isGlobal())
            for (size_t i = 0; i < m_data.size(); ++i)
                this->setElement(i, std::min(value, this->m_data[i]));
        else
            for (const auto& i : inputBox.getIndexList())
                this->setElement(i, std::min(value, this->m_data[i]));
    }

    template< typename T >
    void GridProperty< T >::minvalue( T value, const Box& inputBox ) {
        if (inputBox.isGlobal())
            for (size_t i = 0; i < m_data.size(); ++i)
                this->setElement(i, std::max(value, this->m_data[i]));
        else
            for (const auto& i : inputBox.getIndexList())
                this->setElement(i, std::max(value, this->m_data[i]));
    }

    template< typename T >
    void GridProperty< T >::scale( T scaleFactor, const Box& inputBox ) {
        if (inputBox.isGlobal()) {
            for (size_t i = 0; i < m_data.size(); ++i)
                m_data[i] *= scaleFactor;
        } else {
            const std::vector<size_t>& indexList = inputBox.getIndexList();
            for (size_t i = 0; i < indexList.size(); i++) {
                size_t targetIndex = indexList[i];
                m_data[targetIndex] *= scaleFactor;
            }
        }
    }

    template< typename T >
    void GridProperty< T >::add( T shiftValue, const Box& inputBox ) {
        if (inputBox.isGlobal()) {
            for (size_t i = 0; i < m_data.size(); ++i)
                m_data[i] += shiftValue;
        } else {
            const std::vector<size_t>& indexList = inputBox.getIndexList();
            for (size_t i = 0; i < indexList.size(); i++) {
                size_t targetIndex = indexList[i];
                m_data[targetIndex] += shiftValue;
            }
        }
    }

    template< typename T >
    void GridProperty< T >::setScalar( T value, const Box& inputBox ) {
        if (inputBox.isGlobal()) {
            std::fill(m_data.begin(), m_data.end(), value);
            m_defaulted.assign(m_defaulted.size(), false);
        } else
            for (const auto& i : inputBox.getIndexList())
                this->setElement(i, value);
        this->assigned = true;
    }

    template< typename T >
    const std::string& GridProperty< T >::getKeywordName() const {
        return m_kwInfo.getKeywordName();
    }

    template< typename T >
    const typename GridProperty< T >::SupportedKeywordInfo&
    GridProperty< T >::getKeywordInfo() const {
        return m_kwInfo;
    }

    template< typename T >
    void GridProperty< T >::runPostProcessor() {
        if( this->m_hasRunPostProcessor ) return;
        this->m_hasRunPostProcessor = true;
        this->m_kwInfo.postProcessor()( m_defaulted, m_data );
    }

    template< typename T >
    void GridProperty< T >::checkLimits( T min, T max ) const {
        for (size_t g=0; g < m_data.size(); g++) {
            T value = m_data[g];
            if ((value < min) || (value > max))
                throw std::invalid_argument("Property element " + std::to_string( value) + " in " + getKeywordName() + " outside valid limits: [" + std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    }

    template< typename T  >
    const DeckItem& GridProperty< T >::getDeckItem( const DeckKeyword& deckKeyword ) {
        if (deckKeyword.size() != 1)
            throw std::invalid_argument("Grid properties can only have a single record (keyword "
                                        + deckKeyword.name() + ")");
        if (deckKeyword.getRecord(0).size() != 1)
            // this is an error of the definition of the ParserKeyword (most likely in
            // the corresponding JSON file)
            throw std::invalid_argument("Grid properties may only exhibit a single item  (keyword "
                                        + deckKeyword.name() + ")");

        const auto& deckItem = deckKeyword.getRecord(0).getItem(0);

        if (deckItem.data_size() > m_data.size())
            throw std::invalid_argument("Size mismatch when setting data for:" + getKeywordName()
                                        + " keyword size: " + std::to_string( deckItem.data_size() )
                                        + " input size: " + std::to_string( m_data.size()) );

        return deckItem;
    }

template<>
void GridProperty<int>::setDataPoint(size_t sourceIdx, size_t targetIdx, const DeckItem& deckItem) {
    this->setElement(targetIdx, deckItem.get< int >(sourceIdx));
}

template<>
void GridProperty<double>::setDataPoint(size_t sourceIdx, size_t targetIdx, const DeckItem& deckItem) {
    this->setElement(targetIdx, deckItem.getSIDouble(sourceIdx));
}

template <typename T>
void GridProperty<T>::setElement(const typename std::vector<T>::size_type i, const T value, const bool defaulted) {
    this->m_data[i] = value;
    this->m_defaulted[i] = defaulted;
}

template<>
void GridProperty<double>::mulDataPoint(size_t sourceIdx, size_t targetIdx, const DeckItem& deckItem) {
    this->m_data[targetIdx] *= deckItem.getSIDouble(sourceIdx);
}

template<>
void GridProperty<int>::mulDataPoint(size_t sourceIdx, size_t targetIdx, const DeckItem& deckItem) {
    this->m_data[targetIdx] *= deckItem.get<int>(sourceIdx);
}

template<>
bool GridProperty<int>::containsNaN( ) const {
    throw std::logic_error("Only <double> and can be meaningfully queried for nan");
}

template<>
bool GridProperty<double>::containsNaN( ) const {
    bool return_value = false;
    size_t size = m_data.size();
    size_t index = 0;
    while (true) {
        if (std::isnan(m_data[index])) {
            return_value = true;
            break;
        }

        index++;
        if (index == size)
            break;
    }
    return return_value;
}

template<>
const std::string& GridProperty<int>::getDimensionString() const {
    throw std::logic_error("Only <double> grid properties have dimension");
}

template<>
const std::string& GridProperty<double>::getDimensionString() const {
    return m_kwInfo.getDimensionString();
}

template<typename T>
std::vector<T> GridProperty<T>::compressedCopy(const EclipseGrid& grid) const {
    if (grid.allActive())
        return m_data;
    else {
        return grid.compressedVector( m_data );
    }
}

template<typename T>
std::vector<size_t> GridProperty<T>::cellsEqual(T value, const std::vector<int>& activeMap) const {
    std::vector<size_t> cells;
    for (size_t active_index = 0; active_index < activeMap.size(); active_index++) {
        size_t global_index = activeMap[ active_index ];
        if (m_data[global_index] == value)
            cells.push_back( active_index );
    }
    return cells;
}

template<typename T>
std::vector<size_t> GridProperty<T>::indexEqual(T value) const {
    std::vector<size_t> index_list;
    for (size_t index = 0; index < m_data.size(); index++) {
        if (m_data[index] == value)
            index_list.push_back( index );
    }
    return index_list;
}

template<typename T>
std::vector<size_t> GridProperty<T>::cellsEqual(T value, const EclipseGrid& grid, bool active) const {
    if (active)
        return cellsEqual( value , grid.getActiveMap());
    else
        return indexEqual( value );
}

std::vector< double > temperature_lookup( size_t size,
                                          const TableManager* tables,
                                          const EclipseGrid* grid,
                                          const GridProperties<int>* ig_props ) {

    if (tables->hasTables("RTEMPVD")) {
        const std::vector< int >& eqlNum = ig_props->getKeyword("EQLNUM").getData();

        const auto& rtempvdTables = tables->getRtempvdTables();
        std::vector< double > values( size, 0 );

        for (size_t cellIdx = 0; cellIdx < eqlNum.size(); ++ cellIdx) {
            int cellEquilRegionIdx = eqlNum[cellIdx] - 1; // EQLNUM contains fortran-style indices!
            const RtempvdTable& rtempvdTable = rtempvdTables.getTable<RtempvdTable>(cellEquilRegionIdx);
            double cellDepth = std::get<2>(grid->getCellCenter(cellIdx));
            values[cellIdx] = rtempvdTable.evaluate("Temperature", cellDepth);
        }

        return values;
    } else
        return std::vector< double >( size, tables->rtemp( ) );
}

}

template class Ewoms::GridPropertySupportedKeywordInfo< int >;
template class Ewoms::GridPropertySupportedKeywordInfo< double >;

template class Ewoms::GridProperty< int >;
template class Ewoms::GridProperty< double >;
