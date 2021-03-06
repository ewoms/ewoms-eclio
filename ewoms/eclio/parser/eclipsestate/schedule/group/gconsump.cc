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
#include "config.h"

#include <ewoms/eclio/parser/eclipsestate/schedule/group/gconsump.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include "../eval_uda.hh"

namespace Ewoms {

GConSump GConSump::serializeObject()
{
    GConSump result;
    result.groups = {{"test1", {UDAValue(1.0), UDAValue(2.0), "test2", 3.0,
                                UnitSystem::serializeObject()}}};

    return result;
}

bool GConSump::has(const std::string& name) const {
    return (groups.find(name) != groups.end());
}

const GConSump::GCONSUMPGroup& GConSump::get(const std::string& name) const {

    auto it = groups.find(name);
    if (it == groups.end())
        throw std::invalid_argument("Current GConSump obj. does not contain '" + name + "'.");
    else
        return it->second;
}

const GConSump::GCONSUMPGroupProp GConSump::get(const std::string& name, const SummaryState& st) const {

    GCONSUMPGroupProp prop;
    const GConSump::GCONSUMPGroup& group = this->get(name);
    prop.consumption_rate = UDA::eval_group_uda(group.consumption_rate, name, st, group.udq_undefined);
    prop.import_rate = UDA::eval_group_uda(group.import_rate, name, st, group.udq_undefined);
    prop.network_node = group.network_node;
    return prop;
}

void GConSump::add(const std::string& name, const UDAValue& consumption_rate, const UDAValue& import_rate, const std::string network_node, double udq_undefined_arg, const UnitSystem& unit_system) {

    GConSump::GCONSUMPGroup& group = groups[name];

    group.consumption_rate = consumption_rate;
    group.import_rate = import_rate;
    group.network_node = network_node;
    group.udq_undefined = udq_undefined_arg;
    group.unit_system = unit_system;
}

size_t GConSump::size() const {
    return groups.size();
}

bool GConSump::operator==(const GConSump& data) const {
    return this->groups == data.groups;
}

}
