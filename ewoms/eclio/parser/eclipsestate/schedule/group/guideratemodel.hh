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

#ifndef GUIDE_RATE_MODEL_H
#define GUIDE_RATE_MODEL_H

#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

namespace Ewoms {

class GuideRateModel {
public:

    enum class Target {
        OIL = 0,
        LIQ = 1,
        GAS = 2,
        WAT = 3,
        RES = 4,
        COMB = 5,
        NONE = 6
    };

    static Target TargetFromString(const std::string& s);

    GuideRateModel(double time_interval_arg,
                   Target target_arg,
                   double A_arg,
                   double B_arg,
                   double C_arg,
                   double D_arg,
                   double E_arg,
                   double F_arg,
                   bool allow_increase_arg,
                   double damping_factor_arg,
                   bool use_free_gas_arg);

    GuideRateModel() = default;
    bool updateLINCOM(const UDAValue& alpha, const UDAValue& beta, const UDAValue& gamma);
    double eval(double oil_pot, double gas_pot, double wat_pot) const;
    double update_delay() const;
    bool allow_increase() const;
    double damping_factor() const;
    bool operator==(const GuideRateModel& other) const;
    bool operator!=(const GuideRateModel& other) const;
    Target target() const;
    double getA() const;
    double getB() const;
    double getC() const;
    double getD() const;
    double getE() const;
    double getF() const;

    static Target convert_target(Group::GuideRateTarget group_target);
    static Target convert_target(Well::GuideRateTarget well_target);
    static double pot(Target target, double oil_pot, double gas_pot, double wat_pot);
private:
    double pot(double oil_pot, double gas_pot, double wat_pot) const;
    /*
      Unfortunately the default values will give a GuideRateModel which can not
      be evaluated, due to a division by zero problem.
    */
    double time_interval = 0;
    Target m_target = Target::NONE;
    double A = 0;
    double B = 0;
    double C = 0;
    double D = 0;
    double E = 0;
    double F = 0;
    bool allow_increase_ = true;
    double damping_factor_ = 1.0;
    bool use_free_gas = false;
    bool default_model = true;
    UDAValue alpha;
    UDAValue beta;
    UDAValue gamma;
};

}

#endif
