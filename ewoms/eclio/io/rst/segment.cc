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

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;

namespace Ewoms {
namespace RestartIO {

RstSegment::RstSegment(const int * iseg, const double * rseg) :
    segment(iseg[VI::ISeg::SegNo]),
    outlet_segment(iseg[VI::ISeg::OutSeg]),
    branch(iseg[VI::ISeg::BranchNo]),
    segment_type(iseg[VI::ISeg::SegmentType]),
    icd_scaling_mode(iseg[VI::ISeg::ICDScalingMode]),
    icd_open_flag(iseg[VI::ISeg::ICDOpenShutFlag]),
    dist_outlet(rseg[VI::RSeg::DistOutlet]),
    outlet_dz(rseg[VI::RSeg::OutletDepthDiff]),
    diameter(rseg[VI::RSeg::SegDiam]),
    roughness(rseg[VI::RSeg::SegRough]),
    area(rseg[VI::RSeg::SegArea]),
    volume(rseg[VI::RSeg::SegVolume]),
    dist_bhp_ref(rseg[VI::RSeg::DistBHPRef]),
    bhp_ref_dz(rseg[VI::RSeg::DepthBHPRef]),
    total_flow(rseg[VI::RSeg::TotFlowRate]),
    water_flow_fraction(rseg[VI::RSeg::WatFlowFract]),
    gas_flow_fraction(rseg[VI::RSeg::GasFlowFract]),
    pressure(rseg[VI::RSeg::Pressure]),
    valve_length(rseg[VI::RSeg::ValveLength]),
    valve_area(rseg[VI::RSeg::ValveArea]),
    valve_flow_coeff(rseg[VI::RSeg::ValveFlowCoeff]),
    valve_max_area(rseg[VI::RSeg::ValveMaxArea]),
    base_strength(rseg[VI::RSeg::DeviceBaseStrength]),
    fluid_density(rseg[VI::RSeg::CalibrFluidDensity]),
    fluid_viscosity(rseg[VI::RSeg::CalibrFluidViscosity]),
    critical_water_fraction(rseg[VI::RSeg::CriticalWaterFraction]),
    transition_region_width(rseg[VI::RSeg::TransitionRegWidth]),
    max_emulsion_ratio(rseg[VI::RSeg::MaxEmulsionRatio]),
    max_valid_flow_rate(rseg[VI::RSeg::MaxValidFlowRate]),
    icd_length(rseg[VI::RSeg::ICDLength]),
    valve_area_fraction(rseg[VI::RSeg::ValveAreaFraction])
{
    if (iseg[VI::ISeg::InSegCurBranch] != 0)
        this->inflow_segments.push_back(iseg[VI::ISeg::InSegCurBranch]);
}

}
}
