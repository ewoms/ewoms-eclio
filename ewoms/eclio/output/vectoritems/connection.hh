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

#ifndef EWOMS_OUTPUT_ECLIPSE_VECTOR_CONNECTION_H
#define EWOMS_OUTPUT_ECLIPSE_VECTOR_CONNECTION_H

#include <vector>

namespace Ewoms { namespace RestartIO { namespace Helpers { namespace VectorItems {
    namespace IConn {
        enum index : std::vector<int>::size_type {
            SeqIndex   =  0, // Connection sequence index
            CellI      =  1, // I-location (1-based cell index) of connection
            CellJ      =  2, // J-location (1-based cell index) of connection
            CellK      =  3, // K-location (1-based cell index) of connection
            ConnStat   =  5, // Connection status.
                             //    > 0 => open, shut otherwise

            Drainage   =  6, // Saturation function (table ID) for drainage
            Imbibition =  9, // Saturation function (table ID) for imbibition

            ComplNum   = 12, // Completion ID (1-based)
            ConnDir    = 13, // Penetration direction (1:X, 2:Y, 3:Z)
            Segment    = 14, // Segment ID of connection
                             //   0 for regular connections, > 0 for MSW.
        };
    } // IConn

    namespace SConn {
        enum index : std::vector<float>::size_type {
            ConnTrans    =  0, // Connection transmissibility factor
            Depth        =  1, // Connection centre depth
            Diameter     =  2, // Connection diameter

            EffectiveKH  =  3, // Effective Kh product of connection

            item12       = 11, // Connection transmissibility factor

            SegDistEnd   = 20, // Distance to end of connection in segment
            SegDistStart = 21, // Distance to start of connection in segment

            item30       = 29, // Unknown
            item31       = 30, // Unknown
            item41       = 40, // = 0 for connection factor not defined, = 1 for connection factor defined
        };
    } // SConn

    namespace XConn {
        enum index : std::vector<double>::size_type {
            OilRate    =  0,  // Surface flow rate (oil)
            WaterRate  =  1,  // Surface flow rate (water)
            GasRate    =  2,  // Surface Flow rate (gas)

            Pressure   = 34,  // Connection pressure value

            ResVRate   = 49,  // Reservoir voidage rate
        };
    } // XConn
}}}} // Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_CONNECTION_H