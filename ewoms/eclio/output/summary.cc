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

#include <ewoms/eclio/output/summary.hh>

#include <ewoms/eclio/opmlog/opmlog.hh>
#include <ewoms/eclio/opmlog/keywordlocation.hh>
#include <ewoms/eclio/utility/opminputerror.hh>

#include <ewoms/eclio/output/inplace.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqcontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellproductionproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellinjectionproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/summaryconfig/summaryconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/units/units.hh>

#include <ewoms/eclio/io/ecloutput.hh>
#include <ewoms/eclio/io/outputstream.hh>

#include <ewoms/eclio/output/data/groups.hh>
#include <ewoms/eclio/output/data/guideratevalue.hh>
#include <ewoms/eclio/output/data/wells.hh>
#include <ewoms/eclio/output/data/aquifer.hh>
#include <ewoms/eclio/output/inplace.hh>
#include <ewoms/eclio/output/regioncache.hh>

#include <ewoms/common/fmt/format.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cctype>
#include <ctime>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <numeric>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
    struct ParamCTorArgs
    {
        std::string kw;
        Ewoms::EclIO::SummaryNode::Type type;
    };

    using p_cmode = Ewoms::Group::ProductionCMode;
    const std::map<p_cmode, int> pCModeToPCntlMode = {
                                                    {p_cmode::NONE,       0},
                                                    {p_cmode::ORAT,       1},
                                                    {p_cmode::WRAT,       2},
                                                    {p_cmode::GRAT,       3},
                                                    {p_cmode::LRAT,       4},
                                                    {p_cmode::CRAT,       9},
                                                    {p_cmode::RESV,       5},
                                                    {p_cmode::PRBL,       6},
                                                    {p_cmode::FLD,        0}, // same as NONE

    };

    using i_cmode = Ewoms::Group::InjectionCMode;
    const std::map<i_cmode, int> iCModeToICntlMode = {
                                                    {i_cmode::NONE,       0},
                                                    {i_cmode::RATE,       1},
                                                    {i_cmode::RESV,       2},
                                                    {i_cmode::REIN,       3},
                                                    {i_cmode::VREP,       4},
                                                    {i_cmode::FLD,        0},  // same as NONE
                                                    {i_cmode::SALE,       0},  // not used in E100
    };

    std::vector<ParamCTorArgs> requiredRestartVectors()
    {
        using Type = ::Ewoms::EclIO::SummaryNode::Type;

        return {
            // Production
            ParamCTorArgs{ "OPR" , Type::Rate },
            ParamCTorArgs{ "WPR" , Type::Rate },
            ParamCTorArgs{ "GPR" , Type::Rate },
            ParamCTorArgs{ "VPR" , Type::Rate },
            ParamCTorArgs{ "OPP" , Type::Rate },
            ParamCTorArgs{ "WPP" , Type::Rate },
            ParamCTorArgs{ "GPP" , Type::Rate },
            ParamCTorArgs{ "OPT" , Type::Total },
            ParamCTorArgs{ "WPT" , Type::Total },
            ParamCTorArgs{ "GPT" , Type::Total },
            ParamCTorArgs{ "VPT" , Type::Total },
            ParamCTorArgs{ "OPTH", Type::Total },
            ParamCTorArgs{ "WPTH", Type::Total },
            ParamCTorArgs{ "GPTH", Type::Total },

            // Flow rate ratios (production)
            ParamCTorArgs{ "WCT" , Type::Ratio },
            ParamCTorArgs{ "GOR" , Type::Ratio },

            // injection
            ParamCTorArgs{ "WIR" , Type::Rate },
            ParamCTorArgs{ "GIR" , Type::Rate },
            ParamCTorArgs{ "OPI" , Type::Rate },
            ParamCTorArgs{ "WPI" , Type::Rate },
            ParamCTorArgs{ "GPI" , Type::Rate },
            ParamCTorArgs{ "WIT" , Type::Total },
            ParamCTorArgs{ "GIT" , Type::Total },
            ParamCTorArgs{ "VIT" , Type::Total },
            ParamCTorArgs{ "WITH", Type::Total },
            ParamCTorArgs{ "GITH", Type::Total },
        };
    }

    std::vector<Ewoms::EclIO::SummaryNode>
    requiredRestartVectors(const ::Ewoms::Schedule& sched)
    {
        auto entities = std::vector<Ewoms::EclIO::SummaryNode> {};

        const auto vectors = requiredRestartVectors();
        const auto extra_well_vectors = std::vector<ParamCTorArgs> {
            { "WTHP",  Ewoms::EclIO::SummaryNode::Type::Pressure },
            { "WBHP",  Ewoms::EclIO::SummaryNode::Type::Pressure },
            { "WGVIR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "WWVIR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "WOPGR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "WGPGR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "WWPGR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "WGIGR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "WWIGR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "WMCTL", Ewoms::EclIO::SummaryNode::Type::Mode     },
        };
        const auto extra_group_vectors = std::vector<ParamCTorArgs> {
            { "GOPGR", Ewoms::EclIO::SummaryNode::Type::Rate },
            { "GGPGR", Ewoms::EclIO::SummaryNode::Type::Rate },
            { "GWPGR", Ewoms::EclIO::SummaryNode::Type::Rate },
            { "GGIGR", Ewoms::EclIO::SummaryNode::Type::Rate },
            { "GWIGR", Ewoms::EclIO::SummaryNode::Type::Rate },
            { "GMCTG", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "GMCTP", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "GMCTW", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "GMWPR", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "GMWIN", Ewoms::EclIO::SummaryNode::Type::Mode },
        };
        const auto extra_field_vectors = std::vector<ParamCTorArgs> {
            { "FMCTG", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "FMCTP", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "FMCTW", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "FMWPR", Ewoms::EclIO::SummaryNode::Type::Mode },
            { "FMWIN", Ewoms::EclIO::SummaryNode::Type::Mode },
        };

        using Cat = Ewoms::EclIO::SummaryNode::Category;

        auto makeEntities = [&vectors, &entities]
            (const char                        kwpref,
             const Cat                         cat,
             const std::vector<ParamCTorArgs>& extra_vectors,
             const std::string&                name) -> void
        {
            const auto dflt_num = Ewoms::EclIO::SummaryNode::default_number;

            // Recall: Cannot use emplace_back() for PODs.
            for (const auto& vector : vectors) {
                Ewoms::EclIO::SummaryNode node = {
                    .keyword = kwpref + vector.kw,
                    .category = cat,
                    .type = vector.type,
                    .wgname =  name,
                    .number = dflt_num,
                    .fip_region = std::string("")
                };

                entities.push_back(node);
                                        
            }

            for (const auto& extra_vector : extra_vectors) {
                Ewoms::EclIO::SummaryNode node = {
                    .keyword = extra_vector.kw,
                    .category = cat,
                    .type = extra_vector.type,
                    .wgname =  name,
                    .number = dflt_num,
                    .fip_region = std::string("")
                };

                entities.push_back(node);
            }
        };

        for (const auto& well_name : sched.wellNames()) {
            makeEntities('W', Cat::Well, extra_well_vectors, well_name);
        }

        for (const auto& grp_name : sched.groupNames()) {
            if (grp_name == "FIELD") { continue; }

            makeEntities('G', Cat::Group, extra_group_vectors, grp_name);
        }

        makeEntities('F', Cat::Field, extra_field_vectors, "FIELD");

        return entities;
    }

    std::vector<Ewoms::EclIO::SummaryNode>
    requiredSegmentVectors(const ::Ewoms::Schedule& sched)
    {
        std::vector<Ewoms::EclIO::SummaryNode> ret {};

        constexpr Ewoms::EclIO::SummaryNode::Category category { Ewoms::EclIO::SummaryNode::Category::Segment };
        const std::vector<std::pair<std::string,Ewoms::EclIO::SummaryNode::Type>> requiredVectors {
            { "SOFR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "SGFR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "SWFR", Ewoms::EclIO::SummaryNode::Type::Rate     },
            { "SPR",  Ewoms::EclIO::SummaryNode::Type::Pressure },
        };

        auto makeVectors =
            [&](const std::string& well,
                const int          segNumber) -> void
        {
            for (const auto &requiredVector : requiredVectors) {
                Ewoms::EclIO::SummaryNode node = {
                    .keyword = requiredVector.first,
                    .category = category,
                    .type = requiredVector.second,
                    .wgname =  well,
                    .number = segNumber,
                    .fip_region = std::string("")
                };

                ret.push_back(node);
            }
        };

        for (const auto& wname : sched.wellNames()) {
            const auto& well = sched.getWellatEnd(wname);

            if (! well.isMultiSegment()) {
                // Don't allocate MS summary vectors for non-MS wells.
                continue;
            }

            const auto nSeg = well.getSegments().size();

            for (auto segID = 0*nSeg; segID < nSeg; ++segID) {
                makeVectors(wname, segID + 1); // One-based
            }
        }

        return ret;
    }

Ewoms::TimeStampUTC make_sim_time(const Ewoms::Schedule& sched, const Ewoms::SummaryState& st, double sim_step) {
    auto elapsed = st.get_elapsed() + sim_step;
    return Ewoms::TimeStampUTC( sched.getStartTime() )  + std::chrono::duration<double>(elapsed);
}

/*
 * This class takes simulator state and parser-provided information and
 * orchestrates ert to write simulation results as requested by the SUMMARY
 * section in eclipse. The implementation is somewhat compact as a lot of the
 * requested output may be similar-but-not-quite. Through various techniques
 * the compiler writes a lot of this code for us.
 */
#include "config.h"

using rt = Ewoms::data::Rates::opt;
using measure = Ewoms::UnitSystem::measure;
constexpr const bool injector = true;
constexpr const bool producer = false;

/* Some numerical value with its unit tag embedded to enable caller to apply
 * unit conversion. This removes a lot of boilerplate. ad-hoc solution to poor
 * unit support in general.
 */
measure div_unit( measure denom, measure div ) {
    if( denom == measure::gas_surface_rate &&
        div   == measure::liquid_surface_rate )
        return measure::gas_oil_ratio;

    if( denom == measure::liquid_surface_rate &&
        div   == measure::gas_surface_rate )
        return measure::oil_gas_ratio;

    if( denom == measure::liquid_surface_rate &&
        div   == measure::liquid_surface_rate )
        return measure::water_cut;

    if( denom == measure::liquid_surface_rate &&
        div   == measure::time )
        return measure::liquid_surface_volume;

    if( denom == measure::gas_surface_rate &&
        div   == measure::time )
        return measure::gas_surface_volume;

    if( denom == measure::mass_rate &&
        div   == measure::time )
        return measure::mass;

    if( denom == measure::mass_rate &&
        div   == measure::liquid_surface_rate )
        return measure::polymer_density;

    if( denom == measure::energy_rate &&
        div   == measure::time )
        return measure::energy;

    return measure::identity;
}

measure mul_unit( measure lhs, measure rhs ) {
    if( lhs == rhs ) return lhs;

    if( ( lhs == measure::liquid_surface_rate && rhs == measure::time ) ||
        ( rhs == measure::liquid_surface_rate && lhs == measure::time ) )
        return measure::liquid_surface_volume;

    if( ( lhs == measure::gas_surface_rate && rhs == measure::time ) ||
        ( rhs == measure::gas_surface_rate && lhs == measure::time ) )
        return measure::gas_surface_volume;

    if( ( lhs == measure::rate && rhs == measure::time ) ||
        ( rhs == measure::rate && lhs == measure::time ) )
        return measure::volume;

    if(  lhs == measure::mass_rate && rhs == measure::time)
        return measure::mass;

    if(  lhs == measure::energy_rate && rhs == measure::time)
        return measure::energy;

    return lhs;
}

struct quantity {
    double value;
    Ewoms::UnitSystem::measure unit;

    quantity operator+( const quantity& rhs ) const {
        assert( this->unit == rhs.unit );
        return { this->value + rhs.value, this->unit };
    }

    quantity operator*( const quantity& rhs ) const {
        return { this->value * rhs.value, mul_unit( this->unit, rhs.unit ) };
    }

    quantity operator/( const quantity& rhs ) const {
        const auto res_unit = div_unit( this->unit, rhs.unit );

        if( rhs.value == 0 ) return { 0.0, res_unit };
        return { this->value / rhs.value, res_unit };
    }

    quantity operator/( double divisor ) const {
        if( divisor == 0 ) return { 0.0, this->unit };
        return { this->value / divisor , this->unit };
    }

    quantity& operator/=( double divisor ) {
        if( divisor == 0 )
            this->value = 0;
        else
            this->value /= divisor;

        return *this;
    }

    quantity operator-( const quantity& rhs) const {
        return { this->value - rhs.value, this->unit };
    }
};

/*
 * All functions must have the same parameters, so they're gathered in a struct
 * and functions use whatever information they care about.
 *
 * schedule_wells are wells from the deck, provided by ewoms-eclio. active_index
 * is the index of the block in question. wells is simulation data.
 */
struct fn_args {
    const std::vector<Ewoms::Well>& schedule_wells;
    const std::string group_name;
    double duration;
    const int sim_step;
    int  num;
    const Ewoms::optional<Ewoms::variant<std::string, int>> extra_data;
    const Ewoms::SummaryState& st;
    const Ewoms::data::Wells& wells;
    const Ewoms::data::GroupAndNetworkValues& grp_nwrk;
    const Ewoms::out::RegionCache& regionCache;
    const Ewoms::EclipseGrid& grid;
    const std::vector< std::pair< std::string, double > > eff_factors;
    const Ewoms::Inplace& initial_inplace;
    const Ewoms::Inplace& inplace;
    const Ewoms::UnitSystem& unit_system;
};

/* Since there are several enums in eWoms scattered about more-or-less
 * representing the same thing. Since functions use template parameters to
 * expand into the actual implementations we need a static way to determine
 * what unit to tag the return value with.
 */
template< rt > constexpr
measure rate_unit() { return measure::liquid_surface_rate; }
template< Ewoms::Phase > constexpr
measure rate_unit() { return measure::liquid_surface_rate; }

template <Ewoms::data::GuideRateValue::Item>
measure rate_unit() { return measure::liquid_surface_rate; }

template<> constexpr
measure rate_unit< rt::gas >() { return measure::gas_surface_rate; }
template<> constexpr
measure rate_unit< Ewoms::Phase::GAS >() { return measure::gas_surface_rate; }

template<> constexpr
measure rate_unit< rt::solvent >() { return measure::gas_surface_rate; }

template<> constexpr
measure rate_unit< rt::reservoir_water >() { return measure::rate; }

template<> constexpr
measure rate_unit< rt::reservoir_oil >() { return measure::rate; }

template<> constexpr
measure rate_unit< rt::reservoir_gas >() { return measure::rate; }

template<> constexpr
measure rate_unit < rt::productivity_index_water > () { return measure::liquid_productivity_index; }

template<> constexpr
measure rate_unit < rt::productivity_index_oil > () { return measure::liquid_productivity_index; }

template<> constexpr
measure rate_unit < rt::productivity_index_gas > () { return measure::gas_productivity_index; }

template<> constexpr
measure rate_unit< rt::well_potential_water >() { return measure::liquid_surface_rate; }

template<> constexpr
measure rate_unit< rt::well_potential_oil >() { return measure::liquid_surface_rate; }

template<> constexpr
measure rate_unit< rt::well_potential_gas >() { return measure::gas_surface_rate; }

template <> constexpr
measure rate_unit<Ewoms::data::GuideRateValue::Item::Gas>() { return measure::gas_surface_rate; }

template <> constexpr
measure rate_unit<Ewoms::data::GuideRateValue::Item::ResV>() { return measure::rate; }

double efac( const std::vector<std::pair<std::string,double>>& eff_factors, const std::string& name ) {
    auto it = std::find_if( eff_factors.begin(), eff_factors.end(),
                            [&] ( const std::pair< std::string, double > elem )
                            { return elem.first == name; }
                          );

    return (it != eff_factors.end()) ? it->second : 1;
}

/*
  This is bit dangerous, exactly how the ALQ value should be interpreted varies
  between the different VFP tables. The code here assumes - without checking -
  that it represents gas lift rate.
*/
inline quantity glir( const fn_args& args ) {
    double alq_rate = 0;

    for (const auto& well : args.schedule_wells) {
        if (well.isInjector())
            continue;

        auto xwPos = args.wells.find(well.name());
        if (xwPos == args.wells.end())
            continue;

        double eff_fac = efac( args.eff_factors, well.name() );
        alq_rate += eff_fac*xwPos->second.rates.get(rt::alq, well.alq_value());
    }
    return { alq_rate, measure::gas_surface_rate };
}

inline quantity wwirt( const fn_args& args ) {
    const quantity zero = { 0, rate_unit< Ewoms::Phase::WATER >() };
    const auto& well = args.schedule_wells.front();
    const auto& wtype = well.wellType();

    if (wtype.producer())
        return zero;

    if (wtype.injector_type() != Ewoms::InjectorType::WATER)
        return zero;

    const auto& injection = well.injectionControls(args.st);
    return { injection.surface_rate, rate_unit< Ewoms::Phase::WATER >() };
}

template< rt phase, bool injection = true >
inline quantity rate( const fn_args& args ) {
    double sum = 0.0;

    for( const auto& sched_well : args.schedule_wells ) {
        const auto& name = sched_well.name();
        if( args.wells.count( name ) == 0 ) continue;

        double eff_fac = efac( args.eff_factors, name );

        const auto v = args.wells.at(name).rates.get(phase, 0.0) * eff_fac;

        if( ( v > 0 ) == injection )
            sum += v;
    }

    if( !injection ) sum *= -1;

    if (phase == rt::polymer || phase == rt::brine) return { sum, measure::mass_rate };
    return { sum, rate_unit< phase >() };
}

template< rt phase, bool injection = true >
inline quantity ratel( const fn_args& args ) {
    const auto unit = ((phase == rt::polymer) || (phase == rt::brine))
        ? measure::mass_rate : rate_unit<phase>();

    const quantity zero = { 0.0, unit };

    if (args.schedule_wells.empty())
        return zero;

    const auto& well = args.schedule_wells.front();
    const auto& name = well.name();
    if( args.wells.count( name ) == 0 ) return zero;
    const auto& well_data = args.wells.at( name );
    if (well_data.current_control.isProducer == injection) return zero;

    double sum = 0;
    const auto& connections = well.getConnections( args.num );
    for (const auto& conn_ptr : connections) {
        const size_t global_index = conn_ptr->global_index();
        const auto& conn_data = std::find_if(well_data.connections.begin(),
                                             well_data.connections.end(),
                                             [global_index] (const Ewoms::data::Connection cdata)
                                             {
                                                 return cdata.index == global_index;
                                             });

        if (conn_data != well_data.connections.end()) {
            double eff_fac = efac( args.eff_factors, name );
            sum += conn_data->rates.get( phase, 0.0 ) * eff_fac;
        }
    }
    if( !injection ) sum *= -1;

    return { sum, unit };
}

template< rt phase, bool injection = true >
inline quantity cratel( const fn_args& args ) {
    const auto unit = ((phase == rt::polymer) || (phase == rt::brine))
        ? measure::mass_rate : rate_unit<phase>();

    const quantity zero = { 0.0, unit };

    if (args.schedule_wells.empty())
        return zero;

    const auto& well = args.schedule_wells.front();
    const auto& name = well.name();
    if( args.wells.count( name ) == 0 ) return zero;
    const auto& well_data = args.wells.at( name );
    if (well_data.current_control.isProducer == injection) return zero;

    const auto complnum = getCompletionNumberFromGlobalConnectionIndex(well.getConnections(), args.num - 1);
    if (!static_cast<bool>(complnum))
        // Connection might not yet have come online.
        return zero;

    double sum = 0;
    const auto& connections = well.getConnections(*complnum);
    for (const auto& conn_ptr : connections) {
        const size_t global_index = conn_ptr->global_index();
        const auto& conn_data = std::find_if(well_data.connections.begin(),
                                             well_data.connections.end(),
                                             [global_index] (const Ewoms::data::Connection cdata)
                                             {
                                                 return cdata.index == global_index;
                                             });
        if (conn_data != well_data.connections.end()) {
            double eff_fac = efac( args.eff_factors, name );
            sum += conn_data->rates.get( phase, 0.0 ) * eff_fac;
        }
    }
    if( !injection ) sum *= -1;

    return { sum, unit };
}

template< bool injection >
inline quantity flowing( const fn_args& args ) {
    const auto& wells = args.wells;
    auto pred = [&wells]( const Ewoms::Well& w ) {
        const auto& name = w.name();
        return w.isInjector( ) == injection
            && wells.count( name ) > 0
            && wells.at( name ).flowing();
    };

    return { double( std::count_if( args.schedule_wells.begin(),
                                    args.schedule_wells.end(),
                                    pred ) ),
             measure::identity };
}

template< rt phase, bool injection = true>
inline quantity crate( const fn_args& args ) {
    const quantity zero = { 0, rate_unit< phase >() };
    // The args.num value is the literal value which will go to the
    // NUMS array in the eclipse SMSPEC file; the values in this array
    // are offset 1 - whereas we need to use this index here to look
    // up a completion with offset 0.
    const size_t global_index = args.num - 1;
    if( args.schedule_wells.empty() ) return zero;

    const auto& well = args.schedule_wells.front();
    const auto& name = well.name();
    if( args.wells.count( name ) == 0 ) return zero;

    const auto& well_data = args.wells.at( name );
    const auto& completion = std::find_if( well_data.connections.begin(),
                                           well_data.connections.end(),
                                           [=]( const Ewoms::data::Connection& c ) {
                                                return c.index == global_index;
                                           } );
    if (well_data.current_control.isProducer == injection) return zero;
    if( completion == well_data.connections.end() ) return zero;

    double eff_fac = efac( args.eff_factors, name );
    auto v = completion->rates.get( phase, 0.0 ) * eff_fac;
    if (!injection)
        v *= -1;

    if( phase == rt::polymer || phase == rt::brine ) return { v, measure::mass_rate };
    return { v, rate_unit< phase >() };
}

template< rt phase>
inline quantity srate( const fn_args& args ) {
    const quantity zero = { 0, rate_unit< phase >() };
    // The args.num value is the literal value which will go to the
    // NUMS array in the eclispe SMSPEC file; the values in this array
    // are offset 1 - whereas we need to use this index here to look
    // up a completion with offset 0.
    const size_t segNumber = args.num;
    if( args.schedule_wells.empty() ) return zero;

    const auto& well = args.schedule_wells.front();
    const auto& name = well.name();
    if( args.wells.count( name ) == 0 ) return zero;

    const auto& well_data = args.wells.at( name );

    const auto& segment = well_data.segments.find(segNumber);

    if( segment == well_data.segments.end() ) return zero;

    double eff_fac = efac( args.eff_factors, name );
    auto v = segment->second.rates.get( phase, 0.0 ) * eff_fac;
    //switch sign of rate - opposite convention in flow vs eclipse
    v *= -1;

    if( phase == rt::polymer || phase == rt::brine ) return { v, measure::mass_rate };
    return { v, rate_unit< phase >() };
}

inline quantity trans_factors ( const fn_args& args ) {
    const quantity zero = { 0.0, measure::transmissibility };

    if (args.schedule_wells.empty())
        // No wells.  Before simulation starts?
        return zero;

    auto xwPos = args.wells.find(args.schedule_wells.front().name());
    if (xwPos == args.wells.end())
        // No dynamic results for this well.  Not open?
        return zero;

    // Like completion rate we need to look up a connection with offset 0.
    const size_t global_index = args.num - 1;
    const auto& connections = xwPos->second.connections;
    auto connPos = std::find_if(connections.begin(), connections.end(),
        [global_index](const Ewoms::data::Connection& c)
    {
        return c.index == global_index;
    });

    if (connPos == connections.end())
        // No dynamic results for this connection.
        return zero;

    // Dynamic connection result's "trans_factor" includes PI-adjustment.
    return { connPos->trans_factor, measure::transmissibility };
}

template <Ewoms::data::SegmentPressures::Value ix>
inline quantity segpress ( const fn_args& args ) {
    const quantity zero = { 0, measure::pressure };

    if( args.schedule_wells.empty() ) return zero;
    // Like completion rate we need to look
    // up a connection with offset 0.
    const size_t segNumber = args.num;
    if( args.schedule_wells.empty() ) return zero;

    const auto& well = args.schedule_wells.front();
    const auto& name = well.name();
    if( args.wells.count( name ) == 0 ) return zero;

    const auto& well_data = args.wells.at( name );

    const auto& segment = well_data.segments.find(segNumber);

    if( segment == well_data.segments.end() ) return zero;

    return { segment->second.pressures[ix], measure::pressure };
}

inline quantity bhp( const fn_args& args ) {
    const quantity zero = { 0, measure::pressure };
    if( args.schedule_wells.empty() ) return zero;

    const auto p = args.wells.find( args.schedule_wells.front().name() );
    if( p == args.wells.end() ) return zero;

    return { p->second.bhp, measure::pressure };
}

/*
  This function is slightly ugly - the evaluation of ROEW uses the already
  calculated COPT results. We do not really have any formalism for such
  dependencies between the summary vectors. For this particualar case there is a
  hack in SummaryConfig which should ensure that this is safe.
*/

quantity roew(const fn_args& args) {
    const quantity zero = { 0, measure::identity };
    const auto& region_name = Ewoms::get<std::string>(*args.extra_data);
    if (!args.initial_inplace.has( region_name, Ewoms::Inplace::Phase::OIL, args.num))
        return zero;

    double oil_prod = 0;
    for (const auto& cPair : args.regionCache.connections(region_name, args.num)) {
        const auto& well = cPair.first;
        const auto& global_index = cPair.second;
        const auto copt_key = fmt::format("COPT:{}:{}" , well, global_index + 1);
        if (args.st.has(copt_key))
            oil_prod += args.st.get(copt_key);
    }
    oil_prod = args.unit_system.to_si(Ewoms::UnitSystem::measure::volume, oil_prod);
    return { oil_prod / args.initial_inplace.get( region_name, Ewoms::Inplace::Phase::OIL, args.num ) , measure::identity };
}

inline quantity temperature( const fn_args& args ) {
    const quantity zero = { 0, measure::temperature };
    if( args.schedule_wells.empty() ) return zero;

    const auto p = args.wells.find( args.schedule_wells.front().name() );
    if( p == args.wells.end() ) return zero;

    return { p->second.temperature, measure::temperature };
}

inline quantity thp( const fn_args& args ) {
    const quantity zero = { 0, measure::pressure };
    if( args.schedule_wells.empty() ) return zero;

    const auto p = args.wells.find( args.schedule_wells.front().name() );
    if( p == args.wells.end() ) return zero;

    return { p->second.thp, measure::pressure };
}

inline quantity bhp_history( const fn_args& args ) {
    if( args.schedule_wells.empty() ) return { 0.0, measure::pressure };

    const Ewoms::Well& sched_well = args.schedule_wells.front();

    double bhp_hist;
    if ( sched_well.isProducer(  ) )
        bhp_hist = sched_well.getProductionProperties().BHPH;
    else
        bhp_hist = sched_well.getInjectionProperties().BHPH;

    return { bhp_hist, measure::pressure };
}

inline quantity thp_history( const fn_args& args ) {
    if( args.schedule_wells.empty() ) return { 0.0, measure::pressure };

    const Ewoms::Well& sched_well = args.schedule_wells.front();

    double thp_hist;
    if ( sched_well.isProducer() )
       thp_hist = sched_well.getProductionProperties().THPH;
    else
       thp_hist = sched_well.getInjectionProperties().THPH;

    return { thp_hist, measure::pressure };
}

inline quantity node_pressure(const fn_args& args)
{
    auto nodePos = args.grp_nwrk.nodeData.find(args.group_name);
    if (nodePos == args.grp_nwrk.nodeData.end()) {
        return { 0.0, measure::pressure };
    }

    return { nodePos->second.pressure, measure::pressure };
}

template< Ewoms::Phase phase >
inline quantity production_history( const fn_args& args ) {
    /*
     * For well data, looking up historical rates (both for production and
     * injection) before simulation actually starts is impossible and
     * nonsensical. We therefore default to writing zero (which is what eclipse
     * seems to do as well).
     */

    double sum = 0.0;
    for( const auto& sched_well : args.schedule_wells ){

        double eff_fac = efac( args.eff_factors, sched_well.name() );
        sum += sched_well.production_rate( args.st, phase ) * eff_fac;
    }

    return { sum, rate_unit< phase >() };
}

template< Ewoms::Phase phase >
inline quantity injection_history( const fn_args& args ) {

    double sum = 0.0;
    for( const auto& sched_well : args.schedule_wells ){
        double eff_fac = efac( args.eff_factors, sched_well.name() );
        sum += sched_well.injection_rate( args.st, phase ) * eff_fac;
    }

    return { sum, rate_unit< phase >() };
}

template< bool injection >
inline quantity abondoned_well( const fn_args& args ) {
    std::size_t count = 0;

    for (const auto& sched_well : args.schedule_wells) {
        if (injection && !sched_well.hasInjected())
            continue;

        if (!injection && !sched_well.hasProduced())
            continue;

        const auto& well_name = sched_well.name();
        auto well_iter = args.wells.find( well_name );
        if (well_iter == args.wells.end()) {
            count += 1;
            continue;
        }

        count += !well_iter->second.flowing();
    }

    return { 1.0 * count, measure::identity };
}

inline quantity res_vol_production_target( const fn_args& args ) {

    double sum = 0.0;
    for( const Ewoms::Well& sched_well : args.schedule_wells )
        if (sched_well.getProductionProperties().predictionMode)
            sum += sched_well.getProductionProperties().ResVRate.getSI();

    return { sum, measure::rate };
}

inline quantity duration( const fn_args& args ) {
    return { args.duration, measure::time };
}

template<rt phase , bool injection>
quantity region_rate( const fn_args& args ) {
    double sum = 0;
    const auto& well_connections = args.regionCache.connections( Ewoms::get<std::string>(*args.extra_data), args.num );

    for (const auto& pair : well_connections) {

        double eff_fac = efac( args.eff_factors, pair.first );

        double rate = args.wells.get( pair.first , pair.second , phase ) * eff_fac;

        // We are asking for the production rate in an injector - or
        // opposite. We just clamp to zero.
        if ((rate > 0) != injection)
            rate = 0;

        sum += rate;
    }

    if( injection )
        return { sum, rate_unit< phase >() };
    else
        return { -sum, rate_unit< phase >() };
}

quantity rhpv(const fn_args& args) {
    const auto& inplace = args.inplace;
    const auto& region_name = Ewoms::get<std::string>(*args.extra_data);
    if (inplace.has( region_name, Ewoms::Inplace::Phase::HydroCarbonPV, args.num ))
        return { inplace.get( region_name, Ewoms::Inplace::Phase::HydroCarbonPV, args.num ), measure::volume };
    else
        return {0, measure::volume};
}

template < rt phase, bool outputProducer = true, bool outputInjector = true>
inline quantity potential_rate( const fn_args& args ) {
    double sum = 0.0;

    for( const auto& sched_well : args.schedule_wells ) {
        const auto& name = sched_well.name();
        if( args.wells.count( name ) == 0 ) continue;

        if (sched_well.isInjector() && outputInjector) {
	    const auto v = args.wells.at(name).rates.get(phase, 0.0);
	    sum += v * efac(args.eff_factors, name);
	}
	else if (sched_well.isProducer() && outputProducer) {
	    const auto v = args.wells.at(name).rates.get(phase, 0.0);
	    sum += v * efac(args.eff_factors, name);
	}
    }

    return { sum, rate_unit< phase >() };
}

inline quantity preferred_phase_productivty_index(const fn_args& args) {
    if (args.schedule_wells.empty())
        return {0, rate_unit<rt::productivity_index_oil>()};

    const auto& well = args.schedule_wells.front();
    auto preferred_phase = well.getPreferredPhase();
    if (well.getStatus() == Ewoms::Well::Status::OPEN) {

        switch (preferred_phase) {
        case Ewoms::Phase::OIL:
            return potential_rate<rt::productivity_index_oil>(args);

        case Ewoms::Phase::GAS:
            return potential_rate<rt::productivity_index_gas>(args);

        case Ewoms::Phase::WATER:
            return potential_rate<rt::productivity_index_water>(args);

        default:
            break;
        }
    } else {

        switch (preferred_phase) {
        case Ewoms::Phase::OIL:
            return {0, rate_unit<rt::productivity_index_oil>()};

        case Ewoms::Phase::GAS:
            return {0, rate_unit<rt::productivity_index_gas>()};

        case Ewoms::Phase::WATER:
            return {0, rate_unit<rt::productivity_index_water>()};

        default:
            break;

        }
    }

    throw std::invalid_argument {
        "Unsupported \"preferred\" phase: " +
            std::to_string(static_cast<int>(args.schedule_wells.front().getPreferredPhase()))
            };
}

inline quantity connection_productivity_index(const fn_args& args) {
    const quantity zero = { 0.0, rate_unit<rt::productivity_index_oil>() };

    if (args.schedule_wells.empty())
        return zero;

    auto xwPos = args.wells.find(args.schedule_wells.front().name());
    if (xwPos == args.wells.end())
        return zero;

    // The args.num value is the literal value which will go to the
    // NUMS array in the eclipse SMSPEC file; the values in this array
    // are offset 1 - whereas we need to use this index here to look
    // up a completion with offset 0.
    const auto global_index = static_cast<std::size_t>(args.num) - 1;

    const auto& xcon = xwPos->second.connections;
    const auto& completion =
        std::find_if(xcon.begin(), xcon.end(),
            [global_index](const Ewoms::data::Connection& c)
        {
            return c.index == global_index;
        });

    if (completion == xcon.end())
        return zero;

    switch (args.schedule_wells.front().getPreferredPhase()) {
    case Ewoms::Phase::OIL:
        return { completion->rates.get(rt::productivity_index_oil, 0.0),
                 rate_unit<rt::productivity_index_oil>() };

    case Ewoms::Phase::GAS:
        return { completion->rates.get(rt::productivity_index_gas, 0.0),
                 rate_unit<rt::productivity_index_gas>() };

    case Ewoms::Phase::WATER:
        return { completion->rates.get(rt::productivity_index_water, 0.0),
                 rate_unit<rt::productivity_index_water>() };

    default:
        break;
    }

    throw std::invalid_argument {
        "Unsupported \"preferred\" phase: " +
        std::to_string(static_cast<int>(args.schedule_wells.front().getPreferredPhase()))
    };
}

template < bool isGroup, bool Producer, bool waterInjector, bool gasInjector>
inline quantity group_control( const fn_args& args ) {

    std::string g_name = "";
    if (isGroup) {
        const quantity zero = { static_cast<double>(0), Ewoms::UnitSystem::measure::identity};
        if( args.group_name.empty() ) return zero;

        g_name = args.group_name;
    }
    else {
        g_name = "FIELD";
    }

    int cntl_mode = 0;

    // production control
    if (Producer) {
        auto it_g = args.grp_nwrk.groupData.find(g_name);
        if (it_g != args.grp_nwrk.groupData.end()) {
            const auto& value = it_g->second.currentControl.currentProdConstraint;
            auto it_c = pCModeToPCntlMode.find(value);
            if (it_c == pCModeToPCntlMode.end()) {
                std::stringstream str;
                str << "unknown control CMode: " << static_cast<int>(value);
                throw std::invalid_argument(str.str());
            }
            cntl_mode = it_c->second;
        }
    }
    // water injection control
    else if (waterInjector){
        auto it_g = args.grp_nwrk.groupData.find(g_name);
        if (it_g != args.grp_nwrk.groupData.end()) {
            const auto& value = it_g->second.currentControl.currentWaterInjectionConstraint;
            auto it_c = iCModeToICntlMode.find(value);
            if (it_c == iCModeToICntlMode.end()) {
                std::stringstream str;
                str << "unknown control CMode: " << static_cast<int>(value);
                throw std::invalid_argument(str.str());
            }
            cntl_mode = it_c->second;
        }
    }

    // gas injection control
    else if (gasInjector){
        auto it_g = args.grp_nwrk.groupData.find(g_name);
        if (it_g != args.grp_nwrk.groupData.end()) {
            const auto& value = it_g->second.currentControl.currentGasInjectionConstraint;
            auto it_c = iCModeToICntlMode.find(value);
            if (it_c == iCModeToICntlMode.end()) {
                std::stringstream str;
                str << "unknown control CMode: " << static_cast<int>(value);
                throw std::invalid_argument(str.str());
            }
            cntl_mode = it_c->second;
        }
    }

    return {static_cast<double>(cntl_mode), Ewoms::UnitSystem::measure::identity};
}

namespace {
    bool well_control_mode_defined(const ::Ewoms::data::Well& xw)
    {
        using PMode = ::Ewoms::Well::ProducerCMode;
        using IMode = ::Ewoms::Well::InjectorCMode;

        const auto& curr = xw.current_control;

        return (curr.isProducer && (curr.prod != PMode::CMODE_UNDEFINED))
            || (!curr.isProducer && (curr.inj != IMode::CMODE_UNDEFINED));
    }
}

inline quantity well_control_mode( const fn_args& args ) {
    const auto unit = Ewoms::UnitSystem::measure::identity;

    if (args.schedule_wells.empty()) {
        // No wells.  Possibly determining pertinent unit of measure
        // during SMSPEC configuration.
        return { 0.0, unit };
    }

    const auto& well = args.schedule_wells.front();
    auto xwPos = args.wells.find(well.name());
    if (xwPos == args.wells.end()) {
        // No dynamic results for 'well'.  Treat as shut/stopped.
        return { 0.0, unit };
    }

    if (! well_control_mode_defined(xwPos->second)) {
        // No dynamic control mode defined.  Use input control.
        const auto wmctl = ::Ewoms::eclipseControlMode(well, args.st);

        return { static_cast<double>(wmctl), unit };
    }

    // Well has simulator-provided active control mode.  Pick the
    // appropriate value depending on well type (producer/injector).
    const auto& curr = xwPos->second.current_control;
    const auto wmctl = curr.isProducer
        ? ::Ewoms::eclipseControlMode(curr.prod)
        : ::Ewoms::eclipseControlMode(curr.inj, well.injectorType());

    return { static_cast<double>(wmctl), unit };
}

template <Ewoms::data::GuideRateValue::Item i>
quantity guiderate_value(const ::Ewoms::data::GuideRateValue& grvalue)
{
    return { !grvalue.has(i) ? 0.0 : grvalue.get(i), rate_unit<i>() };
}

template <bool injection, Ewoms::data::GuideRateValue::Item i>
quantity group_guiderate(const fn_args& args)
{
    auto xgPos = args.grp_nwrk.groupData.find(args.group_name);
    if (xgPos == args.grp_nwrk.groupData.end()) {
        return { 0.0, rate_unit<i>() };
    }

    return injection
        ? guiderate_value<i>(xgPos->second.guideRates.injection)
        : guiderate_value<i>(xgPos->second.guideRates.production);
}

template <bool injection, Ewoms::data::GuideRateValue::Item i>
quantity well_guiderate(const fn_args& args)
{
    if (args.schedule_wells.empty()) {
        return { 0.0, rate_unit<i>() };
    }

    const auto& well = args.schedule_wells.front();
    if (well.isInjector() != injection) {
        return { 0.0, rate_unit<i>() };
    }

    auto xwPos = args.wells.find(well.name());
    if (xwPos == args.wells.end()) {
        return { 0.0, rate_unit<i>() };
    }

    return guiderate_value<i>(xwPos->second.guide_rates);
}

/*
 * A small DSL, really poor man's function composition, to avoid massive
 * repetition when declaring the handlers for each individual keyword. bin_op
 * and its aliases will apply the pair of functions F and G that all take const
 * fn_args& and return quantity, making them composable.
 */
template< typename F, typename G, typename Op >
struct bin_op {
    bin_op( F fn, G gn = {} ) : f( fn ), g( gn ) {}
    quantity operator()( const fn_args& args ) const {
        return Op()( f( args ), g( args ) );
    }

    private:
        F f;
        G g;
};

template< typename F, typename G >
auto mul( F f, G g ) -> bin_op< F, G, std::multiplies< quantity > >
{ return { f, g }; }

template< typename F, typename G >
auto sum( F f, G g ) -> bin_op< F, G, std::plus< quantity > >
{ return { f, g }; }

template< typename F, typename G >
auto div( F f, G g ) -> bin_op< F, G, std::divides< quantity > >
{ return { f, g }; }

template< typename F, typename G >
auto sub( F f, G g ) -> bin_op< F, G, std::minus< quantity > >
{ return { f, g }; }

using ofun = std::function< quantity( const fn_args& ) >;

static const std::unordered_map< std::string, ofun > funs = {
    { "WWIR", rate< rt::wat, injector > },
    { "WWIRT", wwirt },
    { "WOIR", rate< rt::oil, injector > },
    { "WGIR", rate< rt::gas, injector > },
    { "WEIR", rate< rt::energy, injector > },
    { "WTIRHEA", rate< rt::energy, injector > },
    { "WNIR", rate< rt::solvent, injector > },
    { "WCIR", rate< rt::polymer, injector > },
    { "WSIR", rate< rt::brine, injector > },
    { "WVIR", sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                       rate< rt::reservoir_gas, injector > ) },
    { "WGIGR", well_guiderate<injector, Ewoms::data::GuideRateValue::Item::Gas> },
    { "WWIGR", well_guiderate<injector, Ewoms::data::GuideRateValue::Item::Water> },

    { "WWIT", mul( rate< rt::wat, injector >, duration ) },
    { "WOIT", mul( rate< rt::oil, injector >, duration ) },
    { "WGIT", mul( rate< rt::gas, injector >, duration ) },
    { "WEIT", mul( rate< rt::energy, injector >, duration ) },
    { "WTITHEA", mul( rate< rt::energy, injector >, duration ) },
    { "WNIT", mul( rate< rt::solvent, injector >, duration ) },
    { "WCIT", mul( rate< rt::polymer, injector >, duration ) },
    { "WSIT", mul( rate< rt::brine, injector >, duration ) },
    { "WVIT", mul( sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                        rate< rt::reservoir_gas, injector > ), duration ) },

    { "WWPR", rate< rt::wat, producer > },
    { "WOPR", rate< rt::oil, producer > },
    { "WWPTL",mul(ratel< rt::wat, producer >, duration) },
    { "WGPTL",mul(ratel< rt::gas, producer >, duration) },
    { "WOPTL",mul(ratel< rt::oil, producer >, duration) },
    { "WWPRL",ratel< rt::wat, producer > },
    { "WGPRL",ratel< rt::gas, producer > },
    { "WOPRL",ratel< rt::oil, producer > },
    { "WOFRL",ratel< rt::oil, producer > },
    { "WWIRL",ratel< rt::wat, injector> },
    { "WWITL",mul(ratel< rt::wat, injector>, duration) },
    { "WGIRL",ratel< rt::gas, injector> },
    { "WGITL",mul(ratel< rt::gas, injector>, duration) },
    { "WLPTL",mul( sum(ratel<rt::wat, producer>, ratel<rt::oil, producer>), duration)},
    { "WWCTL", div( ratel< rt::wat, producer >,
                    sum( ratel< rt::wat, producer >, ratel< rt::oil, producer > ) ) },
    { "WGORL", div( ratel< rt::gas, producer >, ratel< rt::oil, producer > ) },
    { "WGPR", rate< rt::gas, producer > },
    { "WEPR", rate< rt::energy, producer > },
    { "WTPRHEA", rate< rt::energy, producer > },
    { "WGLIR", glir},
    { "WNPR", rate< rt::solvent, producer > },
    { "WCPR", rate< rt::polymer, producer > },
    { "WSPR", rate< rt::brine, producer > },
    { "WCPC", div( rate< rt::polymer, producer >, rate< rt::wat, producer >) },
    { "WSPC", div( rate< rt::brine, producer >, rate< rt::wat, producer >) },

    { "WOPGR", well_guiderate<producer, Ewoms::data::GuideRateValue::Item::Oil> },
    { "WGPGR", well_guiderate<producer, Ewoms::data::GuideRateValue::Item::Gas> },
    { "WWPGR", well_guiderate<producer, Ewoms::data::GuideRateValue::Item::Water> },
    { "WVPGR", well_guiderate<producer, Ewoms::data::GuideRateValue::Item::ResV> },

    { "WGPRS", rate< rt::dissolved_gas, producer > },
    { "WGPRF", sub( rate< rt::gas, producer >, rate< rt::dissolved_gas, producer > ) },
    { "WOPRS", rate< rt::vaporized_oil, producer > },
    { "WOPRF", sub (rate < rt::oil, producer >, rate< rt::vaporized_oil, producer > )  },
    { "WVPR", sum( sum( rate< rt::reservoir_water, producer >, rate< rt::reservoir_oil, producer > ),
                   rate< rt::reservoir_gas, producer > ) },
    { "WGVPR", rate< rt::reservoir_gas, producer > },

    { "WLPR", sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) },
    { "WWPT", mul( rate< rt::wat, producer >, duration ) },
    { "WOPT", mul( rate< rt::oil, producer >, duration ) },
    { "WGPT", mul( rate< rt::gas, producer >, duration ) },
    { "WEPT", mul( rate< rt::energy, producer >, duration ) },
    { "WTPTHEA", mul( rate< rt::energy, producer >, duration ) },
    { "WNPT", mul( rate< rt::solvent, producer >, duration ) },
    { "WCPT", mul( rate< rt::polymer, producer >, duration ) },
    { "WSPT", mul( rate< rt::brine, producer >, duration ) },
    { "WLPT", mul( sum( rate< rt::wat, producer >, rate< rt::oil, producer > ),
                   duration ) },
    { "WGPTS", mul( rate< rt::dissolved_gas, producer >, duration )},
    { "WGPTF", sub( mul( rate< rt::gas, producer >, duration ),
                        mul( rate< rt::dissolved_gas, producer >, duration ))},
    { "WOPTS", mul( rate< rt::vaporized_oil, producer >, duration )},
    { "WOPTF", sub( mul( rate< rt::oil, producer >, duration ),
                        mul( rate< rt::vaporized_oil, producer >, duration ))},
    { "WVPT", mul( sum( sum( rate< rt::reservoir_water, producer >, rate< rt::reservoir_oil, producer > ),
                        rate< rt::reservoir_gas, producer > ), duration ) },

    { "WWCT", div( rate< rt::wat, producer >,
                   sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) ) },
    { "GWCT", div( rate< rt::wat, producer >,
                   sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) ) },
    { "WGOR", div( rate< rt::gas, producer >, rate< rt::oil, producer > ) },
    { "GGOR", div( rate< rt::gas, producer >, rate< rt::oil, producer > ) },
    { "WGLR", div( rate< rt::gas, producer >,
                   sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) ) },

    { "WBHP", bhp },
    { "WTHP", thp },
    { "WTPCHEA", temperature},
    { "WTICHEA", temperature},
    { "WVPRT", res_vol_production_target },

    { "WMCTL", well_control_mode },

    { "GWIR", rate< rt::wat, injector > },
    { "WGVIR", rate< rt::reservoir_gas, injector >},
    { "WWVIR", rate< rt::reservoir_water, injector >},
    { "GOIR", rate< rt::oil, injector > },
    { "GGIR", rate< rt::gas, injector > },
    { "GEIR", rate< rt::energy, injector > },
    { "GTIRHEA", rate< rt::energy, injector > },
    { "GNIR", rate< rt::solvent, injector > },
    { "GCIR", rate< rt::polymer, injector > },
    { "GSIR", rate< rt::brine, injector > },
    { "GVIR", sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                        rate< rt::reservoir_gas, injector > ) },

    { "GGIGR", group_guiderate<injector, Ewoms::data::GuideRateValue::Item::Gas> },
    { "GWIGR", group_guiderate<injector, Ewoms::data::GuideRateValue::Item::Water> },

    { "GWIT", mul( rate< rt::wat, injector >, duration ) },
    { "GOIT", mul( rate< rt::oil, injector >, duration ) },
    { "GGIT", mul( rate< rt::gas, injector >, duration ) },
    { "GEIT", mul( rate< rt::energy, injector >, duration ) },
    { "GTITHEA", mul( rate< rt::energy, injector >, duration ) },
    { "GNIT", mul( rate< rt::solvent, injector >, duration ) },
    { "GCIT", mul( rate< rt::polymer, injector >, duration ) },
    { "GSIT", mul( rate< rt::brine, injector >, duration ) },
    { "GVIT", mul( sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                        rate< rt::reservoir_gas, injector > ), duration ) },

    { "GWPR", rate< rt::wat, producer > },
    { "GOPR", rate< rt::oil, producer > },
    { "GGPR", rate< rt::gas, producer > },
    { "GEPR", rate< rt::energy, producer > },
    { "GTPRHEA", rate< rt::energy, producer > },
    { "GGLIR", glir },
    { "GNPR", rate< rt::solvent, producer > },
    { "GCPR", rate< rt::polymer, producer > },
    { "GSPR", rate< rt::brine, producer > },
    { "GCPC", div( rate< rt::polymer, producer >, rate< rt::wat, producer >) },
    { "GSPC", div( rate< rt::brine, producer >, rate< rt::wat, producer >) },
    { "GOPRS", rate< rt::vaporized_oil, producer > },
    { "GOPRF", sub (rate < rt::oil, producer >, rate< rt::vaporized_oil, producer > ) },
    { "GLPR", sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) },
    { "GVPR", sum( sum( rate< rt::reservoir_water, producer >, rate< rt::reservoir_oil, producer > ),
                        rate< rt::reservoir_gas, producer > ) },

    { "GOPGR", group_guiderate<producer, Ewoms::data::GuideRateValue::Item::Oil> },
    { "GGPGR", group_guiderate<producer, Ewoms::data::GuideRateValue::Item::Gas> },
    { "GWPGR", group_guiderate<producer, Ewoms::data::GuideRateValue::Item::Water> },
    { "GVPGR", group_guiderate<producer, Ewoms::data::GuideRateValue::Item::ResV> },

    { "GPR", node_pressure },

    { "GWPT", mul( rate< rt::wat, producer >, duration ) },
    { "GOPT", mul( rate< rt::oil, producer >, duration ) },
    { "GGPT", mul( rate< rt::gas, producer >, duration ) },
    { "GEPT", mul( rate< rt::energy, producer >, duration ) },
    { "GTPTHEA", mul( rate< rt::energy, producer >, duration ) },
    { "GNPT", mul( rate< rt::solvent, producer >, duration ) },
    { "GCPT", mul( rate< rt::polymer, producer >, duration ) },
    { "GOPTS", mul( rate< rt::vaporized_oil, producer >, duration ) },
    { "GOPTF", mul( sub (rate < rt::oil, producer >,
                         rate< rt::vaporized_oil, producer > ),
                    duration ) },
    { "GLPT", mul( sum( rate< rt::wat, producer >, rate< rt::oil, producer > ),
                   duration ) },
    { "GVPT", mul( sum( sum( rate< rt::reservoir_water, producer >, rate< rt::reservoir_oil, producer > ),
                        rate< rt::reservoir_gas, producer > ), duration ) },
    // Group potential
    { "GWPP", potential_rate< rt::well_potential_water , true, false>},
    { "GOPP", potential_rate< rt::well_potential_oil , true, false>},
    { "GGPP", potential_rate< rt::well_potential_gas , true, false>},
    { "GWPI", potential_rate< rt::well_potential_water , false, true>},
    { "GOPI", potential_rate< rt::well_potential_oil , false, true>},
    { "GGPI", potential_rate< rt::well_potential_gas , false, true>},

    //Group control mode
    { "GMCTP", group_control< true, true,  false, false >},
    { "GMCTW", group_control< true, false, true,  false >},
    { "GMCTG", group_control< true, false, false, true  >},

    { "WWPRH", production_history< Ewoms::Phase::WATER > },
    { "WOPRH", production_history< Ewoms::Phase::OIL > },
    { "WGPRH", production_history< Ewoms::Phase::GAS > },
    { "WLPRH", sum( production_history< Ewoms::Phase::WATER >,
                    production_history< Ewoms::Phase::OIL > ) },

    { "WWPTH", mul( production_history< Ewoms::Phase::WATER >, duration ) },
    { "WOPTH", mul( production_history< Ewoms::Phase::OIL >, duration ) },
    { "WGPTH", mul( production_history< Ewoms::Phase::GAS >, duration ) },
    { "WLPTH", mul( sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ),
                    duration ) },

    { "WWIRH", injection_history< Ewoms::Phase::WATER > },
    { "WOIRH", injection_history< Ewoms::Phase::OIL > },
    { "WGIRH", injection_history< Ewoms::Phase::GAS > },
    { "WWITH", mul( injection_history< Ewoms::Phase::WATER >, duration ) },
    { "WOITH", mul( injection_history< Ewoms::Phase::OIL >, duration ) },
    { "WGITH", mul( injection_history< Ewoms::Phase::GAS >, duration ) },

    /* From our point of view, injectors don't have water cuts and div/sum will return 0.0 */
    { "WWCTH", div( production_history< Ewoms::Phase::WATER >,
                    sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ) ) },

    /* We do not support mixed injections, and gas/oil is undefined when oil is
     * zero (i.e. pure gas injector), so always output 0 if this is an injector
     */
    { "WGORH", div( production_history< Ewoms::Phase::GAS >,
                    production_history< Ewoms::Phase::OIL > ) },
    { "WGLRH", div( production_history< Ewoms::Phase::GAS >,
                    sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ) ) },

    { "WTHPH", thp_history },
    { "WBHPH", bhp_history },

    { "GWPRH", production_history< Ewoms::Phase::WATER > },
    { "GOPRH", production_history< Ewoms::Phase::OIL > },
    { "GGPRH", production_history< Ewoms::Phase::GAS > },
    { "GLPRH", sum( production_history< Ewoms::Phase::WATER >,
                    production_history< Ewoms::Phase::OIL > ) },
    { "GWIRH", injection_history< Ewoms::Phase::WATER > },
    { "GOIRH", injection_history< Ewoms::Phase::OIL > },
    { "GGIRH", injection_history< Ewoms::Phase::GAS > },
    { "GGORH", div( production_history< Ewoms::Phase::GAS >,
                    production_history< Ewoms::Phase::OIL > ) },
    { "GWCTH", div( production_history< Ewoms::Phase::WATER >,
                    sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ) ) },

    { "GWPTH", mul( production_history< Ewoms::Phase::WATER >, duration ) },
    { "GOPTH", mul( production_history< Ewoms::Phase::OIL >, duration ) },
    { "GGPTH", mul( production_history< Ewoms::Phase::GAS >, duration ) },
    { "GGPRF", sub( rate < rt::gas, producer >, rate< rt::dissolved_gas, producer > )},
    { "GGPRS", rate< rt::dissolved_gas, producer> },
    { "GGPTF", mul( sub( rate < rt::gas, producer >, rate< rt::dissolved_gas, producer > ),
                         duration ) },
    { "GGPTS", mul( rate< rt::dissolved_gas, producer>, duration ) },
    { "GGLR",  div( rate< rt::gas, producer >,
                    sum( rate< rt::wat, producer >,
                         rate< rt::oil, producer > ) ) },
    { "GGLRH", div( production_history< Ewoms::Phase::GAS >,
                    sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ) ) },
    { "GLPTH", mul( sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ),
                    duration ) },
    { "GWITH", mul( injection_history< Ewoms::Phase::WATER >, duration ) },
    { "GGITH", mul( injection_history< Ewoms::Phase::GAS >, duration ) },
    { "GMWIN", flowing< injector > },
    { "GMWPR", flowing< producer > },

    { "GVPRT", res_vol_production_target },

    { "CGIRL", cratel< rt::gas, injector> },
    { "CGITL", mul( cratel< rt::gas, injector>, duration) },
    { "CWIRL", cratel< rt::wat, injector> },
    { "CWITL", mul( cratel< rt::wat, injector>, duration) },
    { "CWPRL", cratel< rt::wat, producer > },
    { "CWPTL", mul( cratel< rt::wat, producer >, duration) },
    { "COPRL", cratel< rt::oil, producer > },
    { "COPTL", mul( cratel< rt::oil, producer >, duration) },
    { "CGPRL", cratel< rt::gas, producer > },
    { "CGPTL", mul( cratel< rt::gas, producer >, duration) },
    { "COFRL", cratel< rt::oil, producer > },
    { "CGORL", div( cratel< rt::gas, producer >, cratel< rt::oil, producer > ) },
    { "CWCTL", div( cratel< rt::wat, producer >,
                    sum( cratel< rt::wat, producer >, cratel< rt::oil, producer > ) ) },
    { "CWIR", crate< rt::wat, injector > },
    { "CGIR", crate< rt::gas, injector > },
    { "CCIR", crate< rt::polymer, injector > },
    { "CSIR", crate< rt::brine, injector > },
    { "CWIT", mul( crate< rt::wat, injector >, duration ) },
    { "CGIT", mul( crate< rt::gas, injector >, duration ) },
    { "CNIT", mul( crate< rt::solvent, injector >, duration ) },

    { "CWPR", crate< rt::wat, producer > },
    { "COPR", crate< rt::oil, producer > },
    { "CGPR", crate< rt::gas, producer > },
    { "CCPR", crate< rt::polymer, producer > },
    { "CSPR", crate< rt::brine, producer > },
    { "CGFR", sub(crate<rt::gas, producer>, crate<rt::gas, injector>) },
    { "COFR", sub(crate<rt::oil, producer>, crate<rt::oil, injector>) },
    { "CWFR", sub(crate<rt::wat, producer>, crate<rt::wat, injector>) },
    { "CWCT", div( crate< rt::wat, producer >,
                   sum( crate< rt::wat, producer >, crate< rt::oil, producer > ) ) },
    { "CGOR", div( crate< rt::gas, producer >, crate< rt::oil, producer > ) },
    // Minus for injection rates and pluss for production rate
    { "CNFR", sub( crate< rt::solvent, producer >, crate<rt::solvent, injector >) },
    { "CWPT", mul( crate< rt::wat, producer >, duration ) },
    { "COPT", mul( crate< rt::oil, producer >, duration ) },
    { "CGPT", mul( crate< rt::gas, producer >, duration ) },
    { "CNPT", mul( crate< rt::solvent, producer >, duration ) },
    { "CCIT", mul( crate< rt::polymer, injector >, duration ) },
    { "CCPT", mul( crate< rt::polymer, producer >, duration ) },
    { "CSIT", mul( crate< rt::brine, injector >, duration ) },
    { "CSPT", mul( crate< rt::brine, producer >, duration ) },
    { "CTFAC", trans_factors },
    { "CPI", connection_productivity_index },

    { "FWPR", rate< rt::wat, producer > },
    { "FOPR", rate< rt::oil, producer > },
    { "FGPR", rate< rt::gas, producer > },
    { "FEPR", rate< rt::energy, producer > },
    { "FTPRHEA", rate< rt::energy, producer > },
    { "FGLIR", glir },
    { "FNPR", rate< rt::solvent, producer > },
    { "FCPR", rate< rt::polymer, producer > },
    { "FSPR", rate< rt::brine, producer > },
    { "FCPC", div( rate< rt::polymer, producer >, rate< rt::wat, producer >) },
    { "FSPC", div( rate< rt::brine, producer >, rate< rt::wat, producer >) },
    { "FVPR", sum( sum( rate< rt::reservoir_water, producer>, rate< rt::reservoir_oil, producer >),
                   rate< rt::reservoir_gas, producer>)},
    { "FGPRS", rate< rt::dissolved_gas, producer > },
    { "FGPRF", sub( rate< rt::gas, producer >, rate< rt::dissolved_gas, producer > ) },
    { "FOPRS", rate< rt::vaporized_oil, producer > },
    { "FOPRF", sub (rate < rt::oil, producer >, rate< rt::vaporized_oil, producer > ) },

    { "FLPR", sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) },
    { "FWPT", mul( rate< rt::wat, producer >, duration ) },
    { "FOPT", mul( rate< rt::oil, producer >, duration ) },
    { "FGPT", mul( rate< rt::gas, producer >, duration ) },
    { "FEPT", mul( rate< rt::energy, producer >, duration ) },
    { "FTPTHEA", mul( rate< rt::energy, producer >, duration ) },
    { "FNPT", mul( rate< rt::solvent, producer >, duration ) },
    { "FCPT", mul( rate< rt::polymer, producer >, duration ) },
    { "FSPT", mul( rate< rt::brine, producer >, duration ) },
    { "FLPT", mul( sum( rate< rt::wat, producer >, rate< rt::oil, producer > ),
                   duration ) },
    { "FVPT", mul(sum (sum( rate< rt::reservoir_water, producer>, rate< rt::reservoir_oil, producer >),
                       rate< rt::reservoir_gas, producer>), duration)},
    { "FGPTS", mul( rate< rt::dissolved_gas, producer > , duration )},
    { "FGPTF", mul( sub( rate< rt::gas, producer >, rate< rt::dissolved_gas, producer > ), duration )},
    { "FOPTS", mul( rate< rt::vaporized_oil, producer >, duration ) },
    { "FOPTF", mul( sub (rate < rt::oil, producer >,
                         rate< rt::vaporized_oil, producer > ),
                    duration ) },

    { "FWIR", rate< rt::wat, injector > },
    { "FOIR", rate< rt::oil, injector > },
    { "FGIR", rate< rt::gas, injector > },
    { "FEIR", rate< rt::energy, injector > },
    { "FTIRHEA", rate< rt::energy, injector > },
    { "FNIR", rate< rt::solvent, injector > },
    { "FCIR", rate< rt::polymer, injector > },
    { "FCPR", rate< rt::polymer, producer > },
    { "FSIR", rate< rt::brine, injector > },
    { "FSPR", rate< rt::brine, producer > },
    { "FVIR", sum( sum( rate< rt::reservoir_water, injector>, rate< rt::reservoir_oil, injector >),
                   rate< rt::reservoir_gas, injector>)},

    { "FLIR", sum( rate< rt::wat, injector >, rate< rt::oil, injector > ) },
    { "FWIT", mul( rate< rt::wat, injector >, duration ) },
    { "FOIT", mul( rate< rt::oil, injector >, duration ) },
    { "FGIT", mul( rate< rt::gas, injector >, duration ) },
    { "FEIT", mul( rate< rt::energy, injector >, duration ) },
    { "FTITHEA", mul( rate< rt::energy, injector >, duration ) },
    { "FNIT", mul( rate< rt::solvent, injector >, duration ) },
    { "FCIT", mul( rate< rt::polymer, injector >, duration ) },
    { "FCPT", mul( rate< rt::polymer, producer >, duration ) },
    { "FSIT", mul( rate< rt::brine, injector >, duration ) },
    { "FSPT", mul( rate< rt::brine, producer >, duration ) },
    { "FLIT", mul( sum( rate< rt::wat, injector >, rate< rt::oil, injector > ),
                   duration ) },
    { "FVIT", mul( sum( sum( rate< rt::reservoir_water, injector>, rate< rt::reservoir_oil, injector >),
                   rate< rt::reservoir_gas, injector>), duration)},
    // Field potential
    { "FWPP", potential_rate< rt::well_potential_water , true, false>},
    { "FOPP", potential_rate< rt::well_potential_oil , true, false>},
    { "FGPP", potential_rate< rt::well_potential_gas , true, false>},
    { "FWPI", potential_rate< rt::well_potential_water , false, true>},
    { "FOPI", potential_rate< rt::well_potential_oil , false, true>},
    { "FGPI", potential_rate< rt::well_potential_gas , false, true>},

    { "FWPRH", production_history< Ewoms::Phase::WATER > },
    { "FOPRH", production_history< Ewoms::Phase::OIL > },
    { "FGPRH", production_history< Ewoms::Phase::GAS > },
    { "FLPRH", sum( production_history< Ewoms::Phase::WATER >,
                    production_history< Ewoms::Phase::OIL > ) },
    { "FWPTH", mul( production_history< Ewoms::Phase::WATER >, duration ) },
    { "FOPTH", mul( production_history< Ewoms::Phase::OIL >, duration ) },
    { "FGPTH", mul( production_history< Ewoms::Phase::GAS >, duration ) },
    { "FLPTH", mul( sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ),
                    duration ) },

    { "FWIRH", injection_history< Ewoms::Phase::WATER > },
    { "FOIRH", injection_history< Ewoms::Phase::OIL > },
    { "FGIRH", injection_history< Ewoms::Phase::GAS > },
    { "FWITH", mul( injection_history< Ewoms::Phase::WATER >, duration ) },
    { "FOITH", mul( injection_history< Ewoms::Phase::OIL >, duration ) },
    { "FGITH", mul( injection_history< Ewoms::Phase::GAS >, duration ) },

    { "FWCT", div( rate< rt::wat, producer >,
                   sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) ) },
    { "FGOR", div( rate< rt::gas, producer >, rate< rt::oil, producer > ) },
    { "FGLR", div( rate< rt::gas, producer >,
                   sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) ) },
    { "FWCTH", div( production_history< Ewoms::Phase::WATER >,
                    sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ) ) },
    { "FGORH", div( production_history< Ewoms::Phase::GAS >,
                    production_history< Ewoms::Phase::OIL > ) },
    { "FGLRH", div( production_history< Ewoms::Phase::GAS >,
                    sum( production_history< Ewoms::Phase::WATER >,
                         production_history< Ewoms::Phase::OIL > ) ) },
    { "FMWIN", flowing< injector > },
    { "FMWPR", flowing< producer > },
    { "FVPRT", res_vol_production_target },
    { "FMWPA", abondoned_well< producer > },
    { "FMWIA", abondoned_well< injector >},

    //Field control mode
    { "FMCTP", group_control< false, true,  false, false >},
    { "FMCTW", group_control< false, false, true,  false >},
    { "FMCTG", group_control< false, false, false, true  >},

    /* Region properties */
    { "ROIR"  , region_rate< rt::oil, injector > },
    { "RGIR"  , region_rate< rt::gas, injector > },
    { "RWIR"  , region_rate< rt::wat, injector > },
    { "ROPR"  , region_rate< rt::oil, producer > },
    { "RGPR"  , region_rate< rt::gas, producer > },
    { "RWPR"  , region_rate< rt::wat, producer > },
    { "ROIT"  , mul( region_rate< rt::oil, injector >, duration ) },
    { "RGIT"  , mul( region_rate< rt::gas, injector >, duration ) },
    { "RWIT"  , mul( region_rate< rt::wat, injector >, duration ) },
    { "ROPT"  , mul( region_rate< rt::oil, producer >, duration ) },
    { "RGPT"  , mul( region_rate< rt::gas, producer >, duration ) },
    { "RWPT"  , mul( region_rate< rt::wat, producer >, duration ) },
    { "RHPV"  , rhpv },
    //Multisegment well segment data
    { "SOFR", srate< rt::oil > },
    { "SWFR", srate< rt::wat > },
    { "SGFR", srate< rt::gas > },
    { "SPR", segpress<Ewoms::data::SegmentPressures::Value::Pressure> },
    { "SPRD", segpress<Ewoms::data::SegmentPressures::Value::PDrop> },
    { "SPRDH", segpress<Ewoms::data::SegmentPressures::Value::PDropHydrostatic> },
    { "SPRDF", segpress<Ewoms::data::SegmentPressures::Value::PDropFriction> },
    { "SPRDA", segpress<Ewoms::data::SegmentPressures::Value::PDropAccel> },
    // Well productivity index
    { "WPI", preferred_phase_productivty_index },
    { "WPIW", potential_rate< rt::productivity_index_water >},
    { "WPIO", potential_rate< rt::productivity_index_oil >},
    { "WPIG", potential_rate< rt::productivity_index_gas >},
    { "WPIL", sum( potential_rate< rt::productivity_index_water, true, false >,
                   potential_rate< rt::productivity_index_oil, true, false >)},
    // Well potential
    { "WWPP", potential_rate< rt::well_potential_water , true, false>},
    { "WOPP", potential_rate< rt::well_potential_oil , true, false>},
    { "WGPP", potential_rate< rt::well_potential_gas , true, false>},
    { "WWPI", potential_rate< rt::well_potential_water , false, true>},
    { "WWIP", potential_rate< rt::well_potential_water , false, true>}, // Alias for 'WWPI'
    { "WOPI", potential_rate< rt::well_potential_oil , false, true>},
    { "WGPI", potential_rate< rt::well_potential_gas , false, true>},
    { "WGIP", potential_rate< rt::well_potential_gas , false, true>}, // Alias for 'WGPI'
    { "ROEW", roew },
};

