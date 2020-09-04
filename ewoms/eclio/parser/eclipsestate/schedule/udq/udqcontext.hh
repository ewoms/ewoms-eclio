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

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqset.hh>

namespace Ewoms {
    class SummaryState;
    class UDQFunctionTable;
    class UDQState;

    class UDQContext{
    public:
        UDQContext(const UDQFunctionTable& udqft, SummaryState& summary_state, UDQState& udq_state);
        std::optional<double> get(const std::string& key) const;
        std::optional<double> get_well_var(const std::string& well, const std::string& var) const;
        std::optional<double> get_group_var(const std::string& group, const std::string& var) const;
        void add(const std::string& key, double value);
        void update(const std::string& keyword, const UDQSet& udq_result);
        const UDQFunctionTable& function_table() const;
        std::vector<std::string> wells() const;
        std::vector<std::string> groups() const;
    private:
        const UDQFunctionTable& udqft;
        SummaryState& summary_state;
        UDQState& udq_state;
        //std::unordered_map<std::string, UDQSet> udq_results;
        std::unordered_map<std::string, double> values;
    };
}

#endif
