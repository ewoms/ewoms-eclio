/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  eWoms is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EWOMS_ECLIPSE_PROPERTIES_H
#define EWOMS_ECLIPSE_PROPERTIES_H

#include <vector>
#include <string>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

namespace Ewoms {

    class Box;
    class BoxManager;
    class Deck;
    class DeckItem;
    class DeckKeyword;
    class DeckRecord;
    class EclipseGrid;
    class DeckSection;
    class TableManager;
    class UnitSystem;

    /// Class representing properties on 3D grid for use in EclipseState.
    class Eclipse3DProperties
    {
    public:

        Eclipse3DProperties() = default;

        Eclipse3DProperties(const Deck& deck,
                            const TableManager& tableManager,
                            const EclipseGrid& eclipseGrid);

        Eclipse3DProperties( UnitSystem unit_system,
                             const TableManager& tableManager,
                             const EclipseGrid& eclipseGrid);

        std::vector< int > getRegions( const std::string& keyword ) const;
        std::string getDefaultRegionKeyword() const;

        const GridProperty<int>&      getIntGridProperty     ( const std::string& keyword ) const;
        const GridProperty<double>&   getDoubleGridProperty  ( const std::string& keyword ) const;

        const GridProperties<int>& getIntProperties() const;
        const GridProperties<double>& getDoubleProperties() const;

        bool hasDeckIntGridProperty(const std::string& keyword) const;
        bool hasDeckDoubleGridProperty(const std::string& keyword) const;
        bool supportsGridProperty(const std::string& keyword) const;

    private:
        const GridProperty<int>& getRegion(const DeckItem& regionItem) const;
        void processGridProperties(const Deck& deck,
                                   const EclipseGrid& eclipseGrid);

        void scanSection(const DeckSection& section,
                         const EclipseGrid& eclipseGrid,
                         bool edit_section);

        void handleADDKeyword(     const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleBOXKeyword(     const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleCOPYKeyword(    const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleENDBOXKeyword(  BoxManager& boxManager);
        void handleEQUALSKeyword(  const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleMAXVALUEKeyword(const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleMINVALUEKeyword(const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleMULTIPLYKeyword(const DeckKeyword& deckKeyword, BoxManager& boxManager);

        void handleADDREGKeyword(  const DeckKeyword& deckKeyword );
        void handleCOPYREGKeyword( const DeckKeyword& deckKeyword );
        void handleEQUALREGKeyword(const DeckKeyword& deckKeyword );
        void handleMULTIREGKeyword(const DeckKeyword& deckKeyword );
        void handleOPERATEKeyword( const DeckKeyword& deckKeyword, BoxManager& boxManager);
        void handleOPERATERKeyword( const DeckKeyword& deckKeyword);

        void loadGridPropertyFromDeckKeyword(const Box& inputBox,
                                             const DeckKeyword& deckKeyword,
                                             bool edity_section);

        void adjustSOGCRwithSWL();

        std::string            m_defaultRegion;
        UnitSystem             m_deckUnitSystem;
        GridProperties<int>    m_intGridProperties;
        GridProperties<double> m_doubleGridProperties;
    };
}

#endif // EWOMS_ECLIPSE_PROPERTIES_H