static const std::unordered_map< std::string, Ewoms::UnitSystem::measure> single_values_units = {
  {"TCPU"     , Ewoms::UnitSystem::measure::identity },
  {"ELAPSED"  , Ewoms::UnitSystem::measure::identity },
  {"NEWTON"   , Ewoms::UnitSystem::measure::identity },
  {"NLINERS"  , Ewoms::UnitSystem::measure::identity },
  {"NLINSMIN" , Ewoms::UnitSystem::measure::identity },
  {"NLINSMAX" , Ewoms::UnitSystem::measure::identity },
  {"MLINEARS" , Ewoms::UnitSystem::measure::identity },
  {"MSUMLINS" , Ewoms::UnitSystem::measure::identity },
  {"MSUMNEWT" , Ewoms::UnitSystem::measure::identity },
  {"TCPUTS"   , Ewoms::UnitSystem::measure::identity },
  {"TIMESTEP" , Ewoms::UnitSystem::measure::time },
  {"TCPUDAY"  , Ewoms::UnitSystem::measure::time },
  {"STEPTYPE" , Ewoms::UnitSystem::measure::identity },
  {"TELAPLIN" , Ewoms::UnitSystem::measure::time },
  {"FWIP"     , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"FOIP"     , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"FGIP"     , Ewoms::UnitSystem::measure::gas_surface_volume },
  {"FOIPL"    , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"FOIPG"    , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"FGIPL"    , Ewoms::UnitSystem::measure::gas_surface_volume },
  {"FGIPG"    , Ewoms::UnitSystem::measure::gas_surface_volume },
  {"FPR"      , Ewoms::UnitSystem::measure::pressure },

};

