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

#include <stdexcept>
#include <ewoms/eclio/parser/eclipsestate/schedule/msw/icd.hh>

#include "icd_convert.hh"

namespace Ewoms {

template <>
ICDStatus from_int(int int_status) {
    switch (int_status) {
    case 0:
        return ICDStatus::OPEN;
    case 1:
        return ICDStatus::SHUT;
    default:
        throw std::invalid_argument("Invalid status value");
    }
}

template <>
int to_int(ICDStatus status) {
    switch (status) {
    case ICDStatus::OPEN:
        return 0;
    case ICDStatus::SHUT:
        return 1;
    default:
        throw std::invalid_argument("Invalid status value");
    }
}

}
