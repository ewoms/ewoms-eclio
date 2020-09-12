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

#ifndef UDQSTATE_HH_
#define UDQSTATE_HH_

#include <string>
#include <unordered_map>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqset.hh>

namespace Ewoms {
class UDQState {
public:
    UDQState(double undefined);

    bool has(const std::string& key) const;
    bool has_well_var(const std::string& well, const std::string& key) const;
    bool has_group_var(const std::string& group, const std::string& key) const;

    double get(const std::string& key) const;
    double get_group_var(const std::string& well, const std::string& var) const;
    double get_well_var(const std::string& well, const std::string& var) const;
    void add_define(const std::string& udq_key, const UDQSet& result);
    void add_assign(std::size_t report_step, const std::string& udq_key, const UDQSet& result);
    bool assign(std::size_t report_step, const std::string& udq_key) const;

    std::vector<char> serialize() const;
    void deserialize(const std::vector<char>& buffer);
    bool operator==(const UDQState& other) const;
private:
    void add(const std::string& udq_key, const UDQSet& result);
    double get_wg_var(const std::string& well, const std::string& key, UDQVarType var_type) const;
    double undefined_value;
    std::unordered_map<std::string, UDQSet> values;
    std::unordered_map<std::string, std::size_t> assignments;
};
}

#endif
