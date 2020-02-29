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

#ifndef GUIDE_RATE_H
#define GUIDE_RATE_H

#include <string>
#include <cstddef>
#include <ctime>
#include <unordered_map>

#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>

namespace Ewoms {

class Schedule;
class GuideRate {
private:

struct GuideRateValue {
    GuideRateValue() = default;
    GuideRateValue(double t, double v, GuideRateModel::Target tg):
        sim_time(t),
        value(v),
        target(tg)
    {}

    bool operator==(const GuideRateValue& other) const {
        return (this->sim_time == other.sim_time &&
                this->value == other.value);
    }

    bool operator!=(const GuideRateValue& other) const {
        return !(*this == other);
    }

    double sim_time;
    double value;
    GuideRateModel::Target target;
};

struct Potential {
    Potential() = default;
    Potential(double op, double gp, double wp) :
        oil_pot(op),
        gas_pot(gp),
        wat_pot(wp)
    {}

    double eval(Group::GuideRateTarget target) const;
    double eval(Well::GuideRateTarget target) const;
    double eval(GuideRateModel::Target target) const;

    double oil_pot;
    double gas_pot;
    double wat_pot;
};

public:
    GuideRate(const Schedule& schedule);
    void   compute(const std::string& wgname, size_t report_step, double sim_time, double oil_pot, double gas_pot, double wat_pot);
    double get(const std::string& well, Well::GuideRateTarget target) const;
    double get(const std::string& group, Group::GuideRateTarget target) const;
    double get(const std::string& name, GuideRateModel::Target model_target) const;

private:
    void well_compute(const std::string& wgname, size_t report_step, double sim_time, double oil_pot, double gas_pot, double wat_pot);
    void group_compute(const std::string& wgname, size_t report_step, double sim_time, double oil_pot, double gas_pot, double wat_pot);
    double eval_form(const GuideRateModel& model, double oil_pot, double gas_pot, double wat_pot, const GuideRateValue * prev) const;
    double eval_group_pot() const;
    double eval_group_resvinj() const;

    std::unordered_map<std::string,GuideRateValue> values;
    std::unordered_map<std::string,Potential> potentials;
    const Schedule& schedule;
};

}

#endif
