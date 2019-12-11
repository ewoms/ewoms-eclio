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

#ifndef EWOMS_OUTPUT_ECLIPSE_VECTOR_AQUIFER_H
#define EWOMS_OUTPUT_ECLIPSE_VECTOR_AQUIFER_H

#include <vector>

namespace Ewoms { namespace RestartIO { namespace Helpers { namespace VectorItems {

    namespace IAnalyticAquifer {
        enum index : std::vector<int>::size_type {
            NumAquiferConn = 0,
            WatPropTable = 1,

            TypeRelated1 =  9,
            TypeRelated2 = 10,
        };
    } // IAnalyticAquifer

    namespace SAnalyticAquifer {
        enum index : std::vector<float>::size_type {
            Compressibility = 0,

            FetInitVol = 1,
            FetProdIndex = 2,
            FetTimeConstant = 3,

            CTRadius = 1,
            CTPermeability = 2,
            CTPorosity = 3,

            InitPressure = 4,
            DatumDepth = 5,

            CTThickness = 6,
            CTAngle = 7,
            CTWatMassDensity = 8,
            CTWatViscosity = 9,
        };
    } // SAnalyticAquifer

    namespace XAnalyticAquifer {
        enum index : std::vector<double>::size_type {
            EFlowRate   = 0,
            Pressure   = 1,  // Dynamic aquifer pressure
            ProdVolume = 2,  // Liquid volume produced from aquifer (into reservoir)
            TotalArea  = 3,
        };
    } // XAnalyticAquifer

}}}} // Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_AQUIFER_H
