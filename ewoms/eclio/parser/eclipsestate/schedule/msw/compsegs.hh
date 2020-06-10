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

#ifndef COMPSEGS_HH_
#define COMPSEGS_HH_

#include <unordered_map>
#include <vector>

namespace Ewoms {

    class Segment;
    class Connection;
    class WellConnections;
    class DeckKeyword;
    class WellSegments;
    class EclipseGrid;
    class ParseContext;
    class ErrorGuard;

namespace RestartIO {
    class RstWell;
}

namespace Compsegs {

/*
  The COMPSEGS keyword defines a link between connections and segments. This
  linking is circular because information about the segments is embedded in the
  connections, and visa versa. The function creates new WellSegments and
  WellConnections instances where the segment <--> connection linking has been
  established.
*/

    std::pair<WellConnections, WellSegments>
    processCOMPSEGS(const DeckKeyword& compsegs,
                    const WellConnections& input_connections,
                    const WellSegments& input_segments,
                    const EclipseGrid& grid,
                    const ParseContext& parseContext,
                    ErrorGuard& errors);

    std::pair<WellConnections, WellSegments>
    rstUpdate(const RestartIO::RstWell& rst_well,
              std::vector<Connection> input_connections,
              const std::unordered_map<int, Segment>& input_segments);
}
}

#endif /* COMPSEGS_HH_ */
