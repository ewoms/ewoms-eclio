/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RST_WELL
#define RST_WELL

#include <array>
#include <vector>
#include <string>
#include <unordered_map>

#include <ewoms/eclio/io/rst/connection.hh>
#include <ewoms/eclio/io/rst/segment.hh>

namespace Ewoms {
class UnitSystem;

namespace RestartIO {

struct RstHeader;

struct RstWell {
    RstWell(const ::Ewoms::UnitSystem& unit_system,
            const RstHeader& header,
            const std::string& group_arg,
            const std::string* zwel,
            const int * iwel,
            const float * swel,
            const double * xwel,
            const int * icon,
            const float * scon,
            const double * xcon);

    RstWell(const ::Ewoms::UnitSystem& unit_system,
            const RstHeader& header,
            const std::string& group_arg,
            const std::string* zwel,
            const int * iwel,
            const float * swel,
            const double * xwel,
            const int * icon,
            const float * scon,
            const double * xcon,
            const std::vector<int>& iseg,
            const std::vector<double>& rseg);

    std::string name;
    std::string group;
    std::array<int, 2> ij;
    std::pair<int,int> k1k2;
    int wtype;
    int active_control;
    int vfp_table;
    int pred_requested_control;
    bool allow_xflow;
    int hist_requested_control;
    int msw_index;
    int completion_ordering;

    float orat_target;
    float wrat_target;
    float grat_target;
    float lrat_target;
    float resv_target;
    float thp_target;
    float bhp_target_float;
    float hist_lrat_target;
    float hist_grat_target;
    float hist_bhp_target;
    float datum_depth;

    double oil_rate;
    double water_rate;
    double gas_rate;
    double liquid_rate;
    double void_rate;
    double flow_bhp;
    double wct;
    double gor;
    double oil_total;
    double water_total;
    double gas_total;
    double void_total;
    double water_inj_total;
    double gas_inj_total;
    double gas_fvf;
    double bhp_target_double;
    double hist_oil_total;
    double hist_wat_total;
    double hist_gas_total;
    double hist_water_inj_total;
    double hist_gas_inj_total;
    double water_void_rate;
    double gas_void_rate;

    const RstSegment segment(int segment_number) const;
    std::vector<RstConnection> connections;
    std::vector<RstSegment> segments;
};

}
}

#endif
