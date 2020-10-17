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

#include <ewoms/common/fmt/format.h>
#include <ewoms/eclio/opmlog/infologger.hh>
#include <ewoms/eclio/opmlog/opmlog.hh>

namespace Ewoms {

InfoLogger::InfoLogger(const std::string& context_arg, std::size_t info_limit_arg):
    context(context_arg),
    info_limit(info_limit_arg)
{
    this->log_function = &OpmLog::info;
}

void InfoLogger::operator()(const std::string& msg) {
    if (this->info_count == this->info_limit) {
        OpmLog::info(fmt::format("Reporting limit reached for {} - see PRT file for additional messages", this->context));
        this->log_function = &OpmLog::note;
    }
    this->log_function(msg);
    this->info_count += 1;
}

}
