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

#ifndef DECKITEM_H
#define DECKITEM_H

#include <string>
#include <vector>
#include <memory>
#include <ostream>

#include <ewoms/eclio/parser/units/dimension.hh>
#include <ewoms/eclio/parser/utility/typetools.hh>
#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/deck/value_status.hh>

namespace Ewoms {
    class DeckOutput;

    class DeckItem {
    public:

        DeckItem() = default;
        DeckItem( const std::string&, int);
        DeckItem( const std::string&, std::string);
        DeckItem( const std::string&, double) = delete;
        DeckItem( const std::string&, UDAValue) = delete;
        DeckItem( const std::string&, UDAValue, const std::vector<Dimension>& active_dim, const std::vector<Dimension>& default_dim);
        DeckItem( const std::string&, double, const std::vector<Dimension>& active_dim, const std::vector<Dimension>& default_dim);
        DeckItem(const std::vector<double>& dVec,
                 const std::vector<int>& iVec,
                 const std::vector<std::string>& sVec,
                 const std::vector<UDAValue>& uVec,
                 type_tag type,
                 const std::string& itemName,
                 const std::vector<value::status>& valueStat,
                 bool rawdata,
                 const std::vector<Dimension>& activeDim,
                 const std::vector<Dimension>& defDim);

        const std::string& name() const;

        // return true if the default value was used for a given data point
        bool defaultApplied( size_t ) const;

        // Return true if the item has a value for the current index;
        // does not differentiate between default values from the
        // config and values which have been set in the deck.
        bool hasValue( size_t ) const;

        // if the number returned by this method is less than what is semantically
        // expected (e.g. size() is less than the number of cells in the grid for
        // keywords like e.g. SGL), then the remaining values are defaulted. The deck
        // creates the defaulted items if all their sizes are fully specified by the
        // keyword, though...

        size_t data_size() const;

        template<typename T>
        T get( size_t index ) const;

        double getSIDouble( size_t ) const;
        std::string getTrimmedString( size_t ) const;

        template< typename T > const std::vector< T >& getData() const;
        const std::vector< double >& getSIDoubleData() const;
        const std::vector<value::status>& getValueStatus() const;

        void push_back( UDAValue );
        void push_back( int );
        void push_back( double );
        void push_back( std::string );
        void push_back( UDAValue, size_t );
        void push_back( int, size_t );
        void push_back( double, size_t );
        void push_back( std::string, size_t );
        void push_backDefault( UDAValue );
        void push_backDefault( int );
        void push_backDefault( double );
        void push_backDefault( std::string );
        // trying to access the data of a "dummy default item" will raise an exception

        template <typename T>
        void push_backDummyDefault();

        type_tag getType() const;

        void write(DeckOutput& writer) const;
        friend std::ostream& operator<<(std::ostream& os, const DeckItem& item);

        /*
          The comparison can be adjusted with the cmp_default and
          cmp_numeric flags. If cmp_default is set to true the
          comparison will take the defaulted status of the items into
          account, i.e. two items will compare differently if one is
          defaulted and the other has the default value explicitly
          set. The default behaviour is cmp_default == false -
          itrespective of whether they have been set explicitly or
          have been defaulted.
        */
        bool equal(const DeckItem& other, bool cmp_default, bool cmp_numeric) const;

        /*
          The operator== is implemented based on the equal( ) method,
          with the arguments cmp_default=false and cmp_numeric=true.
        */
        bool operator==(const DeckItem& other) const;
        bool operator!=(const DeckItem& other) const;
        static bool to_bool(std::string string_value);

        const std::vector<double>& dVal() const;
        const std::vector<int>& iVal() const;
        const std::vector<std::string>& sVal() const;
        const std::vector<UDAValue>& uVal() const;

        const std::vector<value::status>& valueStatus() const;
        bool rawData() const;
        const std::vector<Dimension>& activeDimensions() const;
        const std::vector<Dimension>& defaultDimensions() const;

    private:
        mutable std::vector< double > dval;
        std::vector< int > ival;
        std::vector< std::string > sval;
        std::vector< UDAValue > uval;

        type_tag type = type_tag::unknown;

        std::string item_name;
        std::vector<value::status> value_status;
        /*
          To save space we mutate the dval object in place when asking for SI
          data; the current state of of the dval member is tracked with the
          raw_data bool member.
        */
        mutable bool raw_data = true;
        std::vector< Dimension > active_dimensions;
        std::vector< Dimension > default_dimensions;

        template< typename T > std::vector< T >& value_ref();
        template< typename T > const std::vector< T >& value_ref() const;
        template< typename T > void push( T );
        template< typename T > void push( T, size_t );
        template< typename T > void push_default( T );
        template< typename T > void write_vector(DeckOutput& writer, const std::vector<T>& data) const;
    };
}
#endif  /* DECKITEM_HH */
