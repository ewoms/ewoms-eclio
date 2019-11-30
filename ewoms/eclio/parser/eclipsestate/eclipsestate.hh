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

#ifndef EWOMS_ECLIPSE_STATE_H
#define EWOMS_ECLIPSE_STATE_H

#include <memory>
#include <vector>

#include <ewoms/eclio/parser/errorguard.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipseconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/edit/editnnc.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultcollection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/nnc.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/transmult.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/eclipsestate/simulationconfig/simulationconfig.hh>

namespace Ewoms {

    template< typename > class GridProperty;
    template< typename > class GridProperties;

    class Box;
    class BoxManager;
    class Deck;
    class DeckItem;
    class DeckKeyword;
    class DeckRecord;
    class EclipseGrid;
    class InitConfig;
    class IOConfig;
    class ParseContext;
    class RestartConfig;
    class Section;
    class SimulationConfig;
    class TableManager;
    class UnitSystem;

    class EclipseState {
    public:
        enum EnabledTypes {
            IntProperties = 0x01,
            DoubleProperties = 0x02,

            AllProperties = IntProperties | DoubleProperties
        };

        template<typename T>
        EclipseState(const Deck& deck , const ParseContext& parseContext, T&& errors);
        EclipseState(const Deck& deck , const ParseContext& parseContext, ErrorGuard& errors);
        EclipseState(const Deck& deck);

        const IOConfig& getIOConfig() const;
        IOConfig& getIOConfig();

        const InitConfig& getInitConfig() const;
        const SimulationConfig& getSimulationConfig() const;
        const RestartConfig& getRestartConfig() const;
        RestartConfig& getRestartConfig();

        const EclipseGrid& getInputGrid() const;

        const FaultCollection& getFaults() const;
        const TransMult& getTransMult() const;

        /// non-neighboring connections
        /// the non-standard adjacencies as specified in input deck
        const NNC& getInputNNC() const;
        bool hasInputNNC() const;

        /// editing non-neighboring connections
        /// the non-standard adjacencies as specified in input deck
        const EDITNNC& getInputEDITNNC() const;
        bool hasInputEDITNNC() const;

        const FieldPropsManager& fieldProps() const;
        const Eclipse3DProperties& get3DProperties() const;
        const TableManager& getTableManager() const;
        const EclipseConfig& getEclipseConfig() const;
        const EclipseConfig& cfg() const;

        // the unit system used by the deck. note that it is rarely needed to convert
        // units because internally to ewoms-eclio everything is represented by SI
        // units...
        const UnitSystem& getDeckUnitSystem() const;
        const UnitSystem& getUnits() const;

        std::string getTitle() const;

        void applyModifierDeck(const Deck& deck);

        const Runspec& runspec() const;

    private:
        void initIOConfigPostSchedule(const Deck& deck);
        void initTransMult();
        void initFaults(const Deck& deck);

        void setMULTFLT(const Ewoms::Section& section);

        void complainAboutAmbiguousKeyword(const Deck& deck,
                                           const std::string& keywordName);

        const TableManager m_tables;
        Runspec m_runspec;
        EclipseConfig m_eclipseConfig;
        UnitSystem m_deckUnitSystem;
        NNC m_inputNnc;
        EDITNNC m_inputEditNnc;
        EclipseGrid m_inputGrid;
        Eclipse3DProperties m_eclipseProperties;
        FieldPropsManager field_props;
        const SimulationConfig m_simulationConfig;
        TransMult m_transMult;

        FaultCollection m_faults;
        std::string m_title;

    };
}

#endif // EWOMS_ECLIPSE_STATE_H
