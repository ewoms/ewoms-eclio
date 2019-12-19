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
#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <map>
#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/dynamicstate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/dynamicvector.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/events.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/gtnode.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/guiderateconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/gconsale.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/gconsump.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/oilvaporizationproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/tuning.hh>
#include <ewoms/eclio/parser/eclipsestate/util/orderedmap.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/messagelimits.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/rftconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/vfpinjtable.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/vfpprodtable.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/welltestconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actions.hh>

#include <ewoms/eclio/utility/activegridcells.hh>

/*
  The DynamicState<std::shared_ptr<T>> pattern: The quantities in the Schedule
  section like e.g. wellrates and completion properties are typically
  characterized by the following behaviour:

    1. They can be updated repeatedly at arbitrary points in the Schedule
       section.

    2. The value set at one timestep will apply until is explicitly set again at
       a later timestep.

  These properties are typically stored in a DynamicState<T> container; the
  DynamicState<T> class is a container which implements this semantics:

    1. It is legitimate to ask for an out-of-range value, you will then get the
       last value which has been set.

    2. When assigning an out-of-bounds value the container will append the
       currently set value until correct length has been reached, and then the
       new value will be assigned.

    3. The DynamicState<T> has an awareness of the total length of the time
       axis, trying to access values beyound that is illegal.

  For many of the non-trival objects like eg Well and Group the DynamicState<>
  contains a shared pointer to an underlying object, that way the fill operation
  when the vector is resized is quite fast. The following pattern is quite
  common for the Schedule implementation:

       // Create a new well object.
       std::shared_ptr<Well> new_well = this->getWell( well_name, time_step );

       // Update the new well object with new settings from the deck, the
       // updateXXXX() method will return true if the well object was actually
       // updated:
       if (new_well->updateRate( new_rate ))
           this->dynamic_state.update( time_step, new_well);

*/

namespace Ewoms
{
    class Actions;
    class Deck;
    class DeckKeyword;
    class DeckRecord;
    class EclipseGrid;
    class Eclipse3DProperties;
    class EclipseState;
    class FieldPropsManager;
    class Runspec;
    class SCHEDULESection;
    class SummaryState;
    class TimeMap;
    class UnitSystem;
    class ErrorGuard;
    class WListManager;
    class UDQConfig;
    class UDQActive;

    class Schedule {
    public:
        Schedule(const Deck& deck,
                 const EclipseGrid& grid,
                 const FieldPropsManager& fp,
                 const Eclipse3DProperties& eclipseProperties,
                 const Runspec &runspec,
                 const ParseContext& parseContext,
                 ErrorGuard& errors);

        template<typename T>
        Schedule(const Deck& deck,
                 const EclipseGrid& grid,
                 const FieldPropsManager& fp,
                 const Eclipse3DProperties& eclipseProperties,
                 const Runspec &runspec,
                 const ParseContext& parseContext,
                 T&& errors);

        Schedule(const Deck& deck,
                 const EclipseGrid& grid,
                 const FieldPropsManager& fp,
                 const Eclipse3DProperties& eclipseProperties,
                 const Runspec &runspec);

        Schedule(const Deck& deck,
                 const EclipseState& es,
                 const ParseContext& parseContext,
                 ErrorGuard& errors);

        template <typename T>
        Schedule(const Deck& deck,
                 const EclipseState& es,
                 const ParseContext& parseContext,
                 T&& errors);

        Schedule(const Deck& deck,
                 const EclipseState& es);

        /*
         * If the input deck does not specify a start time, Eclipse's 1. Jan
         * 1983 is defaulted
         */
        time_t getStartTime() const;
        time_t posixStartTime() const;
        time_t posixEndTime() const;
        time_t simTime(size_t timeStep) const;
        double seconds(size_t timeStep) const;
        double stepLength(size_t timeStep) const;

        const TimeMap& getTimeMap() const;

        size_t numWells() const;
        size_t numWells(size_t timestep) const;
        bool hasWell(const std::string& wellName) const;
        bool hasWell(const std::string& wellName, std::size_t timeStep) const;

        std::vector<std::string> wellNames(const std::string& pattern, size_t timeStep, const std::vector<std::string>& matching_wells = {}) const;
        std::vector<std::string> wellNames(const std::string& pattern) const;
        std::vector<std::string> wellNames(size_t timeStep) const;
        std::vector<std::string> wellNames() const;

        std::vector<std::string> groupNames(const std::string& pattern, size_t timeStep) const;
        std::vector<std::string> groupNames(size_t timeStep) const;
        std::vector<std::string> groupNames(const std::string& pattern) const;
        std::vector<std::string> groupNames() const;

        void updateWell(std::shared_ptr<Well> well, size_t reportStep);
        const Well& getWell(const std::string& wellName, size_t timeStep) const;
        const Well& getWellatEnd(const std::string& well_name) const;
        std::vector<Well> getWells(size_t timeStep) const;
        std::vector<Well> getWellsatEnd() const;

