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

#ifndef EWOMS_WRITE_INIT_H
#define EWOMS_WRITE_INIT_H

#include <map>
#include <string>
#include <vector>

namespace Ewoms {

    class EclipseGrid;
    class EclipseState;
    class NNC;
    class Schedule;

} // namespace Ewoms

namespace Ewoms { namespace data {

    class Solution;

}} // namespace Ewoms::data

namespace Ewoms { namespace EclIO { namespace OutputStream {

    class Init;

}}} // namespace Ewoms::EclIO::OutputStream

namespace Ewoms { namespace InitIO {

    void write(const ::Ewoms::EclipseState&              es,
               const ::Ewoms::EclipseGrid&               grid,
               const ::Ewoms::Schedule&                  schedule,
               const ::Ewoms::data::Solution&            simProps,
               std::map<std::string, std::vector<int>> int_data,
               const ::Ewoms::NNC&                       nnc,
               ::Ewoms::EclIO::OutputStream::Init&       initFile);

}} // namespace Ewoms::InitIO

#endif // EWOMS_WRITE_INIT_H
