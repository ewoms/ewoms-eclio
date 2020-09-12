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

#ifndef EWOMS_REGION_CACHE_H
#define EWOMS_REGION_CACHE_H

#include <map>
#include <set>
#include <vector>

namespace Ewoms {
    class Schedule;
    class EclipseGrid;
    class FieldPropsManager;

namespace out {
    class RegionCache {
    public:
        RegionCache() = default;
        RegionCache(const std::set<std::string>& fip_regions, const FieldPropsManager& fp, const EclipseGrid& grid, const Schedule& schedule);
        const std::vector<std::pair<std::string,size_t>>& connections( const std::string& region_name, int region_id ) const;

    private:
        std::vector<std::pair<std::string,size_t>> connections_empty;
        std::map<std::pair<std::string, int> , std::vector<std::pair<std::string,size_t>>> connection_map;
    };
}
}

#endif