        std::vector<const Group*> getChildGroups2(const std::string& group_name, size_t timeStep) const;
        std::vector<Well> getChildWells2(const std::string& group_name, size_t timeStep) const;
        const OilVaporizationProperties& getOilVaporizationProperties(size_t timestep) const;
        const Well::ProducerCMode& getGlobalWhistctlMmode(size_t timestep) const;

        const UDQActive& udqActive(size_t timeStep) const;
        const WellTestConfig& wtestConfig(size_t timestep) const;
        const GConSale& gConSale(size_t timestep) const;
        const GConSump& gConSump(size_t timestep) const;
        const WListManager& getWListManager(size_t timeStep) const;
        const UDQConfig& getUDQConfig(size_t timeStep) const;
        const Action::Actions& actions(std::size_t timeStep) const;
        void evalAction(const SummaryState& summary_state, size_t timeStep);

        GTNode groupTree(std::size_t report_step) const;
        GTNode groupTree(const std::string& root_node, std::size_t report_step) const;
        size_t numGroups() const;
        size_t numGroups(size_t timeStep) const;
        bool hasGroup(const std::string& groupName) const;
        bool hasGroup(const std::string& groupName, std::size_t timeStep) const;
        const Group& getGroup(const std::string& groupName, size_t timeStep) const;

        const Tuning& getTuning() const;
        const MessageLimits& getMessageLimits() const;
        void invalidNamePattern (const std::string& namePattern, const ParseContext& parseContext, ErrorGuard& errors, const DeckKeyword& keyword) const;
        const GuideRateConfig& guideRateConfig(size_t timeStep) const;

        const RFTConfig& rftConfig() const;
        const Events& getEvents() const;
        const Events& getWellGroupEvents(const std::string& wellGroup) const;
        bool hasWellGroupEvent(const std::string& wellGroup, uint64_t event_mask, size_t reportStep) const;
        const Deck& getModifierDeck(size_t timeStep) const;
        bool hasOilVaporizationProperties() const;
        const VFPProdTable& getVFPProdTable(int table_id, size_t timeStep) const;
        const VFPInjTable& getVFPInjTable(int table_id, size_t timeStep) const;
        std::map<int, std::shared_ptr<const VFPProdTable> > getVFPProdTables(size_t timeStep) const;
        std::map<int, std::shared_ptr<const VFPInjTable> > getVFPInjTables(size_t timeStep) const;
        /*
          Will remove all completions which are connected to cell which is not
          active. Will scan through all wells and all timesteps.
        */
        void filterConnections(const ActiveGridCells& grid);
        size_t size() const;

        void applyAction(size_t reportStep, const Action::ActionX& action, const Action::Result& result);
        int getNupcol(size_t reportStep) const;
    private:
        TimeMap m_timeMap;
        OrderedMap< std::string, DynamicState<std::shared_ptr<Well>>> wells_static;
        OrderedMap< std::string, DynamicState<std::shared_ptr<Group>>> groups;
        DynamicState< OilVaporizationProperties > m_oilvaporizationproperties;
        Events m_events;
        DynamicVector< Deck > m_modifierDeck;
        Tuning m_tuning;
        MessageLimits m_messageLimits;
        Runspec m_runspec;
        std::map<int, DynamicState<std::shared_ptr<VFPProdTable>>> vfpprod_tables;
        std::map<int, DynamicState<std::shared_ptr<VFPInjTable>>> vfpinj_tables;
        DynamicState<std::shared_ptr<WellTestConfig>> wtest_config;
        DynamicState<std::shared_ptr<WListManager>> wlist_manager;
        DynamicState<std::shared_ptr<UDQConfig>> udq_config;
        DynamicState<std::shared_ptr<UDQActive>> udq_active;
        DynamicState<std::shared_ptr<GuideRateConfig>> guide_rate_config;
        DynamicState<std::shared_ptr<GConSale>> gconsale;
        DynamicState<std::shared_ptr<GConSump>> gconsump;
        DynamicState<Well::ProducerCMode> global_whistctl_mode;
        DynamicState<std::shared_ptr<Action::Actions>> m_actions;
        RFTConfig rft_config;
        DynamicState<int> m_nupcol;

        std::map<std::string,Events> wellgroup_events;

