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
#ifndef GUIDE_RATE_H
#define GUIDE_RATE_H

#include <cstddef>
#include <ctime>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>

#include <stddef.h>

#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/guideratemodel.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

namespace Ewoms {

class Schedule;
class GuideRate {

public:
// used for potentials and well rates
struct RateVector {
    RateVector () = default;
    RateVector (double orat, double grat, double wrat) :
        oil_rat(orat),
        gas_rat(grat),
        wat_rat(wrat)
    {}

    double eval(Group::GuideRateTarget target) const;
    double eval(Well::GuideRateTarget target) const;
    double eval(GuideRateModel::Target target) const;

    double oil_rat;
    double gas_rat;
    double wat_rat;
};

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

    double sim_time { std::numeric_limits<double>::lowest() };
    double value { std::numeric_limits<double>::lowest() };
    GuideRateModel::Target target { GuideRateModel::Target::NONE };
};

struct GRValState {
    GuideRateValue curr{};
    GuideRateValue prev{};
};

public:
    GuideRate(const Schedule& schedule);
    void   compute(const std::string& wgname, size_t report_step, double sim_time, double oil_pot, double gas_pot, double wat_pot);
    double get(const std::string& well, Well::GuideRateTarget target, const RateVector& rates) const;
    double get(const std::string& group, Group::GuideRateTarget target, const RateVector& rates) const;
    double get(const std::string& name, GuideRateModel::Target model_target, const RateVector& rates) const;
    bool has(const std::string& name) const;

private:
    void well_compute(const std::string& wgname, size_t report_step, double sim_time, double oil_pot, double gas_pot, double wat_pot);
    void group_compute(const std::string& wgname, size_t report_step, double sim_time, double oil_pot, double gas_pot, double wat_pot);
    double eval_form(const GuideRateModel& model, double oil_pot, double gas_pot, double wat_pot) const;
    double eval_group_pot() const;
    double eval_group_resvinj() const;

    void assign_grvalue(const std::string& wgname, const GuideRateModel& model, GuideRateValue&& value);
    double get_grvalue_result(const GRValState& gr) const;

    using GRValPtr = std::unique_ptr<GRValState>;

    std::unordered_map<std::string, GRValPtr> values;
    std::unordered_map<std::string, RateVector > potentials;
    const Schedule& schedule;
};

}

#endif
