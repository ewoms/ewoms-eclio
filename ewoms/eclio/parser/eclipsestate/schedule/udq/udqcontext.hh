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

#ifndef UDQ_CONTEXT_H
#define UDQ_CONTEXT_H

#include <vector>
#include <string>
#include <unordered_map>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>

namespace Ewoms {
    class SummaryState;
    class UDQFunctionTable;

    class UDQContext{
    public:
        UDQContext(const UDQFunctionTable& udqft, const SummaryState& summary_state);
        double get(const std::string& key) const;
        bool has_well_var(const std::string& well, const std::string& var) const;
        double get_well_var(const std::string& well, const std::string& var) const;
        bool has_group_var(const std::string& group, const std::string& var) const;
        double get_group_var(const std::string& group, const std::string& var) const;
        void add(const std::string& key, double value);
        const UDQFunctionTable& function_table() const;
        std::vector<std::string> wells() const;
        std::vector<std::string> groups() const;
    private:
        const UDQFunctionTable& udqft;
        const SummaryState& summary_state;
        std::unordered_map<std::string, double> values;
    };
}

#endif