        GTNode groupTree(const std::string& root_node, std::size_t report_step, const GTNode * parent) const;
        void updateGroup(std::shared_ptr<Group> group, size_t reportStep);
        bool checkGroups(const ParseContext& parseContext, ErrorGuard& errors);
        void updateUDQActive( std::size_t timeStep, std::shared_ptr<UDQActive> udq );
        bool updateWellStatus( const std::string& well, size_t reportStep , Well::Status status);
        void addWellToGroup( const std::string& group_name, const std::string& well_name , size_t timeStep);
        void iterateScheduleSection(const ParseContext& parseContext ,  ErrorGuard& errors, const SCHEDULESection& , const EclipseGrid& grid,
                                    const FieldPropsManager& fp,
                                    const Eclipse3DProperties& eclipseProperties);
        void addACTIONX(const Action::ActionX& action, std::size_t currentStep);
        void addGroupToGroup( const std::string& parent_group, const std::string& child_group, size_t timeStep);
        void addGroupToGroup( const std::string& parent_group, const Group& child_group, size_t timeStep);
        void addGroup(const std::string& groupName , size_t timeStep, const UnitSystem& unit_system);
        void addWell(const std::string& wellName, const DeckRecord& record, size_t timeStep, Connection::Order connection_order, const UnitSystem& unit_system);
        void handleUDQ(const DeckKeyword& keyword, size_t currentStep);
        void handleWLIST(const DeckKeyword& keyword, size_t currentStep);
        void handleCOMPORD(const ParseContext& parseContext, ErrorGuard& errors, const DeckKeyword& compordKeyword, size_t currentStep);
        void handleWELSPECS( const SCHEDULESection&, size_t, size_t , const UnitSystem& unit_system, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWCONHIST( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWCONPROD( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWGRUPCON( const DeckKeyword& keyword, size_t currentStep);
        void handleCOMPDAT( const DeckKeyword& keyword,  size_t currentStep, const EclipseGrid& grid, const FieldPropsManager& fp, const Eclipse3DProperties& eclipseProperties, const ParseContext& parseContext, ErrorGuard& errors);
        void handleCOMPLUMP( const DeckKeyword& keyword,  size_t currentStep );
        void handleWELSEGS( const DeckKeyword& keyword, size_t currentStep);
        void handleCOMPSEGS( const DeckKeyword& keyword, size_t currentStep, const EclipseGrid& grid, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWSEGSICD( const DeckKeyword& keyword, size_t currentStep);
        // TODO: we should incorporate ParseContext and ErrorGuard, including the above keyword
        void handleWSEGVALV( const DeckKeyword& keyword, size_t currentStep);

        void handleWCONINJE( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWFOAM( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWPOLYMER( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWSOLVENT( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWTRACER( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWTEMP( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWPMITAB( const DeckKeyword& keyword,  const size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWSKPTAB( const DeckKeyword& keyword,  const size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWINJTEMP( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWCONINJH(const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWELOPEN( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors, const std::vector<std::string>& matching_wells = {});
        void handleWELTARG( const SCHEDULESection&,  const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleGCONINJE( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleGCONPROD( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleGEFAC( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleGCONSALE( const DeckKeyword& keyword, size_t currentStep, const UnitSystem& unit_system);
        void handleGCONSUMP( const DeckKeyword& keyword, size_t currentStep, const UnitSystem& unit_system);
        void handleGUIDERAT( const DeckKeyword& keyword, size_t currentStep);
        void handleLINCOM( const DeckKeyword& keyword, size_t currentStep);
        void handleWEFAC( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);

        void handleTUNING( const DeckKeyword& keyword, size_t currentStep);
        void handleNUPCOL( const DeckKeyword& keyword, size_t currentStep);
        void handleGRUPTREE( const DeckKeyword& keyword, size_t currentStep, const UnitSystem& unit_system, const ParseContext& parseContext, ErrorGuard& errors);
        void handleGRUPNET( const DeckKeyword& keyword, size_t currentStep, const UnitSystem& unit_system);
        void handleWRFT( const DeckKeyword& keyword, size_t currentStep);
        void handleWTEST( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWRFTPLT( const DeckKeyword& keyword, size_t currentStep);
        void handleWPIMULT( const DeckKeyword& keyword, size_t currentStep);
        void handleDRSDT( const DeckKeyword& keyword, size_t currentStep);
        void handleDRVDT( const DeckKeyword& keyword, size_t currentStep);
        void handleDRSDTR( const DeckKeyword& keyword, size_t currentStep);
        void handleDRVDTR( const DeckKeyword& keyword, size_t currentStep);
        void handleVAPPARS( const DeckKeyword& keyword, size_t currentStep);
        void handleWECON( const DeckKeyword& keyword, size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleWHISTCTL(const DeckKeyword& keyword, std::size_t currentStep, const ParseContext& parseContext, ErrorGuard& errors);
        void handleMESSAGES(const DeckKeyword& keyword, size_t currentStep);
        void handleVFPPROD(const DeckKeyword& vfpprodKeyword, const UnitSystem& unit_system, size_t currentStep);
        void handleVFPINJ(const DeckKeyword& vfpprodKeyword, const UnitSystem& unit_system, size_t currentStep);
        void checkUnhandledKeywords( const SCHEDULESection& ) const;
        void checkIfAllConnectionsIsShut(size_t currentStep);
        void handleKeyword(size_t& currentStep,
                           const SCHEDULESection& section,
                           size_t keywordIdx,
                           const DeckKeyword& keyword,
                           const ParseContext& parseContext, ErrorGuard& errors,
                           const EclipseGrid& grid,
                           const FieldPropsManager& fp,
                           const Eclipse3DProperties& eclipseProperties,
                           const UnitSystem& unit_system,
                           std::vector<std::pair<const DeckKeyword*, size_t > >& rftProperties);
        void addWellGroupEvent(const std::string& wellGroup, ScheduleEvents::Events event, size_t reportStep);
    };
}

#endif
