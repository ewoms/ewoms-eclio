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

#ifndef UDQASTNODE_H
#define UDQASTNODE_H

#include <string>
#include <set>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqset.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqcontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

class UDQASTNode {
public:
    UDQASTNode(UDQTokenType type_arg);
    UDQASTNode(double scalar_value);
    UDQASTNode(UDQTokenType type_arg, const std::string& string_value, const std::vector<std::string>& selector);
    UDQASTNode(UDQTokenType type_arg, const std::string& func_name, const UDQASTNode& arg);
    UDQASTNode(UDQTokenType type_arg, const std::string& func_name, const UDQASTNode& left, const UDQASTNode& right);

    UDQSet eval(UDQVarType eval_target, const UDQContext& context) const;

    UDQTokenType type;
    UDQVarType var_type = UDQVarType::NONE;
    std::set<UDQTokenType> func_tokens() const;
private:
    void func_tokens(std::set<UDQTokenType>& tokens) const;

    std::string string_value;
    double scalar_value;
    std::vector<std::string> selector;
    std::vector<UDQASTNode> arglist;
};

}

#endif
