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

#include <sstream>
#include <stdexcept>
#include <ewoms/eclio/opmlog/logutil.hh>

namespace Ewoms {

namespace Log {

    bool isPower2(int64_t x) {
        return ((x != 0) && !(x & (x - 1)));
    }

    std::string fileMessage(const Location& location, const std::string& message) {
        std::ostringstream oss;

        oss << message << "\n" << "In file " << location.filename << ", line " << location.lineno << "\n";

        return oss.str();
    }

    std::string fileMessage(int64_t messageType , const Location& location, const std::string& message) {
        return fileMessage( location , prefixMessage( messageType , message ));
    }

    std::string prefixMessage(int64_t messageType, const std::string& message) {
        std::string prefix;
        switch (messageType) {
        case MessageType::Debug:
            prefix = "Debug";
            break;
        case MessageType::Note:
            prefix = "Note";
            break;
        case MessageType::Info:
            prefix = "Info";
            break;
        case MessageType::Warning:
            prefix = "Warning";
            break;
        case MessageType::Error:
            prefix = "Error";
            break;
        case MessageType::Problem:
            prefix = "Problem";
            break;
        case MessageType::Bug:
            prefix = "Bug";
            break;
        default:
            throw std::invalid_argument("Unhandled messagetype");
        }

        return prefix + ": " + message;
    }

    std::string colorCodeMessage(int64_t messageType, const std::string& message) {
        switch (messageType) {
        case MessageType::Debug:
        case MessageType::Note:
        case MessageType::Info:
            return message; // No color coding, not even the code for default color.
        case MessageType::Warning:
            return AnsiTerminalColors::blue_strong + message + AnsiTerminalColors::none;
        case MessageType::Problem:
            return AnsiTerminalColors::magenta_strong + message + AnsiTerminalColors::none;
        case MessageType::Error:
        case MessageType::Bug:
            return AnsiTerminalColors::red_strong + message + AnsiTerminalColors::none;
        default:
            throw std::invalid_argument("Unhandled messagetype");
        }
    }

}
}
