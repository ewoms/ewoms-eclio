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

#include <ewoms/eclio/output/summary.hh>

#include <ewoms/eclio/opmlog/opmlog.hh>
#include <ewoms/eclio/opmlog/location.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqcontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellproductionproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellinjectionproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/summaryconfig/summaryconfig.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/units/units.hh>

#include <ewoms/eclio/io/ecloutput.hh>
#include <ewoms/eclio/io/outputstream.hh>

#include <ewoms/eclio/output/data/wells.hh>
#include <ewoms/eclio/output/regioncache.hh>

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
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {
    struct ParamCTorArgs
    {
        std::string kw;
        Ewoms::SummaryNode::Type type;
    };

    std::vector<ParamCTorArgs> requiredRestartVectors()
    {
        using Type = ::Ewoms::SummaryNode::Type;

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
            ParamCTorArgs{ "WITH", Type::Total },
            ParamCTorArgs{ "GITH", Type::Total },
        };
    }

    std::vector<Ewoms::SummaryNode>
    requiredRestartVectors(const ::Ewoms::Schedule& sched)
    {
        auto entities = std::vector<Ewoms::SummaryNode>{};

        using SN = ::Ewoms::SummaryNode;

        const auto& vectors = requiredRestartVectors();

        auto makeEntities = [&vectors, &entities]
            (const char         kwpref,
             const SN::Category cat,
             const std::string& name) -> void
        {
            for (const auto& vector : vectors) {
                entities.emplace_back(kwpref + vector.kw, cat, ::Ewoms::Location());

                entities.back().namedEntity(name)
                .parameterType(vector.type);
            }
        };

        for (const auto& well_name : sched.wellNames()) {
            makeEntities('W', SN::Category::Well, well_name);

            entities.emplace_back("WBHP", SN::Category::Well, ::Ewoms::Location());
            entities.back().namedEntity(well_name)
            .parameterType(SN::Type::Pressure);

            entities.emplace_back("WGVIR", SN::Category::Well, ::Ewoms::Location());
            entities.back().namedEntity(well_name)
            .parameterType(SN::Type::Rate);

            entities.emplace_back("WWVIR", SN::Category::Well, ::Ewoms::Location());
            entities.back().namedEntity(well_name)
            .parameterType(SN::Type::Rate);
        }

        for (const auto& grp_name : sched.groupNames()) {
            if (grp_name != "FIELD")
                makeEntities('G', SN::Category::Group, grp_name);
        }

        makeEntities('F', SN::Category::Field, "FIELD");

        return entities;
    }

    std::vector<Ewoms::SummaryNode>
    requiredSegmentVectors(const ::Ewoms::Schedule& sched)
    {
        using SN = Ewoms::SummaryNode;
        auto ret = std::vector<SN>{};

        auto sofr = SN{ "SOFR", SN::Category::Segment, ::Ewoms::Location() }
            .parameterType(SN::Type::Rate);

        auto sgfr = SN{ "SGFR", SN::Category::Segment, ::Ewoms::Location() }
            .parameterType(SN::Type::Rate);

        auto swfr = SN{ "SWFR", SN::Category::Segment, ::Ewoms::Location() }
            .parameterType(SN::Type::Rate);

        auto spr = SN{ "SPR", SN::Category::Segment, ::Ewoms::Location() }
            .parameterType(SN::Type::Pressure);

        auto makeVectors =
            [&](const std::string& well,
                const int          segNumber) -> void
        {
            ret.push_back(sofr.namedEntity(well).number(segNumber));
            ret.push_back(sgfr.namedEntity(well).number(segNumber));
            ret.push_back(swfr.namedEntity(well).number(segNumber));
            ret.push_back(spr .namedEntity(well).number(segNumber));
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

/*
 * This class takes simulator state and parser-provided information and
 * orchestrates ert to write simulation results as requested by the SUMMARY
 * section in eclipse. The implementation is somewhat compact as a lot of the
 * requested output may be similar-but-not-quite. Through various techniques
 * the compiler writes a lot of this code for us.
 */

using rt = Ewoms::data::Rates::opt;
using measure = Ewoms::UnitSystem::measure;
constexpr const bool injector = true;
constexpr const bool producer = false;
constexpr const bool polymer = true;

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
    double duration;
    const int sim_step;
    int  num;
    const Ewoms::SummaryState& st;
    const Ewoms::data::Wells& wells;
    const Ewoms::out::RegionCache& regionCache;
    const Ewoms::EclipseGrid& grid;
    const std::vector< std::pair< std::string, double > > eff_factors;
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

double efac( const std::vector<std::pair<std::string,double>>& eff_factors, const std::string& name ) {
    auto it = std::find_if( eff_factors.begin(), eff_factors.end(),
                            [&] ( const std::pair< std::string, double > elem )
                            { return elem.first == name; }
                          );

    return (it != eff_factors.end()) ? it->second : 1;
}

template< rt phase, bool injection = true, bool polymer = false >
inline quantity rate( const fn_args& args ) {
    double sum = 0.0;

    for( const auto& sched_well : args.schedule_wells ) {
        const auto& name = sched_well.name();
        if( args.wells.count( name ) == 0 ) continue;

        double eff_fac = efac( args.eff_factors, name );

        double concentration = polymer
                             ? sched_well.getPolymerProperties().m_polymerConcentration
                             : 1;

        const auto v = args.wells.at(name).rates.get(phase, 0.0) * eff_fac * concentration;

        if( ( v > 0 ) == injection )
            sum += v;
    }

    if( !injection ) sum *= -1;

    if( polymer ) return { sum, measure::mass_rate };
    return { sum, rate_unit< phase >() };
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

template< rt phase, bool injection = true, bool polymer = false >
inline quantity crate( const fn_args& args ) {
    const quantity zero = { 0, rate_unit< phase >() };
    // The args.num value is the literal value which will go to the
    // NUMS array in the eclispe SMSPEC file; the values in this array
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

    if( completion == well_data.connections.end() ) return zero;

    double eff_fac = efac( args.eff_factors, name );
    double concentration = polymer
                           ? well.getPolymerProperties().m_polymerConcentration
                           : 1;

    auto v = completion->rates.get( phase, 0.0 ) * eff_fac * concentration;
    if( ( v > 0 ) != injection ) return zero;
    if( !injection ) v *= -1;

    if( polymer ) return { v, measure::mass_rate };
    return { v, rate_unit< phase >() };
}

template< rt phase, bool polymer = false >
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
    double concentration = polymer
                           ? well.getPolymerProperties().m_polymerConcentration
                           : 1;

    auto v = segment->second.rates.get( phase, 0.0 ) * eff_fac * concentration;
    //switch sign of rate - opposite convention in flow vs eclipse
    v *= -1;

    if( polymer ) return { v, measure::mass_rate };
    return { v, rate_unit< phase >() };
}

inline quantity trans_factors ( const fn_args& args ) {
    const quantity zero = { 0, measure::transmissibility };

    if( args.schedule_wells.empty() ) return zero;
    // Like completion rate we need to look
    // up a connection with offset 0.
    const size_t global_index = args.num - 1;

    const auto& well = args.schedule_wells.front();
    const auto& name = well.name();
    if( args.wells.count( name ) == 0 ) return zero;

    const auto& grid = args.grid;
    const auto& connections = well.getConnections();

    const auto& connection = std::find_if(
        connections.begin(),
        connections.end(),
        [=]( const Ewoms::Connection& c ) {
            return grid.getGlobalIndex(c.getI(), c.getJ(), c.getK()) == global_index;
        } );

    if( connection == connections.end() ) return zero;

    const auto& v = connection->CF() * connection->wellPi();
    return { v, measure::transmissibility };
}

inline quantity spr ( const fn_args& args ) {
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

    const auto& v = segment->second.pressure;
    return { v, measure::pressure };
}

inline quantity bhp( const fn_args& args ) {
    const quantity zero = { 0, measure::pressure };
    if( args.schedule_wells.empty() ) return zero;

    const auto p = args.wells.find( args.schedule_wells.front().name() );
    if( p == args.wells.end() ) return zero;

    return { p->second.bhp, measure::pressure };
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
    const auto& well_connections = args.regionCache.connections( args.num );

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

template < rt phase, bool outputProducer = true, bool outputInjector = true>
inline quantity potential_rate( const fn_args& args ) {
    double sum = 0.0;

    for( const auto& sched_well : args.schedule_wells ) {
        const auto& name = sched_well.name();
        if( args.wells.count( name ) == 0 ) continue;

        if (sched_well.isInjector() && outputInjector) {
	    const auto v = args.wells.at(name).rates.get(phase, 0.0);
	    sum += v;
	}
	else if (sched_well.isProducer() && outputProducer) {
	    const auto v = args.wells.at(name).rates.get(phase, 0.0);
	    sum += v;
	}
    }

    return { sum, rate_unit< phase >() };
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
    { "WOIR", rate< rt::oil, injector > },
    { "WGIR", rate< rt::gas, injector > },
    { "WNIR", rate< rt::solvent, injector > },
    { "WCIR", rate< rt::wat, injector, polymer > },
    { "WVIR", sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                       rate< rt::reservoir_gas, injector > ) },

    { "WWIT", mul( rate< rt::wat, injector >, duration ) },
    { "WOIT", mul( rate< rt::oil, injector >, duration ) },
    { "WGIT", mul( rate< rt::gas, injector >, duration ) },
    { "WNIT", mul( rate< rt::solvent, injector >, duration ) },
    { "WCIT", mul( rate< rt::wat, injector, polymer >, duration ) },
    { "WVIT", mul( sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                        rate< rt::reservoir_gas, injector > ), duration ) },

    { "WWPR", rate< rt::wat, producer > },
    { "WOPR", rate< rt::oil, producer > },
    { "WGPR", rate< rt::gas, producer > },
    { "WNPR", rate< rt::solvent, producer > },

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
    { "WNPT", mul( rate< rt::solvent, producer >, duration ) },
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
    { "WVPRT", res_vol_production_target },

    { "GWIR", rate< rt::wat, injector > },
    { "WGVIR", rate< rt::reservoir_gas, injector >},
    { "WWVIR", rate< rt::reservoir_water, injector >},
    { "GOIR", rate< rt::oil, injector > },
    { "GGIR", rate< rt::gas, injector > },
    { "GNIR", rate< rt::solvent, injector > },
    { "GCIR", rate< rt::wat, injector, polymer > },
    { "GVIR", sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                        rate< rt::reservoir_gas, injector > ) },
    { "GWIT", mul( rate< rt::wat, injector >, duration ) },
    { "GOIT", mul( rate< rt::oil, injector >, duration ) },
    { "GGIT", mul( rate< rt::gas, injector >, duration ) },
    { "GNIT", mul( rate< rt::solvent, injector >, duration ) },
    { "GCIT", mul( rate< rt::wat, injector, polymer >, duration ) },
    { "GVIT", mul( sum( sum( rate< rt::reservoir_water, injector >, rate< rt::reservoir_oil, injector > ),
                        rate< rt::reservoir_gas, injector > ), duration ) },

    { "GWPR", rate< rt::wat, producer > },
    { "GOPR", rate< rt::oil, producer > },
    { "GGPR", rate< rt::gas, producer > },
    { "GNPR", rate< rt::solvent, producer > },
    { "GOPRS", rate< rt::vaporized_oil, producer > },
    { "GOPRF", sub (rate < rt::oil, producer >, rate< rt::vaporized_oil, producer > ) },
    { "GLPR", sum( rate< rt::wat, producer >, rate< rt::oil, producer > ) },
    { "GVPR", sum( sum( rate< rt::reservoir_water, producer >, rate< rt::reservoir_oil, producer > ),
                        rate< rt::reservoir_gas, producer > ) },

    { "GWPT", mul( rate< rt::wat, producer >, duration ) },
    { "GOPT", mul( rate< rt::oil, producer >, duration ) },
    { "GGPT", mul( rate< rt::gas, producer >, duration ) },
    { "GNPT", mul( rate< rt::solvent, producer >, duration ) },
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

    { "CWIR", crate< rt::wat, injector > },
    { "CGIR", crate< rt::gas, injector > },
    { "CCIR", crate< rt::wat, injector, polymer > },
    { "CWIT", mul( crate< rt::wat, injector >, duration ) },
    { "CGIT", mul( crate< rt::gas, injector >, duration ) },
    { "CNIT", mul( crate< rt::solvent, injector >, duration ) },

    { "CWPR", crate< rt::wat, producer > },
    { "COPR", crate< rt::oil, producer > },
    { "CGPR", crate< rt::gas, producer > },
    // Minus for injection rates and pluss for production rate
    { "CNFR", sub( crate< rt::solvent, producer >, crate<rt::solvent, injector >) },
    { "CWPT", mul( crate< rt::wat, producer >, duration ) },
    { "COPT", mul( crate< rt::oil, producer >, duration ) },
    { "CGPT", mul( crate< rt::gas, producer >, duration ) },
    { "CNPT", mul( crate< rt::solvent, producer >, duration ) },
    { "CCIT", mul( crate< rt::wat, injector, polymer >, duration ) },
    { "CCPT", mul( crate< rt::wat, producer, polymer >, duration ) },
    { "CTFAC", trans_factors },

    { "FWPR", rate< rt::wat, producer > },
    { "FOPR", rate< rt::oil, producer > },
    { "FGPR", rate< rt::gas, producer > },
    { "FNPR", rate< rt::solvent, producer > },
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
    { "FNPT", mul( rate< rt::solvent, producer >, duration ) },
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
    { "FNIR", rate< rt::solvent, injector > },
    { "FCIR", rate< rt::wat, injector, polymer > },
    { "FCPR", rate< rt::wat, producer, polymer > },
    { "FVIR", sum( sum( rate< rt::reservoir_water, injector>, rate< rt::reservoir_oil, injector >),
                   rate< rt::reservoir_gas, injector>)},

    { "FLIR", sum( rate< rt::wat, injector >, rate< rt::oil, injector > ) },
    { "FWIT", mul( rate< rt::wat, injector >, duration ) },
    { "FOIT", mul( rate< rt::oil, injector >, duration ) },
    { "FGIT", mul( rate< rt::gas, injector >, duration ) },
    { "FNIT", mul( rate< rt::solvent, injector >, duration ) },
    { "FCIT", mul( rate< rt::wat, injector, polymer >, duration ) },
    { "FCPT", mul( rate< rt::wat, producer, polymer >, duration ) },
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
    //Multisegment well segment data
    { "SOFR", srate< rt::oil > },
    { "SWFR", srate< rt::wat > },
    { "SGFR", srate< rt::gas > },
    { "SPR",  spr },
    // Well productivity index
    { "WPIW", potential_rate< rt::productivity_index_water >},
    { "WPIO", potential_rate< rt::productivity_index_oil >},
    { "WPIG", potential_rate< rt::productivity_index_gas >},
    { "WPIL", sum( potential_rate< rt::productivity_index_water >, potential_rate< rt::productivity_index_oil>)},
    // Well potential
    { "WWPP", potential_rate< rt::well_potential_water , true, false>},
    { "WOPP", potential_rate< rt::well_potential_oil , true, false>},
    { "WGPP", potential_rate< rt::well_potential_gas , true, false>},
    { "WWPI", potential_rate< rt::well_potential_water , false, true>},
    { "WOPI", potential_rate< rt::well_potential_oil , false, true>},
    { "WGPI", potential_rate< rt::well_potential_gas , false, true>},
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
  {"RWIP"     , Ewoms::UnitSystem::measure::liquid_surface_volume }
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

inline std::vector<Ewoms::Well> find_wells( const Ewoms::Schedule& schedule,
                                           const Ewoms::SummaryNode& node,
                                           const int sim_step,
                                           const Ewoms::out::RegionCache& regionCache ) {

    const auto cat = node.category();

    if ((cat == Ewoms::SummaryNode::Category::Well) ||
        (cat == Ewoms::SummaryNode::Category::Connection) ||
        (cat == Ewoms::SummaryNode::Category::Segment))
    {
        const auto& name = node.namedEntity();

        if (schedule.hasWell(name, sim_step)) {
            const auto& well = schedule.getWell( name, sim_step );
            return { well };
        } else
            return {};
    }

    if( cat == Ewoms::SummaryNode::Category::Group ) {
        const auto& name = node.namedEntity();

        if( !schedule.hasGroup( name ) ) return {};

        return schedule.getChildWells2( name, sim_step);
    }

    if( cat == Ewoms::SummaryNode::Category::Field )
        return schedule.getWells(sim_step);

    if( cat == Ewoms::SummaryNode::Category::Region ) {
        std::vector<Ewoms::Well> wells;

        const auto region = node.number();

        for ( const auto& connection : regionCache.connections( region ) ){
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

    return {};
}

bool need_wells(Ewoms::SummaryNode::Category cat, const std::string& keyword) {
    static const std::set<std::string> region_keywords{"ROIR", "RGIR", "RWIR", "ROPR", "RGPR", "RWPR", "ROIT", "RWIT", "RGIT", "ROPT", "RGPT", "RWPT"};
    if (cat == Ewoms::SummaryNode::Category::Well)
        return true;

    if (cat == Ewoms::SummaryNode::Category::Group)
        return true;

    if (cat == Ewoms::SummaryNode::Category::Field)
        return true;

    if (cat == Ewoms::SummaryNode::Category::Connection)
        return true;

    if (cat == Ewoms::SummaryNode::Category::Segment)
        return true;

    /*
      Some of the region keywords are based on summing over all the connections
      which fall in the region; i.e. RGIR is the total gas injection rate in the
      region and consequently the list of defined wells is required, other
      region keywords like 'ROIP' do not require well information.
    */
    if (cat == Ewoms::SummaryNode::Category::Region) {
        if (region_keywords.count(keyword) > 0)
            return true;
    }

    return false;
}

void eval_udq(const Ewoms::Schedule& schedule, std::size_t sim_step, Ewoms::SummaryState& st)
{
    using namespace Ewoms;

    const UDQConfig& udq = schedule.getUDQConfig(sim_step);
    const auto& func_table = udq.function_table();
    UDQContext context(func_table, st);
    {
        const std::vector<std::string> wells = st.wells();

        for (const auto& assign : udq.assignments(UDQVarType::WELL_VAR)) {
            auto ws = assign.eval(wells);
            for (const auto& well : wells) {
                const auto& udq_value = ws[well];
                if (udq_value)
                    st.update_well_var(well, ws.name(), udq_value.value());
            }
        }

        for (const auto& def : udq.definitions(UDQVarType::WELL_VAR)) {
            auto ws = def.eval(context);
            for (const auto& well : wells) {
                const auto& udq_value = ws[well];
                if (udq_value)
                    st.update_well_var(well, def.keyword(), udq_value.value());
            }
        }
    }

    {
        const std::vector<std::string> groups = st.groups();

        for (const auto& assign : udq.assignments(UDQVarType::GROUP_VAR)) {
            auto ws = assign.eval(groups);
            for (const auto& group : groups) {
                const auto& udq_value = ws[group];
                if (udq_value)
                    st.update_group_var(group, ws.name(), udq_value.value());
            }
        }

        for (const auto& def : udq.definitions(UDQVarType::GROUP_VAR)) {
            auto ws = def.eval(context);
            for (const auto& group : groups) {
                const auto& udq_value = ws[group];
                if (udq_value)
                    st.update_group_var(group, def.keyword(), udq_value.value());
            }
        }
    }

    for (const auto& def : udq.definitions(UDQVarType::FIELD_VAR)) {
        auto field_udq = def.eval(context);
        if (field_udq[0])
            st.update(def.keyword(), field_udq[0].value());
    }
}

void updateValue(const Ewoms::SummaryNode& node, const double value, Ewoms::SummaryState& st)
{
    if (node.category() == Ewoms::SummaryNode::Category::Well)
        st.update_well_var(node.namedEntity(), node.keyword(), value);

    else if (node.category() == Ewoms::SummaryNode::Category::Group)
        st.update_group_var(node.namedEntity(), node.keyword(), value);

    else
        st.update(node.uniqueNodeKey(), value);
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

    void setFactors(const Ewoms::SummaryNode&        node,
                    const Ewoms::Schedule&           schedule,
                    const std::vector<Ewoms::Well>& schedule_wells,
                    const int                      sim_step);
};

void EfficiencyFactor::setFactors(const Ewoms::SummaryNode&        node,
                                  const Ewoms::Schedule&           schedule,
                                  const std::vector<Ewoms::Well>& schedule_wells,
                                  const int                      sim_step)
{
    this->factors.clear();

    if (schedule_wells.empty()) { return; }

    const auto cat = node.category();
    if(    cat != Ewoms::SummaryNode::Category::Group
        && cat != Ewoms::SummaryNode::Category::Field
        && cat != Ewoms::SummaryNode::Category::Region
           && (node.type() != Ewoms::SummaryNode::Type::Total))
        return;

    const bool is_group = (cat == Ewoms::SummaryNode::Category::Group);
    const bool is_rate = (node.type() != Ewoms::SummaryNode::Type::Total);

    for( const auto& well : schedule_wells ) {
        if (!well.hasBeenDefined(sim_step))
            continue;

        double eff_factor = well.getEfficiencyFactor();
        const auto* group_ptr = std::addressof(schedule.getGroup(well.groupName(), sim_step));

        while(true){
            if((   is_group
                && is_rate
                && group_ptr->name() == node.namedEntity() ))
                break;
            eff_factor *= group_ptr->getGroupEfficiencyFactor();

            if (group_ptr->name() == "FIELD")
                break;
            group_ptr = std::addressof( schedule.getGroup( group_ptr->parent(), sim_step ) );
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
    };

    struct SimulatorResults
    {
        const Ewoms::data::WellRates& wellSol;
        const std::map<std::string, double>& single;
        const std::map<std::string, std::vector<double>>& region;
        const std::map<std::pair<std::string, int>, double>& block;
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
        explicit FunctionRelation(Ewoms::SummaryNode node, ofun fcn)
            : node_(std::move(node))
            , fcn_ (std::move(fcn))
        {}

        void update(const std::size_t       sim_step,
                    const double            stepSize,
                    const InputData&        input,
                    const SimulatorResults& simRes,
                    Ewoms::SummaryState&      st) const override
        {
            const auto get_wells =
                need_wells(this->node_.category(), this->node_.keyword());

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

            const fn_args args {
                wells, stepSize, static_cast<int>(sim_step),
                std::max(0, this->node_.number()),
                st, simRes.wellSol, input.reg, input.grid,
                std::move(efac.factors)
            };

            const auto& usys = input.es.getUnits();
            const auto  prm  = this->fcn_(args);

            updateValue(this->node_, usys.from_si(prm.unit, prm.value), st);
        }

    private:
        Ewoms::SummaryNode node_;
        ofun             fcn_;
    };

    class BlockValue : public Base
    {
    public:
        explicit BlockValue(Ewoms::SummaryNode               node,
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
        Ewoms::SummaryNode node_;
        Ewoms::UnitSystem::measure m_;

        Ewoms::out::Summary::BlockValues::key_type lookupKey() const
        {
            return { this->node_.keyword(), this->node_.number() };
        }
    };

    class RegionValue : public Base
    {
    public:
        explicit RegionValue(Ewoms::SummaryNode               node,
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
            if (this->node_.number() < 0)
                return;

            auto xPos = simRes.region.find(this->node_.keyword());
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
        Ewoms::SummaryNode node_;
        Ewoms::UnitSystem::measure m_;

        std::vector<double>::size_type index() const
        {
            return this->node_.number() - 1;
        }
    };

    class GlobalProcessValue : public Base
    {
    public:
        explicit GlobalProcessValue(Ewoms::SummaryNode               node,
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
            auto xPos = simRes.single.find(this->node_.keyword());
            if (xPos == simRes.single.end())
                return;

            const auto  val  = xPos->second;
            const auto& usys = input.es.getUnits();

            updateValue(this->node_, usys.from_si(this->m_, val), st);
        }

    private:
        Ewoms::SummaryNode node_;
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

            st.update(this->saveKey_, convert::to(val, year));
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

        Descriptor create(const Ewoms::SummaryNode&);

    private:
        const Ewoms::EclipseState& es_;
        const Ewoms::EclipseGrid&  grid_;
        const Ewoms::SummaryState& st_;
        const Ewoms::UDQConfig&    udq_;

        const Ewoms::SummaryNode* node_;

        Ewoms::UnitSystem::measure paramUnit_;
        ofun paramFunction_;

        Descriptor functionRelation();
        Descriptor blockValue();
        Descriptor regionValue();
        Descriptor globalProcessValue();
        Descriptor userDefinedValue();
        Descriptor unknownParameter();

        bool isBlockValue();
        bool isRegionValue();
        bool isGlobalProcessValue();
        bool isFunctionRelation();
        bool isUserDefined();

        std::string functionUnitString() const;
        std::string directUnitString() const;
        std::string userDefinedUnit() const;
    };

    Factory::Descriptor Factory::create(const Ewoms::SummaryNode& node)
    {
        this->node_ = &node;

        if (this->isUserDefined())
            return this->userDefinedValue();

        if (this->isBlockValue())
            return this->blockValue();

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

        desc.uniquekey = this->node_->uniqueNodeKey();

        return desc;
    }

    bool Factory::isBlockValue()
    {
        auto pos = block_units.find(this->node_->keyword());
        if (pos == block_units.end())
            return false;

        if (! this->grid_.cellActive(this->node_->number() - 1))
            // 'node_' is a block value, but it is configured in a
            // deactivated cell.  Don't create an evaluation function.
            return false;

        // 'node_' represents a block value in an active cell.
        // Capture unit of measure and return true.
        this->paramUnit_ = pos->second;
        return true;
    }

    bool Factory::isRegionValue()
    {
        auto pos = region_units.find(this->node_->keyword());
        if (pos == region_units.end())
            return false;

        // 'node_' represents a region value.  Capture unit
        // of measure and return true.
        this->paramUnit_ = pos->second;
        return true;
    }

    bool Factory::isGlobalProcessValue()
    {
        auto pos = single_values_units.find(this->node_->keyword());
        if (pos == single_values_units.end())
            return false;

        // 'node_' represents a single value (i.e., global process)
        // value.  Capture unit of measure and return true.
        this->paramUnit_ = pos->second;
        return true;
    }

    bool Factory::isFunctionRelation()
    {
        auto pos = funs.find(this->node_->keyword());
        if (pos == funs.end())
            return false;

        // 'node_' represents a functional relation.
        // Capture evaluation function and return true.
        this->paramFunction_ = pos->second;
        return true;
    }

    bool Factory::isUserDefined()
    {
        return this->node_->isUserDefined();
    }

    std::string Factory::functionUnitString() const
    {
        const auto reg = Ewoms::out::RegionCache{};

        const fn_args args {
            {}, 0.0, 0, std::max(0, this->node_->number()),
            this->st_, {}, reg, this->grid_,
            {}
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
        const auto& kw = this->node_->keyword();

        return this->udq_.has_unit(kw)
            ?  this->udq_.unit(kw) : "?????";
    }
} // namespace Evaluator

void reportUnsupportedKeywords(std::vector<Ewoms::SummaryNode> keywords)
{
    std::sort(keywords.begin(), keywords.end());
    auto node = keywords.begin();
    auto uend = std::unique(keywords.begin(), keywords.end());

    for (; node != uend; ++node) {
        const auto& location = node->location();
        ::Ewoms::OpmLog::debug("Unhandled summary keyword '" + node->keyword() + "' at " + location.filename + ", line " + std::to_string(location.lineno));
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

    void eval(const EclipseState&            es,
              const Schedule&                sched,
              const int                      sim_step,
              const double                   duration,
              const data::WellRates&         well_solution,
              const GlobalProcessParameters& single_values,
              const RegionParameters&        region_values,
              const BlockValues&             block_values,
              SummaryState&                  st) const;

    void internal_store(const SummaryState& st, const int report_step);
    void write();

private:
    struct MiniStep
    {
        int id{0};
        int seq{-1};
        std::vector<float> params{};
    };

    using EvalPtr = SummaryOutputParameters::EvalPtr;

    std::reference_wrapper<const Ewoms::EclipseGrid> grid_;
    Ewoms::out::RegionCache regCache_;

    std::unique_ptr<SMSpecStreamDeferredCreation> deferredSMSpec_;

    Ewoms::EclIO::OutputStream::ResultSet rset_;
    Ewoms::EclIO::OutputStream::Formatted fmt_;
    Ewoms::EclIO::OutputStream::Unified   unif_;

    int miniStepID_{0};
    int prevCreate_{-1};
    int prevReportStepID_{-1};
    std::vector<MiniStep>::size_type numUnwritten_{0};

    SummaryOutputParameters  outputParameters_{};
    std::vector<EvalPtr>     requiredRestartParameters_{};
    std::vector<std::string> valueKeys_{};
    std::vector<MiniStep>    unwritten_{};

    std::unique_ptr<Ewoms::EclIO::OutputStream::SummarySpecification> smspec_{};
    std::unique_ptr<Ewoms::EclIO::EclOutput> stream_{};

    void configureTimeVectors(const EclipseState& es);

    void configureSummaryInput(const EclipseState&  es,
                               const SummaryConfig& sumcfg,
                               const EclipseGrid&   grid,
                               const Schedule&      sched);

    void configureRequiredRestartParameters(const SummaryConfig& sumcfg,
                                            const Schedule&      sched);

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
#ifdef ENABLE_3DPROPS_TESTING
    , regCache_      (es.fieldProps().get<int>("FIPNUM"), grid, sched)
#else
    , regCache_      (es.get3DProperties().getIntGridProperty("FIPNUM").compressedCopy(grid), grid, sched)
#endif
    , deferredSMSpec_(makeDeferredSMSpecCreation(es, grid, sched))
    , rset_          (makeResultSet(es.cfg().io(), basename))
    , fmt_           { es.cfg().io().getFMTOUT() }
    , unif_          { es.cfg().io().getUNIFOUT() }
{
    this->configureTimeVectors(es);
    this->configureSummaryInput(es, sumcfg, grid, sched);
    this->configureRequiredRestartParameters(sumcfg, sched);
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

void
Ewoms::out::Summary::SummaryImplementation::
eval(const EclipseState&            es,
     const Schedule&                sched,
     const int                      sim_step,
     const double                   duration,
     const data::WellRates&         well_solution,
     const GlobalProcessParameters& single_values,
     const RegionParameters&        region_values,
     const BlockValues&             block_values,
     Ewoms::SummaryState&             st) const
{
    const Evaluator::InputData input {
        es, sched, this->grid_, this->regCache_
    };

    const Evaluator::SimulatorResults simRes {
        well_solution, single_values, region_values, block_values
    };

    for (auto& evalPtr : this->outputParameters_.getEvaluators()) {
        evalPtr->update(sim_step, duration, input, simRes, st);
    }

    for (auto& evalPtr : this->requiredRestartParameters_) {
        evalPtr->update(sim_step, duration, input, simRes, st);
    }
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
configureTimeVectors(const EclipseState& es)
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

        const auto* utime = es.getUnits().name(UnitSystem::measure::time);
        auto eval = std::make_unique<Evaluator::Time>(this->valueKeys_.back());

        this->outputParameters_
            .makeParameter(kw, dfltwgname, dfltnum, utime, std::move(eval));
    }

#if NOT_YET
    // YEARS
    {
        const auto& kw = std::string("YEARS");
        makeKey(kw);

        auto eval = std::make_unique<Evaluator::Years>(this->valueKeys_.back());

        this->outputParameters_
            .makeParameter(kw, dfltwgname, dfltnum, kw, std::move(eval));
    }
#endif // NOT_YET
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

    auto unsuppkw = std::vector<SummaryNode>{};
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

void
Ewoms::out::Summary::SummaryImplementation::
configureRequiredRestartParameters(const SummaryConfig& sumcfg,
                                   const Schedule&      sched)
{
    auto makeEvaluator = [&sumcfg, this](const SummaryNode& node) -> void
    {
        if (sumcfg.hasSummaryKey(node.uniqueNodeKey()))
            // Handler already exists.  Don't add second evaluation.
            return;

        auto fcnPos = funs.find(node.keyword());
        assert ((fcnPos != funs.end()) &&
                "Internal error creating required restart vectors");

        auto eval = std::make_unique<
            Evaluator::FunctionRelation>(node, fcnPos->second);

        this->requiredRestartParameters_.push_back(std::move(eval));
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

namespace {

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

namespace Ewoms { namespace out {

Summary::Summary(const EclipseState&  es,
                 const SummaryConfig& sumcfg,
                 const EclipseGrid&   grid,
                 const Schedule&      sched,
                 const std::string&   basename)
    : pImpl_(new SummaryImplementation(es, sumcfg, grid, sched, basename))
{}

void Summary::eval(SummaryState&                  st,
                   const int                      report_step,
                   const double                   secs_elapsed,
                   const EclipseState&            es,
                   const Schedule&                schedule,
                   const data::WellRates&         well_solution,
                   const GlobalProcessParameters& single_values,
                   const RegionParameters&        region_values,
                   const BlockValues&             block_values) const
{
    validateElapsedTime(secs_elapsed, es, st);

    const double duration = secs_elapsed - st.get_elapsed();

    /* report_step is the number of the file we are about to write - i.e. for instance CASE.S$report_step
     * for the data in a non-unified summary file.
     * sim_step is the timestep which has been effective in the simulator, and as such is the value
     * necessary to use when consulting the Schedule object. */
    const auto sim_step = std::max( 0, report_step - 1 );

    this->pImpl_->eval(es, schedule, sim_step, duration,
                       well_solution, single_values,
                       region_values, block_values, st);

    eval_udq(schedule, sim_step, st);

    st.update_elapsed(duration);
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
