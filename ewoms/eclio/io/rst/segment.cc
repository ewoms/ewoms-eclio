/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>

#include <ewoms/eclio/io/rst/segment.hh>
#include <ewoms/eclio/output/vectoritems/msw.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/msw/icd.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/msw/segment.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;

namespace Ewoms {

namespace {

template <typename T>
T from_ecl(int ecl_value);

template <>
Segment::SegmentType from_ecl(int int_type) {
    return Segment::type_from_int(int_type);
}

}

namespace RestartIO {

using M  = ::Ewoms::UnitSystem::measure;

namespace {
double area_to_si(const UnitSystem& unit_system, double raw_value) {
    return unit_system.to_si( M::length, unit_system.to_si( M::length, raw_value));
}
}

RstSegment::RstSegment(const ::Ewoms::UnitSystem& unit_system, const int * iseg, const double * rseg) :
    segment(                                                    iseg[VI::ISeg::SegNo]),
    outlet_segment(                                             iseg[VI::ISeg::OutSeg]),
    branch(                                                     iseg[VI::ISeg::BranchNo]),
    segment_type(                                               from_ecl<Segment::SegmentType>(iseg[VI::ISeg::SegmentType])),
    icd_scaling_mode(                                           iseg[VI::ISeg::ICDScalingMode]),
    icd_status(                                                 from_int<ICDStatus>(iseg[VI::ISeg::ICDOpenShutFlag])),
    dist_outlet(            unit_system.to_si(M::length,        rseg[VI::RSeg::DistOutlet])),
    outlet_dz(              unit_system.to_si(M::length,        rseg[VI::RSeg::OutletDepthDiff])),
    diameter(               unit_system.to_si(M::length,        rseg[VI::RSeg::SegDiam])),
    roughness(              unit_system.to_si(M::length,        rseg[VI::RSeg::SegRough])),
    area(                   area_to_si(unit_system,             rseg[VI::RSeg::SegArea])),
    volume(                 unit_system.to_si(M::volume,        rseg[VI::RSeg::SegVolume])),
    dist_bhp_ref(           unit_system.to_si(M::length,        rseg[VI::RSeg::DistBHPRef])),
    bhp_ref_dz(             unit_system.to_si(M::length,        rseg[VI::RSeg::DepthBHPRef])),
    total_flow(             unit_system.to_si(M::rate,          rseg[VI::RSeg::TotFlowRate])),
    water_flow_fraction(                                        rseg[VI::RSeg::WatFlowFract]),
    gas_flow_fraction(                                          rseg[VI::RSeg::GasFlowFract]),
    pressure(               unit_system.to_si(M::pressure,      rseg[VI::RSeg::Pressure])),
    valve_length(           unit_system.to_si(M::length,        rseg[VI::RSeg::ValveLength])),
    valve_area(             area_to_si( unit_system,            rseg[VI::RSeg::ValveArea])),
    valve_flow_coeff(                                           rseg[VI::RSeg::ValveFlowCoeff]),
    valve_max_area(         area_to_si(unit_system,             rseg[VI::RSeg::ValveMaxArea])),
    base_strength(          unit_system.to_si( M::icd_strength, rseg[VI::RSeg::DeviceBaseStrength])),
    fluid_density(          unit_system.to_si(M::density,       rseg[VI::RSeg::CalibrFluidDensity])),
    fluid_viscosity(        unit_system.to_si(M::viscosity,     rseg[VI::RSeg::CalibrFluidViscosity])),
    critical_water_fraction(                                    rseg[VI::RSeg::CriticalWaterFraction]),
    transition_region_width(unit_system.to_si(M::length,        rseg[VI::RSeg::TransitionRegWidth])),
    max_emulsion_ratio(                                         rseg[VI::RSeg::MaxEmulsionRatio]),
    max_valid_flow_rate(    unit_system.to_si(M::rate,          rseg[VI::RSeg::MaxValidFlowRate])),
    icd_length(             unit_system.to_si(M::rate,          rseg[VI::RSeg::ICDLength])),
    valve_area_fraction(                                        rseg[VI::RSeg::ValveAreaFraction])
{
    if (iseg[VI::ISeg::InSegCurBranch] != 0)
        this->inflow_segments.push_back(iseg[VI::ISeg::InSegCurBranch]);
}

}
}
