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

#ifndef EWOMS_OUTPUT_ECLIPSE_VECTOR_DOUBHEAD_H
#define EWOMS_OUTPUT_ECLIPSE_VECTOR_DOUBHEAD_H

#include <vector>

namespace Ewoms { namespace RestartIO { namespace Helpers { namespace VectorItems {

    // This is a subset of the items in src/ewoms/eclio/output/DoubHEAD.cpp .
    // Promote items from that list to this in order to make them public.
    enum doubhead : std::vector<double>::size_type {
        TsInit  = 1,             // Maximum Length of Next Timestep
        TsMaxz  = 2,             // Maximum Length of Timestep After Next
        TsMinz  = 3,             // Minumum Length of All Timesteps
        TsMchp  = 4,
        TsFMax  = 5,
        TsFMin  = 6,
        TsFcnv  = 7,
        TrgTTE  = 8,
        TrgCNV  = 9,
        TrgMBE  = 10,
        TrgLCV  = 11,
        XxxTTE  = 16,
        XxxCNV  = 17,
        XxxMBE  = 18,
        XxxLCV  = 19,
        XxxWFL  = 20,
        TrgDPR  = 82,
        TfDiff  = 83,
        DdpLim  = 84,
        DdsLim  = 85,
        GRpar_a = 87,     // Guiderate parameter A
        GRpar_b = 88,     // Guiderate parameter B
        GRpar_c = 89,     // Guiderate parameter C
        GRpar_d = 90,     // Guiderate parameter D
        GRpar_e = 91,     // Guiderate parameter E
        GRpar_f = 92,     // Guiderate parameter F
        GRpar_int = 97,     // Guiderate parameter delay interval
        ThrUPT  =  99,
        XxxDPR  = 100,
        TrgFIP  = 101,
        TrgSFT  = 102,
        GRpar_damp = 144,     // Guiderate parameter damping factor
        UdqPar_2 = 212,		// UDQPARAM item number 2 (Permitted range (+/-) of user-defined quantities)
        UdqPar_3 = 213,		// UDQPARAM item number 3 (Value given to undefined elements when outputting data)
        UdqPar_4 = 214,		// UDQPARAM item number 4 (fractional equality tolerance used in ==, <= etc. functions)
    };

}}}} // Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_DOUBHEAD_H
