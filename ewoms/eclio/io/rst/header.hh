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

#ifndef RST_H
#define RST_H

#include <vector>
namespace Ewoms {
namespace RestartIO {

struct Header {
    Header(const std::vector<int>& intehead, const std::vector<bool>& logihead, const std::vector<double>& doubhead);

    int nx;
    int ny;
    int nz;
    int nactive;
    int num_wells;
    int max_perf;
    int max_wells_in_group;
    int max_groups_in_field;
    int max_wells_in_field;
    int year;
    int month;
    int mday;
    int hour;
    int minute;
    int microsecond;
    int phase_sum;
    int niwelz;
    int nswelz;
    int nxwelz;
    int nzwelz;
    int niconz;
    int nsconz;
    int nxconz;
    int nigrpz;
    int nsgrpz;
    int nxgrpz;
    int nzgrpz;
    int ncamax;
    int niaaqz;
    int nsaaqz;
    int nxaaqz;
    int nicaqz;
    int nscaqz;
    int nacaqz;
    int tstep;
    int report_step;
    int newtmx;
    int newtmn;
    int litmax;
    int litmin;
    int mxwsit;
    int mxwpit;
    int version;
    int iprog;
    int nsegwl;
    int nswlmx;
    int nsegmx;
    int nlbrmx;
    int nisegz;
    int nrsegz;
    int nilbrz;
    int ntfip ;
    int nmfipr;
    int nrfreg;
    int ntfreg;
    int nplmix;
    int ngroup;

    bool e300_radial;
    bool e100_radial;
    bool enable_hysteris;
    bool enable_msw;
    bool is_live_oil;
    bool is_wet_gas;
    bool const_comp_oil;
    bool dir_relperm;
    bool reversible_relperm;
    bool endscale;
    bool dir_eps;
    bool reversible_eps;
    bool alt_eps;

    double next_timestep1;
    double next_timestep2;
    double max_timestep;
    double guide_rate_a;
    double guide_rate_b;
    double guide_rate_c;
    double guide_rate_d;
    double guide_rate_e;
    double guide_rate_f;
    double udq_range;
    double udq_undefined;
    double udq_eps;
};

}
}

#endif
