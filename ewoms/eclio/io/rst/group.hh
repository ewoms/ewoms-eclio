// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
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
#ifndef RST_GROUP
#define RST_GROUP

#include <array>
#include <vector>
#include <string>

namespace Ewoms {
class UnitSystem;

namespace RestartIO {

struct RstHeader;

struct RstGroup {
    RstGroup(const UnitSystem& unit_system,
             const RstHeader& header,
             const std::string* zwel,
             const int * igrp,
             const float * sgrp,
             const double * xgrp);

    std::string name;

    int parent_group;
    int prod_active_cmode;
    int gconprod_cmode;
    int winj_cmode;
    int ginj_cmode;
    int guide_rate_def;

    float oil_rate_limit;
    float water_rate_limit;
    float gas_rate_limit;
    float liquid_rate_limit;
    float water_surface_limit;
    float water_reservoir_limit;
    float water_reinject_limit;
    float water_voidage_limit;
    float gas_surface_limit;
    float gas_reservoir_limit;
    float gas_reinject_limit;
    float gas_voidage_limit;

    double oil_production_rate;
    double water_production_rate;
    double gas_production_rate;
    double liquid_production_rate;
    double water_injection_rate;
    double gas_injection_rate;
    double wct;
    double gor;
    double oil_production_total;
    double water_production_total;
    double gas_production_total;
    double voidage_production_total;
    double water_injection_total;
    double gas_injection_total;
    double voidage_injection_total;
    double oil_production_potential;
    double water_production_potential;
    double history_total_oil_production;
    double history_total_water_production;
    double history_total_water_injection;
    double history_total_gas_production;
    double history_total_gas_injection;
};

}
}

#endif
