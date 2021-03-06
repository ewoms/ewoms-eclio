// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
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

#include <ewoms/eclio/parser/eclipsestate/grid/pinchmode.hh>
#include <ewoms/common/string.hh>

#include <stdexcept>

namespace Ewoms {

    namespace PinchMode {

        const std::string PinchMode2String(const ModeEnum enumValue) {
            std::string stringValue;
            switch (enumValue) {
            case ModeEnum::ALL:
                stringValue = "ALL";
                break;

            case ModeEnum::TOPBOT:
                stringValue = "TOPBOT";
                break;

            case ModeEnum::TOP:
                stringValue = "TOP";
                break;

            case ModeEnum::GAP:
                stringValue = "GAP";
                break;
            case ModeEnum::NOGAP:
                stringValue = "NOGAP";
                break;
            }

            return stringValue;
        }

        ModeEnum PinchModeFromString(const std::string& stringValue) {
            std::string s = trim_copy(stringValue);

            ModeEnum mode;
            if      (s == "ALL")    { mode = ModeEnum::ALL;    }
            else if (s == "TOPBOT") { mode = ModeEnum::TOPBOT; }
            else if (s == "TOP")    { mode = ModeEnum::TOP;    }
            else if (s == "GAP")    { mode = ModeEnum::GAP;    }
            else if (s == "NOGAP")  { mode = ModeEnum::NOGAP;    }
            else {
                std::string msg = "Unsupported pinchout mode " + s;
                throw std::invalid_argument(msg);
            }

            return mode;
        }
    }
}
