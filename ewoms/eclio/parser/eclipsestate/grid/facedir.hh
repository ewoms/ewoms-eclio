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

#ifndef EWOMS_FACEDIR_H
#define EWOMS_FACEDIR_H

#include <string>

namespace Ewoms {

    namespace FaceDir {

        enum DirEnum {
            XPlus  = 1,
            XMinus = 2,
            YPlus  = 4,
            YMinus = 8,
            ZPlus  = 16,
            ZMinus = 32
        };
        /**
           The MULTREGTScanner will use these values as bitmaps;
           i.e. it is essential they form a 2^n sequence.
        */

        DirEnum FromString(const std::string& stringValue);
        int     FromMULTREGTString(const std::string& stringValue);
    }
}

#endif