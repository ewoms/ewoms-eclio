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

#include <set>

#include <boost/algorithm/string/join.hpp>

#include <ewoms/eclio/opmlog/logutil.hh>

#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/boxmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultcollection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fault.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/multregtscanner.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/nnc.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/satfuncpropertyinitializers.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/transmult.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/initconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/simulationconfig/simulationconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/parserkeywords/m.hh>
#include <ewoms/eclio/parser/units/dimension.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

namespace Ewoms {

/*
This Function is used in Python to check if the
ENABBLE_3DPROPS_TESTING macro has been set.
*/
#ifdef ENABLE_3DPROPS_TESTING
bool enable3DPropsTesting() {
    return true;
}
#else
bool enable3DPropsTesting() {
    return false;
}
#endif

    EclipseState::EclipseState(const Deck& deck , const ParseContext& parseContext, ErrorGuard& errors) :
        m_tables(            deck ),
        m_runspec(           deck ),
        m_eclipseConfig(     deck, parseContext, errors ),
        m_deckUnitSystem(    deck.getActiveUnitSystem() ),
        m_inputNnc(          deck ),
        m_inputEditNnc(      deck ),
        m_inputGrid(         deck, nullptr ),
        field_props(         deck, m_inputGrid, m_tables),
        m_simulationConfig(  m_eclipseConfig.getInitConfig().restartRequested(), deck, field_props),
        m_transMult(         GridDims(deck), deck, field_props)
    {
        m_inputGrid.resetACTNUM(this->field_props.actnum());
        if( this->runspec().phases().size() < 3 )
            OpmLog::info("Only " + std::to_string( this->runspec().phases().size() )
                                                                + " fluid phases are enabled" );

        if (deck.hasKeyword( "TITLE" )) {
            const auto& titleKeyword = deck.getKeyword( "TITLE" );
            const auto& item = titleKeyword.getRecord( 0 ).getItem( 0 );
            std::vector<std::string> itemValue = item.getData<std::string>();
            m_title = boost::algorithm::join( itemValue, " " );
        }

        initTransMult();
        initFaults(deck);
        this->field_props.reset_actnum( this->m_inputGrid.getACTNUM() );
    }

    template<typename T>
    EclipseState::EclipseState(const Deck& deck, const ParseContext& parseContext, T&& errors) :
        EclipseState(deck, parseContext, errors)
    {}

    EclipseState::EclipseState(const Deck& deck) :
        EclipseState(deck, ParseContext(), ErrorGuard())
    {}

    const UnitSystem& EclipseState::getDeckUnitSystem() const {
        return m_deckUnitSystem;
    }

    const UnitSystem& EclipseState::getUnits() const {
        return m_deckUnitSystem;
    }

    const EclipseGrid& EclipseState::getInputGrid() const {
        return m_inputGrid;
    }

    const RestartConfig& EclipseState::getRestartConfig() const {
        return m_eclipseConfig.getRestartConfig();
    }

    const SimulationConfig& EclipseState::getSimulationConfig() const {
        return m_simulationConfig;
    }

    RestartConfig& EclipseState::getRestartConfig() {
        return const_cast< RestartConfig& >( m_eclipseConfig.getRestartConfig() );
    }

    const FieldPropsManager& EclipseState::fieldProps() const {
        return this->field_props;
    }

    const TableManager& EclipseState::getTableManager() const {
        return m_tables;
    }

    /// [[deprecated]] --- use cfg().io()
    const IOConfig& EclipseState::getIOConfig() const {
        return m_eclipseConfig.io();
    }

    /// [[deprecated]] --- use cfg().io()
    IOConfig& EclipseState::getIOConfig() {
        return m_eclipseConfig.io();
    }

    /// [[deprecated]] --- use cfg().init()
    const InitConfig& EclipseState::getInitConfig() const {
        return m_eclipseConfig.getInitConfig();
    }

    /// [[deprecated]] --- use cfg()
    const EclipseConfig& EclipseState::getEclipseConfig() const {
        return cfg();
    }

    const EclipseConfig& EclipseState::cfg() const {
        return m_eclipseConfig;
    }

    const Runspec& EclipseState::runspec() const {
        return this->m_runspec;
    }

    const FaultCollection& EclipseState::getFaults() const {
        return m_faults;
    }

    const TransMult& EclipseState::getTransMult() const {
        return m_transMult;
    }

    const NNC& EclipseState::getInputNNC() const {
        return m_inputNnc;
    }