static const std::unordered_map< std::string, Ewoms::UnitSystem::measure> region_units = {
  {"RPR"      , Ewoms::UnitSystem::measure::pressure},
  {"ROIP"     , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"ROIPL"    , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"ROIPG"    , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"RGIP"     , Ewoms::UnitSystem::measure::gas_surface_volume },
  {"RGIPL"    , Ewoms::UnitSystem::measure::gas_surface_volume },
  {"RGIPG"    , Ewoms::UnitSystem::measure::gas_surface_volume },
  {"RWIP"     , Ewoms::UnitSystem::measure::liquid_surface_volume },
  {"RRPV"     , Ewoms::UnitSystem::measure::geometric_volume }
};

static const std::unordered_map< std::string, Ewoms::UnitSystem::measure> block_units = {
  {"BPR"        , Ewoms::UnitSystem::measure::pressure},
  {"BPRESSUR"   , Ewoms::UnitSystem::measure::pressure},
  {"BSWAT"      , Ewoms::UnitSystem::measure::identity},
  {"BWSAT"      , Ewoms::UnitSystem::measure::identity},
  {"BSGAS"      , Ewoms::UnitSystem::measure::identity},
  {"BGSAT"      , Ewoms::UnitSystem::measure::identity},
  {"BOSAT"      , Ewoms::UnitSystem::measure::identity},
  {"BWKR"      , Ewoms::UnitSystem::measure::identity},
  {"BOKR"      , Ewoms::UnitSystem::measure::identity},
  {"BKRO"      , Ewoms::UnitSystem::measure::identity},
  {"BKROG"     , Ewoms::UnitSystem::measure::identity},
  {"BKROW"     , Ewoms::UnitSystem::measure::identity},
  {"BGKR"      , Ewoms::UnitSystem::measure::identity},
  {"BKRG"      , Ewoms::UnitSystem::measure::identity},
  {"BKRW"      , Ewoms::UnitSystem::measure::identity},
  {"BWPC"      , Ewoms::UnitSystem::measure::pressure},
  {"BGPC"      , Ewoms::UnitSystem::measure::pressure},
  {"BVWAT"      , Ewoms::UnitSystem::measure::viscosity},
  {"BWVIS"      , Ewoms::UnitSystem::measure::viscosity},
  {"BVGAS"      , Ewoms::UnitSystem::measure::viscosity},
  {"BGVIS"      , Ewoms::UnitSystem::measure::viscosity},
  {"BVOIL"      , Ewoms::UnitSystem::measure::viscosity},
  {"BOVIS"      , Ewoms::UnitSystem::measure::viscosity},
};

