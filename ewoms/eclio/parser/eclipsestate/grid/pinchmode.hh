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

#ifndef EWOMS_PINCHMODE_H
#define EWOMS_PINCHMODE_H

#include <string>

namespace Ewoms {

    namespace PinchMode {
        enum ModeEnum {
            ALL = 1,
            TOPBOT = 2,
            TOP = 3
        };

        const std::string PinchMode2String(const ModeEnum enumValue);
        ModeEnum PinchModeFromString(const std::string& stringValue);
    }
}

#endif // _PINCHMODE_
