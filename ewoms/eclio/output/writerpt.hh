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

#ifndef EWOMS_WRITE_RPT_H
#define EWOMS_WRITE_RPT_H

#include <ostream>

namespace Ewoms {

    class Schedule;

    namespace RptIO {

        void write_report(
            std::ostream&,
            const std::string& report,
            unsigned value,
            const Schedule& schedule,
            std::size_t time_step
        );

        namespace workers {

            void write_WELSPECS(std::ostream&, unsigned, const Schedule&, std::size_t);

}   }   }
#endif // EWOMS_WRITE_RPT_H