static const std::unordered_map< std::string, Ewoms::UnitSystem::measure> aquifer_units = {
    {"AAQT", Ewoms::UnitSystem::measure::liquid_surface_volume},
    {"AAQR", Ewoms::UnitSystem::measure::liquid_surface_rate},
    {"AAQP", Ewoms::UnitSystem::measure::pressure},
};

inline std::vector<Ewoms::Well> find_wells( const Ewoms::Schedule& schedule,
                                           const Ewoms::EclIO::SummaryNode& node,
                                           const int sim_step,
                                           const Ewoms::out::RegionCache& regionCache ) {
    const auto cat = node.category;

    switch (cat) {
    case Ewoms::EclIO::SummaryNode::Category::Well: [[fallthrough]];
    case Ewoms::EclIO::SummaryNode::Category::Connection: [[fallthrough]];
    case Ewoms::EclIO::SummaryNode::Category::Segment: {
        const auto& name = node.wgname;

        if (schedule.hasWell(node.wgname, sim_step)) {
            return { schedule.getWell( name, sim_step ) };
        } else {
            return {};
        }
    }

    case Ewoms::EclIO::SummaryNode::Category::Group: {
        const auto& name = node.wgname;

        if( !schedule.hasGroup( name ) ) return {};

        return schedule.getChildWells2( name, sim_step);
    }

    case Ewoms::EclIO::SummaryNode::Category::Field:
        return schedule.getWells(sim_step);

    case Ewoms::EclIO::SummaryNode::Category::Region: {
        std::vector<Ewoms::Well> wells;

        const auto region = node.number;

        for ( const auto& connection : regionCache.connections( *node.fip_region , region ) ){
            const auto& w_name = connection.first;
            if (schedule.hasWell(w_name, sim_step)) {
                const auto& well = schedule.getWell( w_name, sim_step );

                const auto& it = std::find_if( wells.begin(), wells.end(),
                                               [&] ( const Ewoms::Well& elem )
                                               { return elem.name() == well.name(); });
                if ( it == wells.end() )
                    wells.push_back( well );
            }
        }

        return wells;
    }

    case Ewoms::EclIO::SummaryNode::Category::Aquifer:       [[fallthrough]];
    case Ewoms::EclIO::SummaryNode::Category::Block:         [[fallthrough]];
    case Ewoms::EclIO::SummaryNode::Category::Node:          [[fallthrough]];
    case Ewoms::EclIO::SummaryNode::Category::Miscellaneous:
        return {};
    }

    throw std::runtime_error("Unhandled summary node category in find_wells");
}

