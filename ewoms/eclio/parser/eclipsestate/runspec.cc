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
#include "config.h"

#include <ostream>
#include <type_traits>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/parserkeywords/c.hh>
#include <ewoms/eclio/parser/parserkeywords/n.hh>
#include <ewoms/eclio/parser/parserkeywords/s.hh>
#include <ewoms/eclio/parser/parserkeywords/t.hh>
#include <ewoms/eclio/parser/parserkeywords/w.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/opmlog/opmlog.hh>

namespace Ewoms {

Phase get_phase( const std::string& str ) {
    if( str == "OIL" ) return Phase::OIL;
    if( str == "GAS" ) return Phase::GAS;
    if( str == "WAT" ) return Phase::WATER;
    if( str == "WATER" )   return Phase::WATER;
    if( str == "SOLVENT" ) return Phase::SOLVENT;
    if( str == "POLYMER" ) return Phase::POLYMER;
    if( str == "ENERGY" ) return Phase::ENERGY;
    if( str == "POLYMW" ) return Phase::POLYMW;
    if( str == "FOAM" ) return Phase::FOAM;
    if( str == "BRINE" ) return Phase::BRINE;
    if( str == "ZFRACTION" ) return Phase::ZFRACTION;

    throw std::invalid_argument( "Unknown phase '" + str + "'" );
}

std::ostream& operator<<( std::ostream& stream, const Phase& p ) {
    switch( p ) {
        case Phase::OIL:     return stream << "OIL";
        case Phase::GAS:     return stream << "GAS";
        case Phase::WATER:   return stream << "WATER";
        case Phase::SOLVENT: return stream << "SOLVENT";
        case Phase::POLYMER: return stream << "POLYMER";
        case Phase::ENERGY:  return stream << "ENERGY";
        case Phase::POLYMW:  return stream << "POLYMW";
        case Phase::FOAM:    return stream << "FOAM";
        case Phase::BRINE:   return stream << "BRINE";
        case Phase::ZFRACTION:    return stream << "ZFRACTION";

    }

    return stream;
}

using un = std::underlying_type< Phase >::type;

Phases::Phases( bool oil, bool gas, bool wat, bool sol, bool pol, bool energy, bool polymw, bool foam, bool brine, bool zfraction) noexcept :
    bits( (oil ? (1 << static_cast< un >( Phase::OIL ) )     : 0) |
          (gas ? (1 << static_cast< un >( Phase::GAS ) )     : 0) |
          (wat ? (1 << static_cast< un >( Phase::WATER ) )   : 0) |
          (sol ? (1 << static_cast< un >( Phase::SOLVENT ) ) : 0) |
          (pol ? (1 << static_cast< un >( Phase::POLYMER ) ) : 0) |
          (energy ? (1 << static_cast< un >( Phase::ENERGY ) ) : 0) |
          (polymw ? (1 << static_cast< un >( Phase::POLYMW ) ) : 0) |
          (foam ? (1 << static_cast< un >( Phase::FOAM ) ) : 0) |
          (brine ? (1 << static_cast< un >( Phase::BRINE ) ) : 0) |
          (zfraction ? (1 << static_cast< un >( Phase::ZFRACTION ) ) : 0) )

{}

Phases Phases::serializeObject()
{
    return Phases(true, true, true, false, true, false, true, false);
}

bool Phases::active( Phase p ) const noexcept {
    return this->bits[ static_cast< int >( p ) ];
}

size_t Phases::size() const noexcept {
    return this->bits.count();
}

bool Phases::operator==(const Phases& data) const {
    return bits == data.bits;
}

Welldims::Welldims(const Deck& deck)
{
    using WD = ParserKeywords::WELLDIMS;
    if (deck.hasKeyword<WD>()) {
        const auto& keyword = deck.getKeyword<WD>(0);
        const auto& wd = keyword.getRecord(0);

        this->nCWMax = wd.getItem<WD::MAXCONN>().get<int>(0);
        this->nWGMax = wd.getItem<WD::MAX_GROUPSIZE>().get<int>(0);

        // This is the E100 definition.  E300 instead uses
        //
        //   Max{ "MAXGROUPS", "MAXWELLS" }
        //
        // i.e., the maximum of item 1 and item 4 here.
        this->nGMax = wd.getItem<WD::MAXGROUPS>().get<int>(0);
	      this->nWMax = wd.getItem<WD::MAXWELLS>().get<int>(0);

        this->m_location = keyword.location();
    }
}

Welldims Welldims::serializeObject()
{
    Welldims result;
    result.nWMax = 1;
    result.nCWMax = 2;
    result.nWGMax = 3;
    result.nGMax = 4;
    result.m_location = KeywordLocation::serializeObject();
    return result;
}

WellSegmentDims::WellSegmentDims() :
    nSegWellMax( ParserKeywords::WSEGDIMS::NSWLMX::defaultValue ),
    nSegmentMax( ParserKeywords::WSEGDIMS::NSEGMX::defaultValue ),
    nLatBranchMax( ParserKeywords::WSEGDIMS::NLBRMX::defaultValue )
{}

WellSegmentDims::WellSegmentDims(const Deck& deck) : WellSegmentDims()
{
    if (deck.hasKeyword("WSEGDIMS")) {
        const auto& wsd = deck.getKeyword("WSEGDIMS", 0).getRecord(0);

        this->nSegWellMax   = wsd.getItem("NSWLMX").get<int>(0);
        this->nSegmentMax   = wsd.getItem("NSEGMX").get<int>(0);
        this->nLatBranchMax = wsd.getItem("NLBRMX").get<int>(0);
    }
}

WellSegmentDims WellSegmentDims::serializeObject()
{
    WellSegmentDims result;
    result.nSegWellMax = 1;
    result.nSegmentMax = 2;
    result.nLatBranchMax = 3;

    return result;
}

bool WellSegmentDims::operator==(const WellSegmentDims& data) const
{
    return this->maxSegmentedWells() == data.maxSegmentedWells() &&
           this->maxSegmentsPerWell() == data.maxSegmentsPerWell() &&
           this->maxLateralBranchesPerWell() == data.maxLateralBranchesPerWell();
}

EclHysterConfig::EclHysterConfig(const Ewoms::Deck& deck)
    {

        if (!deck.hasKeyword("SATOPTS"))
            return;

        const auto& satoptsItem = deck.getKeyword("SATOPTS").getRecord(0).getItem(0);
        for (unsigned i = 0; i < satoptsItem.data_size(); ++i) {
            std::string satoptsValue = satoptsItem.get< std::string >(0);
            std::transform(satoptsValue.begin(),
                           satoptsValue.end(),
                           satoptsValue.begin(),
                           ::toupper);

            if (satoptsValue == "HYSTER")
                activeHyst = true;
        }

        // check for the (deprecated) HYST keyword
        if (deck.hasKeyword("HYST"))
            activeHyst = true;

        if (!activeHyst)
	      return;

        if (!deck.hasKeyword("EHYSTR"))
            throw std::runtime_error("Enabling hysteresis via the HYST parameter for SATOPTS requires the "
                                     "presence of the EHYSTR keyword");
	    /*!
	* \brief Set the type of the hysteresis model which is used for relative permeability.
	*
	* -1: relperm hysteresis is disabled
	* 0: use the Carlson model for relative permeability hysteresis of the non-wetting
	*    phase and the drainage curve for the relperm of the wetting phase
	* 1: use the Carlson model for relative permeability hysteresis of the non-wetting
	*    phase and the imbibition curve for the relperm of the wetting phase
	*/
        const auto& ehystrKeyword = deck.getKeyword("EHYSTR");
        if (deck.hasKeyword("NOHYKR"))
            krHystMod = -1;
        else {
            krHystMod = ehystrKeyword.getRecord(0).getItem("relative_perm_hyst").get<int>(0);

            if (krHystMod != 0 && krHystMod != 1)
                throw std::runtime_error(
                    "Only the Carlson relative permeability hysteresis models (indicated by '0' or "
                    "'1' for the second item of the 'EHYSTR' keyword) are supported");
        }

        // this is slightly screwed: it is possible to specify contradicting hysteresis
        // models with HYPC/NOHYPC and the fifth item of EHYSTR. Let's ignore that for
        // now.
            /*!
	* \brief Return the type of the hysteresis model which is used for capillary pressure.
	*
	* -1: capillary pressure hysteresis is disabled
	* 0: use the Killough model for capillary pressure hysteresis
	*/
        std::string whereFlag =
            ehystrKeyword.getRecord(0).getItem("limiting_hyst_flag").getTrimmedString(0);
        if (deck.hasKeyword("NOHYPC") || whereFlag == "KR")
            pcHystMod = -1;
        else {
            // if capillary pressure hysteresis is enabled, Eclipse always uses the
            // Killough model
            pcHystMod = 0;

            throw std::runtime_error("Capillary pressure hysteresis is not supported yet");
        }
    }

EclHysterConfig EclHysterConfig::serializeObject()
{
    EclHysterConfig result;
    result.activeHyst = true;
    result.pcHystMod = 1;
    result.krHystMod = 2;

    return result;
}

bool EclHysterConfig::active() const
    { return activeHyst; }

int EclHysterConfig::pcHysteresisModel() const
    { return pcHystMod; }

int EclHysterConfig::krHysteresisModel() const
    { return krHystMod; }

bool EclHysterConfig::operator==(const EclHysterConfig& data) const {
    return this->active() == data.active() &&
           this->pcHysteresisModel() == data.pcHysteresisModel() &&
           this->krHysteresisModel() == data.krHysteresisModel();
}

SatFuncControls::SatFuncControls()
    : tolcrit(ParserKeywords::TOLCRIT::VALUE::defaultValue)
{}

SatFuncControls::SatFuncControls(const Deck& deck)
    : SatFuncControls()
{
    using Kw = ParserKeywords::TOLCRIT;

    if (deck.hasKeyword<Kw>()) {
        // SIDouble doesn't perform any unit conversions here since
        // TOLCRIT is a pure scalar (Dimension = 1).
        this->tolcrit = deck.getKeyword<Kw>(0).getRecord(0)
            .getItem<Kw::VALUE>().getSIDouble(0);
    }

    if (deck.hasKeyword<ParserKeywords::STONE1>())
        krmodel = ThreePhaseOilKrModel::Stone1;
    else if (deck.hasKeyword<ParserKeywords::STONE>() ||
             deck.hasKeyword<ParserKeywords::STONE2>())
        krmodel = ThreePhaseOilKrModel::Stone2;
}

SatFuncControls::SatFuncControls(const double tolcritArg,
                                 ThreePhaseOilKrModel model)
    : tolcrit(tolcritArg)
    , krmodel(model)
{}

SatFuncControls SatFuncControls::serializeObject()
{
    SatFuncControls result;
    result.tolcrit = 1.0;
    result.krmodel = ThreePhaseOilKrModel::Stone2;

    return result;
}

bool SatFuncControls::operator==(const SatFuncControls& rhs) const
{
    return this->minimumRelpermMobilityThreshold() == rhs.minimumRelpermMobilityThreshold() &&
           this->krModel() == rhs.krModel();
}

Runspec::Runspec( const Deck& deck ) :
    active_phases( Phases( deck.hasKeyword( "OIL" ),
                           deck.hasKeyword( "GAS" ),
                           deck.hasKeyword( "WATER" ),
                           deck.hasKeyword( "SOLVENT" ),
                           deck.hasKeyword( "POLYMER" ),
                           deck.hasKeyword( "THERMAL" ),
                           deck.hasKeyword( "POLYMW"  ),
                           deck.hasKeyword( "FOAM" ),
                           deck.hasKeyword( "BRINE" ),
                           deck.hasKeyword( "PVTSOL" ) ) ),
    m_tabdims( deck ),
    endscale( deck ),
    welldims( deck ),
    wsegdims( deck ),
    udq_params( deck ),
    hystpar( deck ),
    m_actdims( deck ),
    m_sfuncctrl( deck ),
    m_nupcol( ParserKeywords::NUPCOL::NUM_ITER::defaultValue ),
    m_co2storage (false)
{
    if (DeckSection::hasRUNSPEC(deck)) {
        const RUNSPECSection runspecSection{deck};
        if (runspecSection.hasKeyword("NUPCOL")) {
            using NC = ParserKeywords::NUPCOL;
            const auto& item = runspecSection.getKeyword<NC>().getRecord(0).getItem<NC::NUM_ITER>();
            m_nupcol = item.get<int>(0);
            if (item.defaultApplied(0)) {
                std::string msg = "eflow uses 12 as default NUPCOL value";
                OpmLog::note(msg);
            }
        }
        if (runspecSection.hasKeyword<ParserKeywords::CO2STOR>()) {
            m_co2storage = true;
            std::string msg = "The CO2 storage option is given. PVT properties from the Brine-CO2 system is used";
            OpmLog::note(msg);
        }
    }
}

Runspec Runspec::serializeObject()
{
    Runspec result;
    result.active_phases = Phases::serializeObject();
    result.m_tabdims = Tabdims::serializeObject();
    result.endscale = EndpointScaling::serializeObject();
    result.welldims = Welldims::serializeObject();
    result.wsegdims = WellSegmentDims::serializeObject();
    result.udq_params = UDQParams::serializeObject();
    result.hystpar = EclHysterConfig::serializeObject();
    result.m_actdims = Actdims::serializeObject();
    result.m_sfuncctrl = SatFuncControls::serializeObject();
    result.m_nupcol = 2;
    result.m_co2storage = true;

    return result;
}

const Phases& Runspec::phases() const noexcept {
    return this->active_phases;
}

const Tabdims& Runspec::tabdims() const noexcept {
    return this->m_tabdims;
}

const Actdims& Runspec::actdims() const noexcept {
    return this->m_actdims;
}

const EndpointScaling& Runspec::endpointScaling() const noexcept {
    return this->endscale;
}

const Welldims& Runspec::wellDimensions() const noexcept
{
    return this->welldims;
}

const WellSegmentDims& Runspec::wellSegmentDimensions() const noexcept
{
    return this->wsegdims;
}

const EclHysterConfig& Runspec::hysterPar() const noexcept
{
    return this->hystpar;
}

const SatFuncControls& Runspec::saturationFunctionControls() const noexcept
{
    return this->m_sfuncctrl;
}

int Runspec::nupcol() const noexcept
{
    return this->m_nupcol;
}

bool Runspec::co2Storage() const noexcept
{
    return this->m_co2storage;
}

/*
  Returns an integer in the range 0...7 which can be used to indicate
  available phases in Eclipse restart and init files.
*/
int Runspec::eclPhaseMask( ) const noexcept {
    const int water = 1 << 2;
    const int oil   = 1 << 0;
    const int gas   = 1 << 1;

    return ( active_phases.active( Phase::WATER ) ? water : 0 )
         | ( active_phases.active( Phase::OIL ) ? oil : 0 )
         | ( active_phases.active( Phase::GAS ) ? gas : 0 );
}

const UDQParams& Runspec::udqParams() const noexcept {
    return this->udq_params;
}

bool Runspec::operator==(const Runspec& data) const {
    return this->phases() == data.phases() &&
           this->tabdims() == data.tabdims() &&
           this->endpointScaling() == data.endpointScaling() &&
           this->wellDimensions() == data.wellDimensions() &&
           this->wellSegmentDimensions() == data.wellSegmentDimensions() &&
           this->hysterPar() == data.hysterPar() &&
           this->actdims() == data.actdims() &&
           this->saturationFunctionControls() == data.saturationFunctionControls() &&
           this->m_nupcol == data.m_nupcol &&
           this->m_co2storage == data.m_co2storage;
}

}
