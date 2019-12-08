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

#ifndef EWOMS_OUTPUT_ECLIPSE_VECTOR_MSW_H
#define EWOMS_OUTPUT_ECLIPSE_VECTOR_MSW_H

#include <vector>

namespace Ewoms { namespace RestartIO { namespace Helpers { namespace VectorItems {

    namespace ISeg {
        enum index : std::vector<int>::size_type {
            SegNo          = 0, // Segment number (one-based)
            OutSeg         = 1, // Outlet segment (one-based)
            InSegCurBranch = 2, // Inflow segment current branch (one-based)
            BranchNo       = 3, // Branch number (one-based)

            SegmentType    = 11,

            ICDScalingMode  = 18,
            ICDOpenShutFlag = 19,
        };

        namespace Value {
            enum SegmentType : int {
                AICD  = -8,
                SICD  = -7,
                Valve = -5,
            };
        } // Value
    } // ISeg

    namespace RSeg {
        enum index : std::vector<double>::size_type {
            DistOutlet      = 0, // Segment's distance to outlet
            OutletDepthDiff = 1, // Segment's depth differential to outlet
            SegDiam         = 2, // Internal diameter of segment
            SegRough        = 3, // Roughness parameter of segment
            SegArea         = 4, // Cross-sectional area of segment
            SegVolume       = 5, // Physical volume of segment
            DistBHPRef      = 6, // Segment's distance to BHP reference node
            DepthBHPRef     = 7, // Segment's depth differential to BHP ref. node

            TotFlowRate  =  8,  // Normalised total segment flow rate
            WatFlowFract =  9,  // Normalised Water flow rate fraction
            GasFlowFract = 10,  // Normalised Gas flow rate fraction
            Pressure     = 11,  // Segment pressure

            item40  =  39,      // Unknown

            ValveLength    = 40, // Length of valve
            ValveArea      = 41, // Cross-sectional area of valve
            ValveEFlowCoeff = 42, // Valve's dimensionless flow coefficient
            ValveMaxArea   = 43, // Maximal cross-sectional valve area

            DeviceBaseStrength = 86,

            CalibrFluidDensity   = 88,
            CalibrFluidViscosity = 89,

            CriticalWaterFraction = 90,
            TransitionRegWidth    = 91,
            MaxEmulsionRatio      = 92,
            MaxValidEFlowRate      = 97,

            ICDLength = 102,

            ValveAreaFraction  = 103,

            item106 = 105,      // Unknown
            item107 = 106,      // Unknown
            item108 = 107,      // Unknown
            item109 = 108,      // Unknown
            item110 = 109,      // Unknown
            item111 = 110,      // Unknown
        };
    } // RSeg

}}}} // Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_MSW_H