bool need_wells(const Ewoms::EclIO::SummaryNode& node)
{
    static const std::regex region_keyword_regex { "R[OGW][IP][RT]" };
    static const std::regex group_guiderate_regex { "G[OGWV][IP]GR" };

    using Cat = Ewoms::EclIO::SummaryNode::Category;

    switch (node.category) {
    case Cat::Connection: [[fallthrough]];
    case Cat::Field:      [[fallthrough]];
    case Cat::Group:      [[fallthrough]];
    case Cat::Segment:    [[fallthrough]];
    case Cat::Well:
        // Need to capture wells for anything other than guiderates at group
        // level.  Those are directly available in the solution values from
        // the simulator and don't need aggregation from well level.
        return (node.category != Cat::Group)
            || !std::regex_match(node.keyword, group_guiderate_regex);

    case Cat::Region:
        return std::regex_match(node.keyword, region_keyword_regex);

    case Cat::Aquifer:       [[fallthrough]];
    case Cat::Miscellaneous: [[fallthrough]];
    case Cat::Node:          [[fallthrough]];
        // Node values directly available in solution.
    case Cat::Block:
        return false;
    }

    throw std::runtime_error("Unhandled summary node category in need_wells");
}

void updateValue(const Ewoms::EclIO::SummaryNode& node, const double value, Ewoms::SummaryState& st)
{
    if (node.category == Ewoms::EclIO::SummaryNode::Category::Well)
        st.update_well_var(node.wgname, node.keyword, value);

    else if (node.category == Ewoms::EclIO::SummaryNode::Category::Group)
        st.update_group_var(node.wgname, node.keyword, value);

    else
        st.update(node.unique_key(), value);
}

/*
 * The well efficiency factor will not impact the well rate itself, but is
 * rather applied for accumulated values.The WEFAC can be considered to shut
 * and open the well for short intervals within the same timestep, and the well
 * is therefore solved at full speed.
 *
 * Groups are treated similarly as wells. The group's GEFAC is not applied for
 * rates, only for accumulated volumes. When GEFAC is set for a group, it is
 * considered that all wells are taken down simultaneously, and GEFAC is
 * therefore not applied to the group's rate. However, any efficiency factors
 * applied to the group's wells or sub-groups must be included.
 *
 * Regions and fields will have the well and group efficiency applied for both
 * rates and accumulated values.
 *
 */
