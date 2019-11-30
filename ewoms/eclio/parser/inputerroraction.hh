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

#ifndef EWOMS_INPUT_ERROR_ACTION_H
#define EWOMS_INPUT_ERROR_ACTION_H

namespace Ewoms {

    namespace InputError {

        /*
          The THROW_EXCEPTION and EXIT1 error handlers are overlapping, the
          intention os that the EXIT1 handler should be used in situations which
          are clearly user errors where an exception/traceback is of no use to
          the developers. The prototype of an error mode which should be handled
          with EXIT1 is PARSE_MISSING_INCLUDE.
        */

        enum Action {
            THROW_EXCEPTION = 0,
            WARN = 1,
            IGNORE = 2,
            EXIT1 = 3,
            DELAYED_EXIT1 = 4
        };
    }
}

#endif
