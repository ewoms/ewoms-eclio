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

#ifndef EWOMS_OUTPUT_ECLIPSE_VECTOR_TABDIMS_H
#define EWOMS_OUTPUT_ECLIPSE_VECTOR_TABDIMS_H

#include <vector>

namespace Ewoms { namespace RestartIO { namespace Helpers { namespace VectorItems {

    namespace TabDims {
        enum index : std::vector<int>::size_type {
            // Number of elements in 'TAB' array
            TabSize           = 0,

            // Oil PVT table
            PvtoMainStart     =  6,
            PvtoCompStart     =  7,
            NumPvtoCompNodes  =  8,
            NumPvtoPressNodes =  9,
            NumPvtoTables     = 10,

            // Water PVT table
            PvtwStart         = 11,
            NumPvtwTables     = 12,

            // Gas PVT tables
            PvtgMainStart     = 13,
            PvtgPressStart    = 14,
            NumPvtgCompNodes  = 15,
            NumPvtgPressNodes = 16,
            NumPvtgTables     = 17,

            // Density tables
            DensityTableStart = 18,
            DensityNumTables  = 19,

            // SWFN tables
            SwfnTableStart    = 20,
            SwfnNumSatNodes   = 21,
            SwfnNumTables     = 22,

            // SGFN tables
            SgfnTableStart    = 23,
            SgfnNumSatNodes   = 24,
            SgfnNumTables     = 25,

            // SOFN tables
            SofnTableStart    = 26,
            SofnNumSatNodes   = 28,
            SofnNumTables     = 29,

            // Size of TABDIMS array
            TabDimsNumElems  = 100,
        };
    } // namespace TabDims

}}}} // namespace Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_TABDIMS_H
