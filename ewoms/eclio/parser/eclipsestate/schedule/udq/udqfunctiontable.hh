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
#ifndef UDQFUNCTIONTABLE_H
#define UDQFUNCTIONTABLE_H

#include <unordered_map>
#include <memory>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqfunction.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>

namespace Ewoms {

class UDQFunctionTable {
public:
    using FunctionMap = std::unordered_map<std::string,
                                           std::shared_ptr<UDQFunction>>;
    explicit UDQFunctionTable(const UDQParams& params);
    UDQFunctionTable();
    UDQFunctionTable(const UDQParams& param,
                     const FunctionMap& map);
    bool has_function(const std::string& name) const;
    const UDQFunction& get(const std::string& name) const;

    const UDQParams& getParams() const;
    const FunctionMap& functionMap() const;

    bool operator==(const UDQFunctionTable& data) const;

private:
    void insert_function(std::shared_ptr<UDQFunction> func);
    UDQParams params;
    FunctionMap function_table;
};
}
#endif