    bool EclipseState::hasInputNNC() const {
        return m_inputNnc.hasNNC();
    }

    const EDITNNC& EclipseState::getInputEDITNNC() const {
        return m_inputEditNnc;
    }

    bool EclipseState::hasInputEDITNNC() const {
        return !m_inputEditNnc.empty();
    }
    std::string EclipseState::getTitle() const {
        return m_title;
    }

    void EclipseState::initTransMult() {
        const auto& fp = this->field_props;
        if (fp.has<double>("MULTX"))  this->m_transMult.applyMULT(fp.get_global<double>("MULTX") , FaceDir::XPlus);
        if (fp.has<double>("MULTX-")) this->m_transMult.applyMULT(fp.get_global<double>("MULTX-"), FaceDir::XMinus);

        if (fp.has<double>("MULTY"))  this->m_transMult.applyMULT(fp.get_global<double>("MULTY") , FaceDir::YPlus);
        if (fp.has<double>("MULTY-")) this->m_transMult.applyMULT(fp.get_global<double>("MULTY-"), FaceDir::YMinus);

        if (fp.has<double>("MULTZ"))  this->m_transMult.applyMULT(fp.get_global<double>("MULTZ") , FaceDir::ZPlus);
        if (fp.has<double>("MULTZ-")) this->m_transMult.applyMULT(fp.get_global<double>("MULTZ-"), FaceDir::ZMinus);
    }

    void EclipseState::initFaults(const Deck& deck) {
        if (!DeckSection::hasGRID(deck))
            return;

        const GRIDSection gridSection ( deck );

        m_faults = FaultCollection(gridSection, m_inputGrid);
        setMULTFLT(gridSection);

        if (DeckSection::hasEDIT(deck)) {
            setMULTFLT(EDITSection ( deck ));
        }

        m_transMult.applyMULTFLT( m_faults );
    }

    void EclipseState::setMULTFLT(const DeckSection& section) {
        for (size_t index=0; index < section.count("MULTFLT"); index++) {
            const auto& faultsKeyword = section.getKeyword("MULTFLT" , index);
            for (auto iter = faultsKeyword.begin(); iter != faultsKeyword.end(); ++iter) {

                const auto& faultRecord = *iter;
                const std::string& faultName = faultRecord.getItem(0).get< std::string >(0);
                double multFlt = faultRecord.getItem(1).get< double >(0);

                m_faults.setTransMult( faultName , multFlt );
            }
        }
    }

    void EclipseState::complainAboutAmbiguousKeyword(const Deck& deck, const std::string& keywordName) {
        OpmLog::error("The " + keywordName + " keyword must be unique in the deck. Ignoring all!");
        auto keywords = deck.getKeywordList(keywordName);
        for (size_t i = 0; i < keywords.size(); ++i) {
            std::string msg = "Ambiguous keyword "+keywordName+" defined here";
            OpmLog::error(Log::fileMessage(keywords[i]->location(), msg));
        }
    }

    void EclipseState::applyModifierDeck(const Deck& deck) {
        using namespace ParserKeywords;
        for (const auto& keyword : deck) {

            if (keyword.isKeyword<MULTFLT>()) {
                for (const auto& record : keyword) {
                    const std::string& faultName = record.getItem<MULTFLT::fault>().get< std::string >(0);
                    auto& fault = m_faults.getFault( faultName );
                    double tmpMultFlt = record.getItem<MULTFLT::factor>().get< double >(0);
                    double oldMultFlt = fault.getTransMult( );
                    double newMultFlt = oldMultFlt * tmpMultFlt;

                    /*
                      This extremely contrived way of doing it is because of difference in
                      behavior and section awareness between the Fault object and the
                      Transmult object:

                      1. MULTFLT keywords found in the SCHEDULE section should apply the
                         transmissibility modifiers cumulatively - i.e. the current
                         transmissibility across the fault should be *multiplied* with the
                         newly entered MULTFLT value, and the resulting transmissibility
                         multplier for this fault should be the product of the newly
                         entered value and the current value.

                      2. The TransMult::applyMULTFLT() implementation will *multiply* the
                         transmissibility across a face with the value in the fault
                         object. Hence the current value has already been multiplied in;
                         we therefor first *set* the MULTFLT value to the new value, then
                         apply it to the TransMult object and then eventually update the
                         MULTFLT value in the fault instance.

                    */
                    fault.setTransMult( tmpMultFlt );
                    m_transMult.applyMULTFLT( fault );
                    fault.setTransMult( newMultFlt );
                }
            }
        }
    }
}
