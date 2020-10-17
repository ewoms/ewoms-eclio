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
#ifndef GUIDE_RATE_CONFIG_H
#define GUIDE_RATE_CONFIG_H

#include <string>
#include <unordered_map>
#include <memory>

#include <ewoms/eclio/parser/eclipsestate/schedule/group/guideratemodel.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

namespace Ewoms {

class GuideRateConfig {
public:

 struct WellTarget {
    double guide_rate;
    Well::GuideRateTarget target;
    double scaling_factor;

    bool operator==(const WellTarget& data) const {
        return guide_rate == data.guide_rate &&
               target == data.target &&
               scaling_factor == data.scaling_factor;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(guide_rate);
        serializer(target);
        serializer(scaling_factor);
    }
};

struct GroupTarget {
    double guide_rate;
    Group::GuideRateTarget target;

    bool operator==(const GroupTarget& data) const {
        return guide_rate == data.guide_rate &&
               target == data.target;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(guide_rate);
        serializer(target);
    }
};

    static GuideRateConfig serializeObject();

    const GuideRateModel& model() const;
    bool has_model() const;
    bool update_model(const GuideRateModel& model);
    void update_well(const Well& well);
    void update_group(const Group& group);
    const WellTarget& well(const std::string& well) const;
    const GroupTarget& group(const std::string& group) const;
    bool has_well(const std::string& well) const;
    bool has_group(const std::string& group) const;

    bool operator==(const GuideRateConfig& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(m_model);
        serializer.map(wells);
        serializer.map(groups);
    }

private:
    std::shared_ptr<GuideRateModel> m_model;
    std::unordered_map<std::string, WellTarget> wells;
    std::unordered_map<std::string, GroupTarget> groups;
};

}

#endif
