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

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqassign.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

UDQAssign::UDQAssign() :
    m_var_type(UDQVarType::NONE)
{
}

UDQAssign::UDQAssign(const std::string& keyword, const std::vector<std::string>& selector, double value) :
    m_keyword(keyword),
    m_var_type(UDQ::varType(keyword))
{
    this->add_record(selector, value);
}

UDQAssign UDQAssign::serializeObject()
{
    UDQAssign result;
    result.m_keyword = "test";
    result.m_var_type = UDQVarType::CONNECTION_VAR;
    result.records = {{{"test1"}, 1.0}};

    return result;
}

void UDQAssign::add_record(const std::vector<std::string>& selector, double value) {
    this->records.push_back({selector, value});
}

const std::string& UDQAssign::keyword() const {
    return this->m_keyword;
}

UDQVarType UDQAssign::var_type() const {
    return this->m_var_type;
}

UDQSet UDQAssign::eval(const std::vector<std::string>& wells) const {
    if (this->m_var_type == UDQVarType::WELL_VAR) {
        UDQSet ws = UDQSet::wells(this->m_keyword, wells);

        for (const auto& record : this->records) {
            const auto& selector = record.selector;
            double value = record.value;
            if (selector.empty())
                ws.assign(value);
            else
                ws.assign(selector[0], value);
        }

        return ws;
    }
    throw std::invalid_argument("Not yet implemented");
}

bool UDQAssign::operator==(const UDQAssign& data) const {
    return this->keyword() == data.keyword() &&
           this->var_type() == data.var_type() &&
           this->records == data.records;
}

}
