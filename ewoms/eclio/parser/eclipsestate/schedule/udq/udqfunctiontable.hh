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

#ifndef UDQFUNCTIONTABLE_H
#define UDQFUNCTIONTABLE_H

#include <unordered_map>
#include <memory>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqfunction.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>

namespace Ewoms {

class UDQFunctionTable {
public:
    explicit UDQFunctionTable(const UDQParams& params);
    UDQFunctionTable();
    bool has_function(const std::string& name) const;
    const UDQFunction& get(const std::string& name) const;
private:
    void insert_function(std::shared_ptr<const UDQFunction> func);
    UDQParams params;
    std::unordered_map<std::string, std::shared_ptr<const UDQFunction>> function_table;
};
}
#endif