struct EfficiencyFactor
{
    using Factor  = std::pair<std::string, double>;
    using FacColl = std::vector<Factor>;

    FacColl factors{};

    void setFactors(const Ewoms::EclIO::SummaryNode& node,
                    const Ewoms::Schedule&           schedule,
                    const std::vector<Ewoms::Well>& schedule_wells,
                    const int                      sim_step);
};

void EfficiencyFactor::setFactors(const Ewoms::EclIO::SummaryNode& node,
                                  const Ewoms::Schedule&           schedule,
                                  const std::vector<Ewoms::Well>&  schedule_wells,
                                  const int                      sim_step)
{
    this->factors.clear();

    const bool is_field  { node.category == Ewoms::EclIO::SummaryNode::Category::Field  } ;
    const bool is_group  { node.category == Ewoms::EclIO::SummaryNode::Category::Group  } ;
    const bool is_region { node.category == Ewoms::EclIO::SummaryNode::Category::Region } ;
    const bool is_rate   { node.type     != Ewoms::EclIO::SummaryNode::Type::Total      } ;

    if (!is_field && !is_group && !is_region && is_rate)
        return;

    for( const auto& well : schedule_wells ) {
        if (!well.hasBeenDefined(sim_step))
            continue;

        double eff_factor = well.getEfficiencyFactor();
        const auto* group_ptr = std::addressof(schedule.getGroup(well.groupName(), sim_step));

        while (group_ptr) {
            if (is_group && is_rate && group_ptr->name() == node.wgname )
                break;

            eff_factor *= group_ptr->getGroupEfficiencyFactor();

            const auto parent_group = group_ptr->flow_group();

            if (parent_group)
                group_ptr = std::addressof(schedule.getGroup( parent_group.value(), sim_step ));
            else
                group_ptr = nullptr;
        }

        this->factors.emplace_back( well.name(), eff_factor );
    }
}

namespace Evaluator {
    struct InputData
    {
        const Ewoms::EclipseState& es;
        const Ewoms::Schedule& sched;
        const Ewoms::EclipseGrid& grid;
        const Ewoms::out::RegionCache& reg;
        const Ewoms::Inplace initial_inplace;
    };

    struct SimulatorResults
    {
        const Ewoms::data::WellRates& wellSol;
        const Ewoms::data::GroupAndNetworkValues& grpNwrkSol;
        const std::map<std::string, double>& single;
        const Ewoms::Inplace inplace;
        const std::map<std::string, std::vector<double>>& region;
        const std::map<std::pair<std::string, int>, double>& block;
        const Ewoms::data::Aquifers& aquifers;
    };

    class Base
    {
    public:
        virtual ~Base() {}

        virtual void update(const std::size_t       sim_step,
                            const double            stepSize,
                            const InputData&        input,
                            const SimulatorResults& simRes,
                            Ewoms::SummaryState&      st) const = 0;
    };

    class FunctionRelation : public Base
    {
    public:
        explicit FunctionRelation(Ewoms::EclIO::SummaryNode node, ofun fcn)
            : node_(std::move(node))
            , fcn_ (std::move(fcn))
        {}

        void update(const std::size_t       sim_step,
                    const double            stepSize,
                    const InputData&        input,
                    const SimulatorResults& simRes,
                    Ewoms::SummaryState&      st) const override
        {
            const auto get_wells = need_wells(this->node_);

            const auto wells = get_wells
                ? find_wells(input.sched, this->node_,
                             static_cast<int>(sim_step), input.reg)
                : std::vector<Ewoms::Well>{};

            if (get_wells && wells.empty())
                // Parameter depends on well information, but no active
                // wells apply at this sim_step.  Nothing to do.
                return;

            EfficiencyFactor efac{};
            efac.setFactors(this->node_, input.sched, wells, sim_step);

            using Foo = Ewoms::variant<std::string,int>;
            using Bar = Ewoms::optional<Foo>;
            const fn_args args {
                wells, this->group_name(), stepSize, static_cast<int>(sim_step),
                std::max(0, this->node_.number),
                static_cast<bool>(this->node_.fip_region)
                ?   Bar(Foo(*this->node_.fip_region))
                :   Ewoms::nullopt,
                st, simRes.wellSol, simRes.grpNwrkSol, input.reg, input.grid,
                std::move(efac.factors), input.initial_inplace, simRes.inplace, input.sched.getUnits()
            };

            const auto& usys = input.es.getUnits();
            const auto  prm  = this->fcn_(args);

            updateValue(this->node_, usys.from_si(prm.unit, prm.value), st);
        }

    private:
        Ewoms::EclIO::SummaryNode node_;
        ofun                    fcn_;

        std::string group_name() const
        {
            using Cat = ::Ewoms::EclIO::SummaryNode::Category;

            const auto need_grp_name =
                (this->node_.category == Cat::Group) ||
                (this->node_.category == Cat::Node);

            return need_grp_name
                ? this->node_.wgname : std::string{""};
        }
    };

    class BlockValue : public Base
    {
    public:
        explicit BlockValue(Ewoms::EclIO::SummaryNode node,
                            const Ewoms::UnitSystem::measure m)
            : node_(std::move(node))
            , m_   (m)
        {}

        void update(const std::size_t    /* sim_step */,
                    const double         /* stepSize */,
                    const InputData&        input,
                    const SimulatorResults& simRes,
                    Ewoms::SummaryState&      st) const override
        {
            auto xPos = simRes.block.find(this->lookupKey());
            if (xPos == simRes.block.end()) {
                return;
            }

            const auto& usys = input.es.getUnits();
            updateValue(this->node_, usys.from_si(this->m_, xPos->second), st);
        }

    private:
        Ewoms::EclIO::SummaryNode  node_;
        Ewoms::UnitSystem::measure m_;

        Ewoms::out::Summary::BlockValues::key_type lookupKey() const
        {
            return { this->node_.keyword, this->node_.number };
        }
    };

    class AquiferValue: public Base
    {
    public:
        explicit AquiferValue(Ewoms::EclIO::SummaryNode node,
                              const Ewoms::UnitSystem::measure m)
        : node_(std::move(node))
        , m_   (m)
        {}

        void update(const std::size_t    /* sim_step */,
                    const double         /* stepSize */,
                    const InputData&        input,
                    const SimulatorResults& simRes,
                    Ewoms::SummaryState&      st) const override
        {
            auto xPos = simRes.aquifers.find(this->node_.number);
            if (xPos == simRes.aquifers.end()) {
                return;
            }

            const auto& usys = input.es.getUnits();
            updateValue(this->node_, usys.from_si(this->m_, xPos->second.get(this->node_.keyword)), st);
        }
    private:
        Ewoms::EclIO::SummaryNode  node_;
        Ewoms::UnitSystem::measure m_;
    };

    class RegionValue : public Base
    {
    public:
        explicit RegionValue(Ewoms::EclIO::SummaryNode node,
                             const Ewoms::UnitSystem::measure m)
            : node_(std::move(node))
            , m_   (m)
        {}

        void update(const std::size_t    /* sim_step */,
                    const double         /* stepSize */,
                    const InputData&        input,
                    const SimulatorResults& simRes,
                    Ewoms::SummaryState&      st) const override
        {
            if (this->node_.number < 0)
                return;

            auto xPos = simRes.region.find(this->node_.keyword);
            if (xPos == simRes.region.end())
                return;

            const auto ix = this->index();
            if (ix >= xPos->second.size())
                return;

            const auto  val  = xPos->second[ix];
            const auto& usys = input.es.getUnits();

            updateValue(this->node_, usys.from_si(this->m_, val), st);
        }

    private:
        Ewoms::EclIO::SummaryNode  node_;
        Ewoms::UnitSystem::measure m_;

        std::vector<double>::size_type index() const
        {
            return this->node_.number - 1;
        }
    };

    class GlobalProcessValue : public Base
    {
    public:
        explicit GlobalProcessValue(Ewoms::EclIO::SummaryNode node,
                                    const Ewoms::UnitSystem::measure m)
            : node_(std::move(node))
            , m_   (m)
        {}

        void update(const std::size_t    /* sim_step */,
                    const double         /* stepSize */,
                    const InputData&        input,
                    const SimulatorResults& simRes,
                    Ewoms::SummaryState&      st) const override
        {
            auto xPos = simRes.single.find(this->node_.keyword);
            if (xPos == simRes.single.end())
                return;

            const auto  val  = xPos->second;
            const auto& usys = input.es.getUnits();

            updateValue(this->node_, usys.from_si(this->m_, val), st);
        }

    private:
        Ewoms::EclIO::SummaryNode  node_;
        Ewoms::UnitSystem::measure m_;
    };

    class UserDefinedValue : public Base
    {
    public:
        void update(const std::size_t       /* sim_step */,
                    const double            /* stepSize */,
                    const InputData&        /* input */,
                    const SimulatorResults& /* simRes */,
                    Ewoms::SummaryState&      /* st */) const override
        {
            // No-op
        }
    };

    class Time : public Base
    {
    public:
        explicit Time(std::string saveKey)
            : saveKey_(std::move(saveKey))
        {}

        void update(const std::size_t       /* sim_step */,
                    const double               stepSize,
                    const InputData&           input,
                    const SimulatorResults& /* simRes */,
                    Ewoms::SummaryState&         st) const override
        {
            const auto& usys = input.es.getUnits();

            const auto m   = ::Ewoms::UnitSystem::measure::time;
            const auto val = st.get_elapsed() + stepSize;

            st.update(this->saveKey_, usys.from_si(m, val));
            st.update("TIME", usys.from_si(m, val));
        }

    private:
        std::string saveKey_;
    };

    class Day : public Base
    {
    public:
        explicit Day(std::string saveKey)
            : saveKey_(std::move(saveKey))
        {}

        void update(const std::size_t       /* sim_step */,
                    const double               stepSize,
                    const InputData&           input,
                    const SimulatorResults& /* simRes */,
                    Ewoms::SummaryState&         st) const override
        {
            auto sim_time = make_sim_time(input.sched, st, stepSize);
            st.update(this->saveKey_, sim_time.day());
        }

    private:
        std::string saveKey_;
    };

    class Month : public Base
    {
    public:
        explicit Month(std::string saveKey)
            : saveKey_(std::move(saveKey))
        {}

        void update(const std::size_t       /* sim_step */,
                    const double               stepSize,
                    const InputData&           input,
                    const SimulatorResults& /* simRes */,
                    Ewoms::SummaryState&         st) const override
        {
            auto sim_time = make_sim_time(input.sched, st, stepSize);
            st.update(this->saveKey_, sim_time.month());
        }

    private:
        std::string saveKey_;
    };

    class Year : public Base
    {
    public:
        explicit Year(std::string saveKey)
            : saveKey_(std::move(saveKey))
        {}

        void update(const std::size_t       /* sim_step */,
                    const double               stepSize,
                    const InputData&           input,
                    const SimulatorResults& /* simRes */,
                    Ewoms::SummaryState&         st) const override
        {
            auto sim_time = make_sim_time(input.sched, st, stepSize);
            st.update(this->saveKey_, sim_time.year());
        }

    private:
        std::string saveKey_;
    };

    class Years : public Base
    {
    public:
        explicit Years(std::string saveKey)
            : saveKey_(std::move(saveKey))
        {}

        void update(const std::size_t       /* sim_step */,
                    const double               stepSize,
                    const InputData&        /* input */,
                    const SimulatorResults& /* simRes */,
                    Ewoms::SummaryState&         st) const override
        {
            using namespace ::Ewoms::unit;

            const auto val = st.get_elapsed() + stepSize;

            st.update(this->saveKey_, convert::to(val, ecl_year));
        }

    private:
        std::string saveKey_;
    };

    class Factory
    {
    public:
        struct Descriptor
        {
            std::string uniquekey{};
            std::string unit{};
            std::unique_ptr<Base> evaluator{};
        };

        explicit Factory(const Ewoms::EclipseState& es,
                         const Ewoms::EclipseGrid&  grid,
                         const Ewoms::SummaryState& st,
                         const Ewoms::UDQConfig&    udq)
            : es_(es), grid_(grid), st_(st), udq_(udq)
        {}

        ~Factory() = default;

        Factory(const Factory&) = delete;
        Factory(Factory&&) = delete;
        Factory& operator=(const Factory&) = delete;
        Factory& operator=(Factory&&) = delete;

        Descriptor create(const Ewoms::EclIO::SummaryNode&);

    private:
        const Ewoms::EclipseState& es_;
        const Ewoms::EclipseGrid&  grid_;
        const Ewoms::SummaryState& st_;
        const Ewoms::UDQConfig&    udq_;

        const Ewoms::EclIO::SummaryNode* node_;

        Ewoms::UnitSystem::measure paramUnit_;
        ofun paramFunction_;

        Descriptor functionRelation();
        Descriptor blockValue();
        Descriptor aquiferValue();
        Descriptor regionValue();
        Descriptor globalProcessValue();
        Descriptor userDefinedValue();
        Descriptor unknownParameter();

        bool isBlockValue();
        bool isAquiferValue();
        bool isRegionValue();
        bool isGlobalProcessValue();
        bool isFunctionRelation();
        bool isUserDefined();

        std::string functionUnitString() const;
        std::string directUnitString() const;
        std::string userDefinedUnit() const;
    };

    Factory::Descriptor Factory::create(const Ewoms::EclIO::SummaryNode& node)
    {
        this->node_ = &node;

        if (this->isUserDefined())
            return this->userDefinedValue();

        if (this->isBlockValue())
            return this->blockValue();

       if (this->isAquiferValue())
            return this->aquiferValue();

        if (this->isRegionValue())
            return this->regionValue();

        if (this->isGlobalProcessValue())
            return this->globalProcessValue();

        if (this->isFunctionRelation())
            return this->functionRelation();

        return this->unknownParameter();
    }

    Factory::Descriptor Factory::functionRelation()
    {
        auto desc = this->unknownParameter();

        desc.unit = this->functionUnitString();
        desc.evaluator.reset(new FunctionRelation {
            *this->node_, std::move(this->paramFunction_)
        });

        return desc;
    }

    Factory::Descriptor Factory::blockValue()
    {
        auto desc = this->unknownParameter();

        desc.unit = this->directUnitString();
        desc.evaluator.reset(new BlockValue {
            *this->node_, this->paramUnit_
        });

        return desc;
    }

    Factory::Descriptor Factory::aquiferValue()
    {
        auto desc = this->unknownParameter();

        desc.unit = this->directUnitString();
        desc.evaluator.reset(new AquiferValue {
                *this->node_, this->paramUnit_
        });

        return desc;
    }

    Factory::Descriptor Factory::regionValue()
    {
        auto desc = this->unknownParameter();

        desc.unit = this->directUnitString();
        desc.evaluator.reset(new RegionValue {
            *this->node_, this->paramUnit_
        });

        return desc;
    }

    Factory::Descriptor Factory::globalProcessValue()
    {
        auto desc = this->unknownParameter();

        desc.unit = this->directUnitString();
        desc.evaluator.reset(new GlobalProcessValue {
            *this->node_, this->paramUnit_
        });

        return desc;
    }

    Factory::Descriptor Factory::userDefinedValue()
    {
        auto desc = this->unknownParameter();

        desc.unit = this->userDefinedUnit();
        desc.evaluator.reset(new UserDefinedValue {});

        return desc;
    }

    Factory::Descriptor Factory::unknownParameter()
    {
        auto desc = Descriptor{};

        desc.uniquekey = this->node_->unique_key();

        return desc;
    }

