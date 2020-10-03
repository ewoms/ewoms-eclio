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

#include <utility>

#include <ewoms/common/fmt/format.h>
#include <ewoms/eclio/utility/opminputerror.hh>

namespace Ewoms {

std::string OpmInputError::formatException(const KeywordLocation& loc, const std::exception& e) {
    const std::string defaultMessage { R"(Problem parsing keyword {{keyword}}
In {{file}} line {{line}}.
Internal error: {})" } ;

    return format(fmt::format(defaultMessage, e.what()), loc);
}

/*
  For the format() function it is possible to have an alternative function with
  a variaditic template which can be forwarded directly to the fmt::format()
  function, that is an elegant way to pass arbitrary additional arguments. That
  will require the OpmInputError::format() to become a templated function and
  the fmtlib dependendcy will be imposed on downstream modules.
*/
std::string OpmInputError::format(const std::string& msg_format, const KeywordLocation& loc) {
    return fmt::format(msg_format,
                       fmt::arg("keyword", loc.keyword),
                       fmt::arg("file", loc.filename),
                       fmt::arg("line", loc.lineno)
                       );
}

}
