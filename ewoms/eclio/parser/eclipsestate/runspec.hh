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

#ifndef EWOMS_RUNSPEC_H
#define EWOMS_RUNSPEC_H

#include <iosfwd>
#include <string>

#include <ewoms/eclio/parser/eclipsestate/tables/tabdims.hh>
#include <ewoms/eclio/parser/eclipsestate/endpointscaling.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actdims.hh>

namespace Ewoms {
class Deck;

enum class Phase {
    OIL     = 0,
    GAS     = 1,
    WATER   = 2,
    SOLVENT = 3,
    POLYMER = 4,
    ENERGY  = 5,
    POLYMW  = 6,
    FOAM  = 7,
    BRINE = 8
    // If you add more entries to this enum, remember to update NUM_PHASES_IN_ENUM below.
};

constexpr int NUM_PHASES_IN_ENUM = static_cast<int>(Phase::BRINE) + 1;  // Used to get correct size of the bitset in class Phases.

Phase get_phase( const std::string& );
std::ostream& operator<<( std::ostream&, const Phase& );

class Phases {
    public:
        Phases() noexcept = default;
        Phases( bool oil, bool gas, bool wat, bool solvent = false, bool polymer = false, bool energy = false,
                bool polymw = false, bool foam = false, bool brine = false ) noexcept;
        Phases(const std::bitset<NUM_PHASES_IN_ENUM>& bbits);
        unsigned long getBits() const;

        bool active( Phase ) const noexcept;
        size_t size() const noexcept;

        bool operator==(const Phases& data) const;

    private:
        std::bitset< NUM_PHASES_IN_ENUM > bits;
};

class Welldims {
public:
    Welldims() = default;
    explicit Welldims(const Deck& deck);
    Welldims(int WMax, int CWMax, int WGMax, int GMax) :
      nWMax(WMax), nCWMax(CWMax), nWGMax(WGMax), nGMax(GMax) {}

    int maxConnPerWell() const
    {
        return this->nCWMax;
    }

    int maxWellsPerGroup() const
    {
        return this->nWGMax;
    }

    int maxGroupsInField() const
    {
        return this->nGMax;
    }

        int maxWellsInField() const
    {
        return this->nWMax;
    }

    bool operator==(const Welldims& data) const {
        return this->maxConnPerWell() == data.maxConnPerWell() &&
               this->maxWellsPerGroup() == data.maxWellsPerGroup() &&
               this->maxGroupsInField() == data.maxGroupsInField() &&
               this->maxWellsInField() == data.maxWellsInField();
    }

private:
    int nWMax  { 0 };
    int nCWMax { 0 };
    int nWGMax { 0 };
    int nGMax  { 0 };
};

class WellSegmentDims {
public:
    WellSegmentDims();
    explicit WellSegmentDims(const Deck& deck);
    WellSegmentDims(int segWellMax, int segMax, int latBranchMax);

    int maxSegmentedWells() const
    {
        return this->nSegWellMax;
    }

    int maxSegmentsPerWell() const
    {
        return this->nSegmentMax;
    }

    int maxLateralBranchesPerWell() const
    {
        return this->nLatBranchMax;
    }

    bool operator==(const WellSegmentDims& data) const;

private:
    int nSegWellMax;
    int nSegmentMax;
    int nLatBranchMax;
};

class EclHysterConfig
{
public:
    EclHysterConfig() = default;
    explicit EclHysterConfig(const Deck& deck);
    EclHysterConfig(bool active, int pcMod, int krMod);

    /*!
     * \brief Specify whether hysteresis is enabled or not.
     */
    //void setActive(bool yesno);

    /*!
     * \brief Returns whether hysteresis is enabled (active).
     */
    bool active() const;

    /*!
     * \brief Return the type of the hysteresis model which is used for capillary pressure.
     *
     * -1: capillary pressure hysteresis is disabled
     * 0: use the Killough model for capillary pressure hysteresis
     */
    int pcHysteresisModel() const;

    /*!
     * \brief Return the type of the hysteresis model which is used for relative permeability.
     *
     * -1: relperm hysteresis is disabled
     * 0: use the Carlson model for relative permeability hysteresis
     */
    int krHysteresisModel() const;

    bool operator==(const EclHysterConfig& data) const;

private:
    // enable hysteresis at all
    bool activeHyst  { false };

    // the capillary pressure and the relperm hysteresis models to be used
    int pcHystMod { 0 };
    int krHystMod { 0 };
};

class SatFuncControls {
public:
    enum class ThreePhaseOilKrModel {
        Default,
        Stone1,
        Stone2
    };

    SatFuncControls();
    explicit SatFuncControls(const Deck& deck);
    SatFuncControls(const double tolcritArg,
                    ThreePhaseOilKrModel model);

    double minimumRelpermMobilityThreshold() const
    {
        return this->tolcrit;
    }

    ThreePhaseOilKrModel krModel() const
    {
        return this->krmodel;
    }

    bool operator==(const SatFuncControls& rhs) const;

private:
    double tolcrit;
    ThreePhaseOilKrModel krmodel = ThreePhaseOilKrModel::Default;
};

class Runspec {
public:
    Runspec() = default;
    explicit Runspec( const Deck& );
    Runspec(const Phases& act_phases,
            const Tabdims& tabdims,
            const EndpointScaling& endScale,
            const Welldims& wellDims,
            const WellSegmentDims& wsegDims,
            const UDQParams& udqparams,
            const EclHysterConfig& hystPar,
            const Actdims& actDims,
            const SatFuncControls& sfuncctrl);

    const UDQParams& udqParams() const noexcept;
    const Phases& phases() const noexcept;
    const Tabdims&  tabdims() const noexcept;
    const EndpointScaling& endpointScaling() const noexcept;
    const Welldims& wellDimensions() const noexcept;
    const WellSegmentDims& wellSegmentDimensions() const noexcept;
    int eclPhaseMask( ) const noexcept;
    const EclHysterConfig& hysterPar() const noexcept;
    const Actdims& actdims() const noexcept;
    const SatFuncControls& saturationFunctionControls() const noexcept;

    bool operator==(const Runspec& data) const;

private:
    Phases active_phases;
    Tabdims m_tabdims;
    EndpointScaling endscale;
    Welldims welldims;
    WellSegmentDims wsegdims;
    UDQParams udq_params;
    EclHysterConfig hystpar;
    Actdims m_actdims;
    SatFuncControls m_sfuncctrl;
};

}

#endif // EWOMS_RUNSPEC_H