    bool Factory::isBlockValue()
    {
        auto pos = block_units.find(this->node_->keyword);
        if (pos == block_units.end())
            return false;

        if (! this->grid_.cellActive(this->node_->number - 1))
            // 'node_' is a block value, but it is configured in a
            // deactivated cell.  Don't create an evaluation function.
            return false;

        // 'node_' represents a block value in an active cell.
        // Capture unit of measure and return true.
        this->paramUnit_ = pos->second;
        return true;
    }

    bool Factory::isAquiferValue()
    {
        auto pos = aquifer_units.find(this->node_->keyword);
        if (pos == aquifer_units.end()) return false;

        // if the aquifer does not exist, should we warn?
        if ( !this->es_.aquifer().hasAquifer(this->node_->number) ) return false;

        this->paramUnit_ = pos->second;
        return true;
    }

    bool Factory::isRegionValue()
    {
        auto keyword = this->node_->keyword;
        auto dash_pos = keyword.find("_");
        if (dash_pos != std::string::npos)
            keyword = keyword.substr(0, dash_pos);

        auto pos = region_units.find(keyword);
        if (pos == region_units.end())
            return false;

        // 'node_' represents a region value.  Capture unit
        // of measure and return true.
        this->paramUnit_ = pos->second;
        return true;
    }

    bool Factory::isGlobalProcessValue()
    {
        auto pos = single_values_units.find(this->node_->keyword);
        if (pos == single_values_units.end())
            return false;

        // 'node_' represents a single value (i.e., global process)
        // value.  Capture unit of measure and return true.
        this->paramUnit_ = pos->second;
        return true;
    }

    bool Factory::isFunctionRelation()
    {
        auto pos = funs.find(this->node_->keyword);
        if (pos != funs.end()) {
            // 'node_' represents a functional relation.
            // Capture evaluation function and return true.
            this->paramFunction_ = pos->second;
            return true;
        }

        auto keyword = this->node_->keyword;
        auto dash_pos = keyword.find("_");
        if (dash_pos != std::string::npos)
            keyword = keyword.substr(0, dash_pos);

        pos = funs.find(keyword);
        if (pos != funs.end()) {
            // 'node_' represents a functional relation.
            // Capture evaluation function and return true.
            this->paramFunction_ = pos->second;
            return true;
        }

        return false;
    }

    bool Factory::isUserDefined()
    {
        return this->node_->is_user_defined();
    }

    std::string Factory::functionUnitString() const
    {
        const auto reg = Ewoms::out::RegionCache{};

        using Foo = Ewoms::variant<std::string,int>;
        using Bar = Ewoms::optional<Foo>;
        const fn_args args {
            {}, "", 0.0, 0, std::max(0, this->node_->number),
            static_cast<bool>(this->node_->fip_region)
            ?   Bar(Foo(*this->node_->fip_region))
            :   Ewoms::nullopt,
            this->st_, {}, {}, reg, this->grid_,
            {}, {}, {}, Ewoms::UnitSystem(Ewoms::UnitSystem::UnitType::UNIT_TYPE_METRIC)
        };

        const auto prm = this->paramFunction_(args);

        return this->es_.getUnits().name(prm.unit);
    }

    std::string Factory::directUnitString() const
    {
        return this->es_.getUnits().name(this->paramUnit_);
    }

    std::string Factory::userDefinedUnit() const
    {
        const auto& kw = this->node_->keyword;

        return this->udq_.has_unit(kw)
            ?  this->udq_.unit(kw) : "";
    }
} // namespace Evaluator

void reportUnsupportedKeywords(std::vector<Ewoms::SummaryConfigNode> keywords)
{
    // Sort by location first, then keyword.
    auto loc_kw_ordering = [](const Ewoms::SummaryConfigNode& n1, const Ewoms::SummaryConfigNode& n2) {
        if (n1.location() == n2.location()) {
            return n1.keyword() < n2.keyword();
        }
        if (n1.location().filename == n2.location().filename) {
            return n1.location().lineno < n2.location().lineno;
        }
        return n1.location().filename < n2.location().filename;
    };
    std::sort(keywords.begin(), keywords.end(), loc_kw_ordering);

    // Reorder to remove duplicate { keyword, location } pairs, since
    // that will give duplicate and therefore useless warnings.
    auto same_kw_and_loc = [](const Ewoms::SummaryConfigNode& n1, const Ewoms::SummaryConfigNode& n2) {
        return (n1.keyword() == n2.keyword()) && (n1.location() == n2.location());
    };
    auto uend = std::unique(keywords.begin(), keywords.end(), same_kw_and_loc);

    for (auto node = keywords.begin(); node != uend; ++node) {
        const auto& location = node->location();
        Ewoms::OpmLog::warning(Ewoms::OpmInputError::format("Unhandled summary keyword {keyword}\n"
                                                        "In {file} line {line}", location));
    }
}

std::string makeWGName(std::string name)
{
    // Use default WGNAME if 'name' is empty or consists
    // exlusively of white-space (space and tab) characters.
    //
    // Use 'name' itself otherwise.

    const auto use_dflt = name.empty() ||
        (name.find_first_not_of(" \t") == std::string::npos);

    return use_dflt ? std::string(":+:+:+:+") : std::move(name);
}

class SummaryOutputParameters
{
public:
    using EvalPtr = std::unique_ptr<Evaluator::Base>;
    using SMSpecPrm = Ewoms::EclIO::OutputStream::
        SummarySpecification::Parameters;

    SummaryOutputParameters() = default;
    ~SummaryOutputParameters() = default;

    SummaryOutputParameters(const SummaryOutputParameters& rhs) = delete;
    SummaryOutputParameters(SummaryOutputParameters&& rhs) = default;

    SummaryOutputParameters&
    operator=(const SummaryOutputParameters& rhs) = delete;

    SummaryOutputParameters&
    operator=(SummaryOutputParameters&& rhs) = default;

    void makeParameter(std::string keyword,
                       std::string name,
                       const int   num,
                       std::string unit,
                       EvalPtr     evaluator)
    {
        this->smspec_.add(std::move(keyword), std::move(name),
                          std::max (num, 0), std::move(unit));

        this->evaluators_.push_back(std::move(evaluator));
    }

    const SMSpecPrm& summarySpecification() const
    {
        return this->smspec_;
    }

    const std::vector<EvalPtr>& getEvaluators() const
    {
        return this->evaluators_;
    }

private:
    SMSpecPrm smspec_{};
    std::vector<EvalPtr> evaluators_{};
};

class SMSpecStreamDeferredCreation
{
private:
    using Spec = ::Ewoms::EclIO::OutputStream::SummarySpecification;

public:
    using ResultSet = ::Ewoms::EclIO::OutputStream::ResultSet;
    using Formatted = ::Ewoms::EclIO::OutputStream::Formatted;

    explicit SMSpecStreamDeferredCreation(const Ewoms::InitConfig&          initcfg,
                                          const Ewoms::EclipseGrid&         grid,
                                          const std::time_t               start,
                                          const Ewoms::UnitSystem::UnitType utype);

    std::unique_ptr<Spec>
    createStream(const ResultSet& rset, const Formatted& fmt) const
    {
        return std::make_unique<Spec>(rset, fmt, this->uconv(),
                                      this->cartDims_, this->restart_,
                                      this->start_);
    }

private:
    Ewoms::UnitSystem::UnitType  utype_;
    std::array<int,3>          cartDims_;
    Spec::StartTime            start_;
    Spec::RestartSpecification restart_{};

    Spec::UnitConvention uconv() const;
};

SMSpecStreamDeferredCreation::
SMSpecStreamDeferredCreation(const Ewoms::InitConfig&          initcfg,
                             const Ewoms::EclipseGrid&         grid,
                             const std::time_t               start,
                             const Ewoms::UnitSystem::UnitType utype)
    : utype_   (utype)
    , cartDims_(grid.getNXYZ())
    , start_   (std::chrono::system_clock::from_time_t(start))
{
    if (initcfg.restartRequested()) {
        this->restart_.root = initcfg.getRestartRootName();
        this->restart_.step = initcfg.getRestartStep();
    }
}

SMSpecStreamDeferredCreation::Spec::UnitConvention
SMSpecStreamDeferredCreation::uconv() const
{
    using UType = ::Ewoms::UnitSystem::UnitType;

    if (this->utype_ == UType::UNIT_TYPE_METRIC)
        return Spec::UnitConvention::Metric;

    if (this->utype_ == UType::UNIT_TYPE_FIELD)
        return Spec::UnitConvention::Field;

    if (this->utype_ == UType::UNIT_TYPE_LAB)
        return Spec::UnitConvention::Lab;

    if (this->utype_ == UType::UNIT_TYPE_PVT_M)
        return Spec::UnitConvention::Pvt_M;

    throw std::invalid_argument {
        "Unsupported Unit Convention (" +
        std::to_string(static_cast<int>(this->utype_)) + ')'
    };
}

std::unique_ptr<SMSpecStreamDeferredCreation>
makeDeferredSMSpecCreation(const Ewoms::EclipseState& es,
                           const Ewoms::EclipseGrid&  grid,
                           const Ewoms::Schedule&     sched)
{
    return std::make_unique<SMSpecStreamDeferredCreation>
        (es.cfg().init(), grid, sched.posixStartTime(),
         es.getUnits().getType());
}

std::string makeUpperCase(std::string input)
{
    for (auto& c : input) {
        const auto u = std::toupper(static_cast<unsigned char>(c));
        c = static_cast<std::string::value_type>(u);
    }

    return input;
}

Ewoms::EclIO::OutputStream::ResultSet
makeResultSet(const Ewoms::IOConfig& iocfg, const std::string& basenm)
{
    const auto& base = basenm.empty()
        ? makeUpperCase(iocfg.getBaseName())
        : basenm;

    return { iocfg.getOutputDir(), base };
}

void validateElapsedTime(const double             secs_elapsed,
                         const Ewoms::EclipseState& es,
                         const Ewoms::SummaryState& st)
{
    if (! (secs_elapsed < st.get_elapsed()))
        return;

    const auto& usys    = es.getUnits();
    const auto  elapsed = usys.from_si(measure::time, secs_elapsed);
    const auto  prev_el = usys.from_si(measure::time, st.get_elapsed());
    const auto  unt     = '[' + std::string{ usys.name(measure::time) } + ']';

    throw std::invalid_argument {
        "Elapsed time ("
            + std::to_string(elapsed) + ' ' + unt
            + ") must not precede previous elapsed time ("
            + std::to_string(prev_el) + ' ' + unt
            + "). Incorrect restart time?"
            };
}

} // Anonymous namespace

class Ewoms::out::Summary::SummaryImplementation
{
public:
    explicit SummaryImplementation(const EclipseState&  es,
                                   const SummaryConfig& sumcfg,
                                   const EclipseGrid&   grid,
                                   const Schedule&      sched,
                                   const std::string&   basename);

    SummaryImplementation(const SummaryImplementation& rhs) = delete;
    SummaryImplementation(SummaryImplementation&& rhs) = default;
    SummaryImplementation& operator=(const SummaryImplementation& rhs) = delete;
    SummaryImplementation& operator=(SummaryImplementation&& rhs) = default;

    void eval(const int                          sim_step,
              const double                       secs_elapsed,
              const data::WellRates&             well_solution,
              const data::GroupAndNetworkValues& grp_nwrk_solution,
              GlobalProcessParameters&           single_values,
              const Inplace&                     initial_inplace,
              const Ewoms::Inplace&                inplace,
              const RegionParameters&            region_values,
              const BlockValues&                 block_values,
              const data::Aquifers&              aquifer_values,
              SummaryState&                      st) const;

    void internal_store(const SummaryState& st, const int report_step);
    void write();
    PAvgCalculatorCollection wbp_calculators(std::size_t report_step) const;

private:
    struct MiniStep
    {
        int id{0};
        int seq{-1};
        std::vector<float> params{};
    };

    using EvalPtr = SummaryOutputParameters::EvalPtr;

    std::reference_wrapper<const Ewoms::EclipseGrid> grid_;
    std::reference_wrapper<const Ewoms::EclipseState> es_;
    std::reference_wrapper<const Ewoms::Schedule> sched_;
    Ewoms::out::RegionCache regCache_;
    std::unordered_set<std::string> wbp_wells;

    std::unique_ptr<SMSpecStreamDeferredCreation> deferredSMSpec_;

    Ewoms::EclIO::OutputStream::ResultSet rset_;
    Ewoms::EclIO::OutputStream::Formatted fmt_;
    Ewoms::EclIO::OutputStream::Unified   unif_;

    int miniStepID_{0};
    int prevCreate_{-1};
    int prevReportStepID_{-1};
    std::vector<MiniStep>::size_type numUnwritten_{0};

    SummaryOutputParameters                  outputParameters_{};
    std::unordered_map<std::string, EvalPtr> extra_parameters{};
    std::vector<std::string> valueKeys_{};
    std::vector<MiniStep>    unwritten_{};

    std::unique_ptr<Ewoms::EclIO::OutputStream::SummarySpecification> smspec_{};
    std::unique_ptr<Ewoms::EclIO::EclOutput> stream_{};

    void configureTimeVectors(const EclipseState& es, const SummaryConfig& sumcfg);

    void configureSummaryInput(const EclipseState&  es,
                               const SummaryConfig& sumcfg,
                               const EclipseGrid&   grid,
                               const Schedule&      sched);

    void configureRequiredRestartParameters(const SummaryConfig& sumcfg,
                                            const Schedule&      sched);

    void configureUDQ(const SummaryConfig& summary_config, const Schedule& sched);

    MiniStep& getNextMiniStep(const int report_step);
    const MiniStep& lastUnwritten() const;

    void write(const MiniStep& ms);

    void createSMSpecIfNecessary();
    void createSmryStreamIfNecessary(const int report_step);
};

Ewoms::out::Summary::SummaryImplementation::
SummaryImplementation(const EclipseState&  es,
                      const SummaryConfig& sumcfg,
                      const EclipseGrid&   grid,
                      const Schedule&      sched,
                      const std::string&   basename)
    : grid_          (std::cref(grid))
    , es_            (std::cref(es))
    , sched_         (std::cref(sched))
    , regCache_      (sumcfg.fip_regions(), es.globalFieldProps(), grid, sched)
    , deferredSMSpec_(makeDeferredSMSpecCreation(es, grid, sched))
    , rset_          (makeResultSet(es.cfg().io(), basename))
    , fmt_           { es.cfg().io().getFMTOUT() }
    , unif_          { es.cfg().io().getUNIFOUT() }
{
    this->configureTimeVectors(es, sumcfg);
    this->configureSummaryInput(es, sumcfg, grid, sched);
    this->configureRequiredRestartParameters(sumcfg, sched);
    this->configureUDQ(sumcfg, sched);

    for (const auto& config_node : sumcfg.keywords("WBP*"))
        this->wbp_wells.insert( config_node.namedEntity() );
}

void Ewoms::out::Summary::SummaryImplementation::
internal_store(const SummaryState& st, const int report_step)
{
    auto& ms = this->getNextMiniStep(report_step);

    const auto nParam = this->valueKeys_.size();

    for (auto i = decltype(nParam){0}; i < nParam; ++i) {
        if (! st.has(this->valueKeys_[i]))
            // Parameter not yet evaluated (e.g., well/group not
            // yet active).  Nothing to do here.
            continue;

        ms.params[i] = st.get(this->valueKeys_[i]);
    }
}

