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

#include <ewoms/eclio/parser/deck/udavalue.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/group/gconsale.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include "../eval_uda.hh"

namespace Ewoms {

GConSale::GConSale(const std::map<std::string,GCONSALEGroup>& group)
    : groups(group)
{}

bool GConSale::has(const std::string& name) const {
    return (groups.find(name) != groups.end());
}

const GConSale::GCONSALEGroup& GConSale::get(const std::string& name) const {

    auto it = groups.find(name);
    if (it == groups.end())
        throw std::invalid_argument("Current GConSale obj. does not contain '" + name + "'.");
    else
        return it->second;
}

const GConSale::GCONSALEGroupProp GConSale::get(const std::string& name, const SummaryState& st) const {

    GCONSALEGroupProp prop;
    const GConSale::GCONSALEGroup& group = this->get(name);
    prop.sales_target = UDA::eval_group_uda(group.sales_target, name, st, group.udq_undefined);
    prop.max_sales_rate = UDA::eval_group_uda(group.max_sales_rate, name, st, group.udq_undefined);
    prop.min_sales_rate = UDA::eval_group_uda(group.min_sales_rate, name, st, group.udq_undefined);
    prop.max_proc = group.max_proc;
    return prop;
}

GConSale::MaxProcedure GConSale::stringToProcedure(const std::string& str_proc) {

    if      (str_proc == "NONE") return MaxProcedure::NONE;
    else if (str_proc == "CON" ) return MaxProcedure::CON;
    else if (str_proc == "+CON") return MaxProcedure::CON_P;
    else if (str_proc == "WELL") return MaxProcedure::WELL;
    else if (str_proc == "PLUG") return MaxProcedure::PLUG;
    else if (str_proc == "RATE") return MaxProcedure::RATE;
    else if (str_proc == "MAXR") return MaxProcedure::MAXR;
    else if (str_proc == "END" ) return MaxProcedure::END;
    else
        throw std::invalid_argument(str_proc + "invalid argument to GConSake::stringToProcedure");

    return MaxProcedure::NONE;
}

void GConSale::add(const std::string& name, const UDAValue& sales_target, const UDAValue& max_rate, const UDAValue& min_rate, const std::string& procedure, double udq_undefined_arg, const UnitSystem& unit_system) {
    GConSale::GCONSALEGroup& group = groups[name];
    group.sales_target = sales_target;
    group.max_sales_rate = max_rate;
    group.min_sales_rate = min_rate;
    group.max_proc = stringToProcedure(procedure);
    group.udq_undefined = udq_undefined_arg;
    group.unit_system = unit_system;
}

size_t GConSale::size() const {
    return groups.size();
}

const std::map<std::string, GConSale::GCONSALEGroup>& GConSale::getGroups() const {
    return groups;
}

bool GConSale::operator==(const GConSale& data) const {
    return this->getGroups() == data.getGroups();
}

}
