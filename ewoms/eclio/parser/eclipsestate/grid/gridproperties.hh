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
#ifndef ECLIPSE_GRIDPROPERTIES_HH_
#define ECLIPSE_GRIDPROPERTIES_HH_

#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/section.hh>

#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/boxmanager.hh>
#include <ewoms/eclio/parser/units/dimension.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

/*
  This class implements a container (std::unordered_map<std::string ,
  Gridproperty<T>>) of Gridproperties. Usage is as follows:

    1. Instantiate the class; passing the number of grid cells and the
       supported keywords as a list of strings to the constructor.

    2. Query the container with the supportsKeyword() and hasKeyword()
       methods.

    3. When you ask the container to get a keyword with the
       getKeyword() method it will automatically create a new
       GridProperty object if the container does not have this
       property.
*/

namespace Ewoms {

    class Eclipse3DProperties;

    template <typename T>
    class GridProperties {
    public:
        typedef typename GridProperty<T>::SupportedKeywordInfo SupportedKeywordInfo;

        struct const_iterator;

        GridProperties() = default;
        GridProperties(const EclipseGrid& eclipseGrid,
                       const UnitSystem*  deckUnitSystem,
                       std::vector< SupportedKeywordInfo >&& supportedKeywords);

        explicit GridProperties(const EclipseGrid& eclipseGrid,
                       std::vector< SupportedKeywordInfo >&& supportedKeywords);

        T convertInputValue(  const GridProperty<T>& property , double doubleValue) const;
        T convertInputValue( double doubleValue ) const;

        bool supportsKeyword(const std::string& keyword) const;
        bool isDefaultInitializable(const std::string& keyword) const;

        /*
          The difference between hasKeyword() and hasDeckKeyword( ) is
          that hasDeckKeyword( ) will return false for keywords which
          have only been auto created - and are not explicitly
          mentioned in the deck.
        */

        bool hasKeyword(const std::string& keyword) const;
        bool hasDeckKeyword(const std::string& keyword) const;

        size_t size() const;
        void assertKeyword(const std::string& keyword) const;

        /*
          The getKeyword() method will auto create a keyword if
          requested, the getDeckKeyword() method will onyl return a
          keyword if it has been explicitly mentioned in the deck. The
          getDeckKeyword( ) method will throw an exception instead of
          auto creating the keyword.
        */

        const GridProperty<T>& getKeyword(const std::string& keyword) const;
        const GridProperty<T>& getDeckKeyword(const std::string& keyword) const;

        bool addKeyword(const std::string& keywordName);
        void copyKeyword(const std::string& srcField ,
                         const std::string& targetField ,
                         const Box& inputBox);

        template <class Keyword>
        bool hasKeyword() const {
            return hasKeyword( Keyword::keywordName );
        }

        template <class Keyword>
        const GridProperty<T>& getKeyword() const {
            return getKeyword( Keyword::keywordName );
        }

        template <class Keyword>
        const GridProperty<T>& getInitializedKeyword() const {
            return getInitializedKeyword( Keyword::keywordName );
        }

        GridProperty<T>& getOrCreateProperty(const std::string& name);

        /**
           The fine print of the manual says the ADD keyword should support
           some state dependent semantics regarding endpoint scaling arrays
           in the PROPS section. That is not supported.
        */

        void handleADDRecord( const DeckRecord& record, BoxManager& boxManager);
        void handleMAXVALUERecord( const DeckRecord& record, BoxManager& boxManager);
        void handleMINVALUERecord( const DeckRecord& record, BoxManager& boxManager);
        void handleMULTIPLYRecord( const DeckRecord& record, BoxManager& boxManager);
        void handleCOPYRecord( const DeckRecord& record, BoxManager& boxManager);
        void handleEQUALSRecord( const DeckRecord& record, BoxManager& boxManager);
        void handleOPERATERecord( const DeckRecord& record , BoxManager& boxManager);

        void handleEQUALREGRecord( const DeckRecord& record, const GridProperty<int>& regionProperty );
        void handleADDREGRecord( const DeckRecord& record, const GridProperty<int>& regionProperty );
        void handleMULTIREGRecord( const DeckRecord& record, const GridProperty<int>& regionProperty );
        void handleCOPYREGRecord( const DeckRecord& record, const GridProperty<int>& regionProperty );
        void handleOPERATERRecord( const DeckRecord& record , const GridProperty<int>& regionProperty );
        /*
          Iterators over initialized properties. The overloaded
          operator*() opens the pair which comes natively from the
          std::map iterator.
        */
        const_iterator begin() const {
            return const_iterator( m_properties.begin() );
        }

        const_iterator end() const {
            return const_iterator( m_properties.end() );
        }

        /*
         * storage MUST ensure that std::addressof(storage.at( key )) is valid.
         */
        typedef typename std::map<std::string , GridProperty<T> > storage;
        typedef typename storage::const_iterator storage_iterator;

        struct const_iterator : public storage_iterator {
            const_iterator( storage_iterator iter ) : storage_iterator( iter ) { }

            const GridProperty<T>& operator*( ) {
                const auto& pair = storage_iterator::operator*( );
                return pair.second;
            }
        };

    private:
        /// this method exists for (friend) Eclipse3DProperties to be allowed initializing PORV and ACTNUM keyword
        void postAddKeyword(const std::string& name,
                            const T defaultValue,
                            std::function< void( const std::vector<bool>& defaulted, std::vector< T >& ) > postProcessor,
                            const std::string& dimString,
                            const bool defaultInitializable );

        void postAddKeyword(const std::string& name,
                            std::function< std::vector< T >(size_t) > initProcessor,
                            const std::string& dimString);

        GridProperty<T>& getKeyword(const std::string& keyword);
        bool addAutoGeneratedKeyword_(const std::string& keywordName) const;
        void insertKeyword(const SupportedKeywordInfo& supportedKeyword) const;
        bool isAutoGenerated_(const std::string& keyword) const;

        friend class Eclipse3DProperties; // needed for PORV keyword entanglement
        size_t nx = 0;
        size_t ny = 0;
        size_t nz = 0;
        const UnitSystem *  m_deckUnitSystem = nullptr;

        mutable std::unordered_map<std::string, SupportedKeywordInfo> m_supportedKeywords;
        mutable storage m_properties;
        mutable std::set<std::string> m_autoGeneratedProperties;
    };

}

#endif // ECLIPSE_GRIDPROPERTIES_HH_
