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
        TsInit = 1,             // Maximum Length of Next Timestep
        TsMaxz = 2,             // Maximum Length of Timestep After Next
        TsMinz = 3,             // Minumum Length of All Timesteps
	UdqPar_2 = 212,		// UDQPARAM item number 2 (Permitted range (+/-) of user-defined quantities)
	UdqPar_3 = 213,		// UDQPARAM item number 3 (Value given to undefined elements when outputting data)
	UdqPar_4 = 214,		// UDQPARAM item number 4 (fractional equality tolerance used in ==, <= etc. functions)
    };

}}}} // Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_DOUBHEAD_H