Ewoms::PAvgCalculatorCollection Ewoms::out::Summary::SummaryImplementation::wbp_calculators(std::size_t report_step) const {
    if (this->wbp_wells.empty())
        return {};

    Ewoms::PAvgCalculatorCollection calculators;
    const auto& porv = this->es_.get().globalFieldProps().porv(true);
    for (const auto& wname : this->wbp_wells) {
        if (this->sched_.get().hasWell(wname, report_step)) {
            const auto& well = this->sched_.get().getWell(wname, report_step);
            if (well.getStatus() == Ewoms::Well::Status::OPEN)
                calculators.add(well.pavg_calculator(this->grid_, porv));
        }
    }
    return calculators;
}

void
Ewoms::out::Summary::SummaryImplementation::
eval(const int                          sim_step,
     const double                       secs_elapsed,
     const data::WellRates&             well_solution,
     const data::GroupAndNetworkValues& grp_nwrk_solution,
     GlobalProcessParameters&           single_values,
     const Inplace&                     initial_inplace,
     const Ewoms::Inplace&                inplace,
     const RegionParameters&            region_values,
     const BlockValues&                 block_values,
     const data::Aquifers&              aquifer_values,
     Ewoms::SummaryState&                 st) const
{
    validateElapsedTime(secs_elapsed, this->es_, st);

    const double duration = secs_elapsed - st.get_elapsed();
    single_values["TIMESTEP"] = duration;
    st.update("TIMESTEP", this->es_.get().getUnits().from_si(Ewoms::UnitSystem::measure::time, duration));

    const Evaluator::InputData input {
        this->es_, this->sched_, this->grid_, this->regCache_, initial_inplace
    };

    const Evaluator::SimulatorResults simRes {
        well_solution, grp_nwrk_solution, single_values, inplace, region_values, block_values, aquifer_values
    };

    for (auto& evalPtr : this->outputParameters_.getEvaluators()) {
        evalPtr->update(sim_step, duration, input, simRes, st);
    }

    for (auto& ePair : this->extra_parameters) {
        auto& evalPtr = ePair.second;
        evalPtr->update(sim_step, duration, input, simRes, st);
    }

    st.update_elapsed(duration);
}

void Ewoms::out::Summary::SummaryImplementation::write()
{
    const auto zero = std::vector<MiniStep>::size_type{0};
    if (this->numUnwritten_ == zero)
        // No unwritten data.  Nothing to do so return early.
        return;

    this->createSMSpecIfNecessary();

    if (this->prevReportStepID_ < this->lastUnwritten().seq) {
        this->smspec_->write(this->outputParameters_.summarySpecification());
    }

    for (auto i = 0*this->numUnwritten_; i < this->numUnwritten_; ++i)
        this->write(this->unwritten_[i]);

    // Eagerly output last set of parameters to permanent storage.
    this->stream_->flushStream();

    // Reset "unwritten" counter to reflect the fact that we've
    // output all stored ministeps.
    this->numUnwritten_ = zero;
}

void Ewoms::out::Summary::SummaryImplementation::write(const MiniStep& ms)
{
    this->createSmryStreamIfNecessary(ms.seq);

    if (this->prevReportStepID_ < ms.seq) {
        // XXX: Should probably write SEQHDR = 0 here since
        ///     we do not know the actual encoding needed.
        this->stream_->write("SEQHDR", std::vector<int>{ ms.seq });
        this->prevReportStepID_ = ms.seq;
    }

    this->stream_->write("MINISTEP", std::vector<int>{ ms.id });
    this->stream_->write("PARAMS"  , ms.params);
}

void
Ewoms::out::Summary::SummaryImplementation::
configureTimeVectors(const EclipseState& es, const SummaryConfig& sumcfg)
{
    const auto dfltwgname = std::string(":+:+:+:+");
    const auto dfltnum    = 0;

    // XXX: Save keys might need/want to include a random component too.
    auto makeKey = [this](const std::string& keyword) -> void
    {
        this->valueKeys_.push_back(
            "SMSPEC.Internal." + keyword + ".Value.SAVE"
        );
    };

    // TIME
    {
        const auto& kw = std::string("TIME");
        makeKey(kw);

        const std::string& unit_string = es.getUnits().name(UnitSystem::measure::time);
        auto eval = std::make_unique<Evaluator::Time>(this->valueKeys_.back());

        this->outputParameters_
            .makeParameter(kw, dfltwgname, dfltnum, unit_string, std::move(eval));
    }

    if (sumcfg.hasKeyword("DAY")) {
        const auto& kw = std::string("DAY");
        makeKey(kw);

        auto eval = std::make_unique<Evaluator::Day>(this->valueKeys_.back());
        this->outputParameters_
            .makeParameter(kw, dfltwgname, dfltnum, "", std::move(eval));
    }

    if (sumcfg.hasKeyword("MONTH")) {
        const auto& kw = std::string("MONTH");
        makeKey(kw);

        auto eval = std::make_unique<Evaluator::Month>(this->valueKeys_.back());
        this->outputParameters_
            .makeParameter(kw, dfltwgname, dfltnum, "", std::move(eval));
    }

    if (sumcfg.hasKeyword("YEAR")) {
        const auto& kw = std::string("YEAR");
        makeKey(kw);

        auto eval = std::make_unique<Evaluator::Year>(this->valueKeys_.back());
        this->outputParameters_
            .makeParameter(kw, dfltwgname, dfltnum, "", std::move(eval));
    }

    // YEARS
    {
        const auto& kw = std::string("YEARS");
        makeKey(kw);

        auto eval = std::make_unique<Evaluator::Years>(this->valueKeys_.back());

        this->outputParameters_
            .makeParameter(kw, dfltwgname, dfltnum, kw, std::move(eval));
    }
}

void
Ewoms::out::Summary::SummaryImplementation::
configureSummaryInput(const EclipseState&  es,
                      const SummaryConfig& sumcfg,
                      const EclipseGrid&   grid,
                      const Schedule&      sched)
{
    const auto st = SummaryState {
        std::chrono::system_clock::from_time_t(sched.getStartTime())
    };

    Evaluator::Factory fact {
        es, grid, st, sched.getUDQConfig(sched.size() - 1)
    };

    auto unsuppkw = std::vector<SummaryConfigNode>{};
    for (const auto& node : sumcfg) {
        auto prmDescr = fact.create(node);

        if (! prmDescr.evaluator) {
            // No known evaluation function/type for this keyword
            unsuppkw.push_back(node);
            continue;
        }

        // This keyword has a known evaluation method.

        this->valueKeys_.push_back(std::move(prmDescr.uniquekey));

        this->outputParameters_
            .makeParameter(node.keyword(),
                           makeWGName(node.namedEntity()),
                           node.number(),
                           std::move(prmDescr.unit),
                           std::move(prmDescr.evaluator));
    }

    if (! unsuppkw.empty())
        reportUnsupportedKeywords(std::move(unsuppkw));
}

/*
   These nodes are added to the summary evaluation list because they are
   requested by the UDQ system. In the case of well and group variables the code
   will all nodes for all wells / groups - irrespective of what has been
   requested in the UDQ code.
*/

std::vector<Ewoms::EclIO::SummaryNode> make_default_nodes(const std::string& keyword, const Ewoms::Schedule& sched) {
    auto nodes = std::vector<Ewoms::EclIO::SummaryNode> {};
    auto category = Ewoms::parseKeywordCategory(keyword);
    auto type = Ewoms::parseKeywordType(keyword);

    switch (category) {
    case Ewoms::EclIO::SummaryNode::Category::Field:
        {
            Ewoms::EclIO::SummaryNode node;
            node.keyword = keyword;
            node.category = category;
            node.type = type;

            nodes.push_back(node);
        }
        break;
    case Ewoms::EclIO::SummaryNode::Category::Miscellaneous:
        {
            Ewoms::EclIO::SummaryNode node;
            node.keyword = keyword;
            node.category = category;
            node.type = type;

            nodes.push_back(node);
        }
        break;
    case Ewoms::EclIO::SummaryNode::Category::Well:
        {
            for (const auto& well : sched.wellNames()) {
                Ewoms::EclIO::SummaryNode node;
                node.keyword = keyword;
                node.category = category;
                node.type = type;
                node.wgname = well;

                nodes.push_back(node);
            }
        }
        break;
    case Ewoms::EclIO::SummaryNode::Category::Group:
        {
            for (const auto& group : sched.groupNames()) {
                Ewoms::EclIO::SummaryNode node;
                node.keyword = keyword;
                node.category = category;
                node.type = type;
                node.wgname = group;

                nodes.push_back(node);
            }
        }
        break;
    default:
        throw std::logic_error(fmt::format("make_default_nodes does not yet support: {}", keyword));
    }

    return nodes;
}

void Ewoms::out::Summary::SummaryImplementation::configureUDQ(const SummaryConfig& summary_config, const Schedule& sched) {
    const std::unordered_set<std::string> time_vectors = {"TIME", "DAY", "MONTH", "YEAR", "YEARS", "MNTH"};
    auto nodes = std::vector<Ewoms::EclIO::SummaryNode> {};
    std::unordered_set<std::string> summary_keys;
    for (const auto& udq_ptr : sched.udqConfigList())
        udq_ptr->required_summary(summary_keys);

    for (const auto& action : sched.actions(sched.size() - 1))
        action.required_summary(summary_keys);

    for (const auto& key : summary_keys) {
        const auto& default_nodes = make_default_nodes(key, sched);
        for (const auto& def_node : default_nodes)
            nodes.push_back(def_node);
    }

    for (const auto& node: nodes) {
        // Handler already configured/requested through the normal SummaryConfig path.
        if (summary_config.hasSummaryKey(node.unique_key()))
            continue;

        // Time related vectors are special cased in the valueKeys_ vector and must be checked explicitly.
        if (time_vectors.count(node.keyword) > 0)
            continue;

        // Handler already registered in the summary evaluator, in some other way.
        if ( std::find(this->valueKeys_.begin(), this->valueKeys_.end(), node.unique_key()) != this->valueKeys_.end())
            continue;

        auto fun_pos = funs.find(node.keyword);
        if (fun_pos != funs.end()) {
            this->extra_parameters.emplace( node.unique_key(), std::make_unique<Evaluator::FunctionRelation>(node, fun_pos->second) );
            continue;
        }

        auto unit = single_values_units.find(node.keyword);
        if (unit != single_values_units.end()) {
            this->extra_parameters.emplace( node.unique_key(), std::make_unique<Evaluator::GlobalProcessValue>(node, unit->second));
            continue;
        }

        if (node.is_user_defined())
            continue;

        if (TimeMap::valid_month(node.keyword))
            continue;

        throw std::logic_error(fmt::format("Evaluation function for: {} not found ", node.keyword));
    }
}

void
Ewoms::out::Summary::SummaryImplementation::
configureRequiredRestartParameters(const SummaryConfig& sumcfg,
                                   const Schedule&      sched)
{
    auto makeEvaluator = [&sumcfg, this](const Ewoms::EclIO::SummaryNode& node) -> void
    {
        if (sumcfg.hasSummaryKey(node.unique_key()))
            // Handler already exists.  Don't add second evaluation.
            return;

        auto fcnPos = funs.find(node.keyword);
        if (fcnPos == funs.end())
            throw std::logic_error(fmt::format("Evaluation function for:{} not found", node.keyword));

        auto eval = std::make_unique<
            Evaluator::FunctionRelation>(node, fcnPos->second);

        this->extra_parameters.emplace(node.unique_key(), std::move(eval));
    };

    for (const auto& node : requiredRestartVectors(sched))
        makeEvaluator(node);

    for (const auto& node : requiredSegmentVectors(sched))
        makeEvaluator(node);
}

Ewoms::out::Summary::SummaryImplementation::MiniStep&
Ewoms::out::Summary::SummaryImplementation::getNextMiniStep(const int report_step)
{
    if (this->numUnwritten_ == this->unwritten_.size())
        this->unwritten_.emplace_back();

    assert ((this->numUnwritten_ < this->unwritten_.size()) &&
            "Internal inconsistency in 'unwritten' counter");

    auto& ms = this->unwritten_[this->numUnwritten_++];

    ms.id  = this->miniStepID_++;  // MINSTEP IDs start at zero.
    ms.seq = report_step;

    ms.params.resize(this->valueKeys_.size(), 0.0f);

    std::fill(ms.params.begin(), ms.params.end(), 0.0f);

    return ms;
}

const Ewoms::out::Summary::SummaryImplementation::MiniStep&
Ewoms::out::Summary::SummaryImplementation::lastUnwritten() const
{
    assert (this->numUnwritten_ <= this->unwritten_.size());
    assert (this->numUnwritten_ >  decltype(this->numUnwritten_){0});

    return this->unwritten_[this->numUnwritten_ - 1];
}

void Ewoms::out::Summary::SummaryImplementation::createSMSpecIfNecessary()
{
    if (this->deferredSMSpec_) {
        // We need an SMSPEC file and none exists.  Create it and release
        // the resources captured to make the deferred creation call.
        this->smspec_ = this->deferredSMSpec_
            ->createStream(this->rset_, this->fmt_);

        this->deferredSMSpec_.reset();
    }
}

void
Ewoms::out::Summary::SummaryImplementation::
createSmryStreamIfNecessary(const int report_step)
{
    // Create stream if unset or if non-unified (separate) and new step.

    assert ((this->prevCreate_ <= report_step) &&
            "Inconsistent Report Step Sequence Detected");

    const auto do_create = ! this->stream_
        || (! this->unif_.set && (this->prevCreate_ < report_step));

    if (do_create) {
        this->stream_ = Ewoms::EclIO::OutputStream::
            createSummaryFile(this->rset_, report_step,
                              this->fmt_, this->unif_);

        this->prevCreate_ = report_step;
    }
}

namespace Ewoms { namespace out {

Summary::Summary(const EclipseState&  es,
                 const SummaryConfig& sumcfg,
                 const EclipseGrid&   grid,
                 const Schedule&      sched,
                 const std::string&   basename)
    : pImpl_(new SummaryImplementation(es, sumcfg, grid, sched, basename))
{}

void Summary::eval(SummaryState&                      st,
                   const int                          report_step,
                   const double                       secs_elapsed,
                   const data::WellRates&             well_solution,
                   const data::GroupAndNetworkValues& grp_nwrk_solution,
                   GlobalProcessParameters            single_values,
                   const Inplace&                     initial_inplace,
                   const Inplace&                     inplace,
                   const PAvgCalculatorCollection&    ,
                   const RegionParameters&            region_values,
                   const BlockValues&                 block_values,
                   const Ewoms::data::Aquifers&         aquifer_values) const
{

    /* Report_step is the one-based sequence number of the containing report.
     * Report_step = 0 for the initial condition, before simulation starts.
     * We typically don't get reports_step = 0 here.  When outputting
     * separate summary files 'report_step' is the number that gets
     * incorporated into the filename extension.
     *
     * Sim_step is the timestep which has been effective in the simulator,
     * and as such is the value necessary to use when looking up active
     * wells, groups, connections &c in the Schedule object. */
    const auto sim_step = std::max( 0, report_step - 1 );

    this->pImpl_->eval(sim_step, secs_elapsed,
                       well_solution, grp_nwrk_solution, single_values,
                       initial_inplace, inplace,
                       region_values, block_values, aquifer_values, st);
}

PAvgCalculatorCollection Summary::wbp_calculators(std::size_t report_step) const {
    return this->pImpl_->wbp_calculators(report_step);
}

void Summary::add_timestep(const SummaryState& st, const int report_step)
{
    this->pImpl_->internal_store(st, report_step);
}

void Summary::write() const
{
    this->pImpl_->write();
}

Summary::~Summary() {}

}} // namespace Ewoms::out
