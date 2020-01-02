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
#include <memory>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqset.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqcontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

class UDQASTNode {
public:
    UDQASTNode();
    explicit UDQASTNode(UDQTokenType type_arg);
    explicit UDQASTNode(double scalar_value);
    UDQASTNode(UDQTokenType type_arg, const std::string& func_name, const UDQASTNode& arg);
    UDQASTNode(UDQTokenType type_arg, const std::string& func_name, const UDQASTNode& left, const UDQASTNode& right);
    UDQASTNode(UDQTokenType type_arg, const std::string& func_name);
    UDQASTNode(UDQTokenType type_arg, const std::string& string_value, const std::vector<std::string>& selector);
    UDQASTNode(UDQVarType varType, UDQTokenType typ,
               const std::string& stringVal,
               double scalarVal,
               const std::vector<std::string>& selectors,
               const std::shared_ptr<UDQASTNode>& left_arg,
               const std::shared_ptr<UDQASTNode>& right_arg);

    UDQSet eval(UDQVarType eval_target, const UDQContext& context) const;

    bool valid() const;
    UDQVarType var_type = UDQVarType::NONE;
    std::set<UDQTokenType> func_tokens() const;
    void update_type(const UDQASTNode& arg);
    void set_left(const UDQASTNode& arg);
    void set_right(const UDQASTNode& arg);
    UDQASTNode* get_left() const;
    UDQASTNode* get_right() const;

    bool operator==(const UDQASTNode& data) const;

    const std::string& stringValue() const;
    UDQTokenType getType() const;
    double scalarValue() const;
    const std::vector<std::string>& getSelectors() const;
    const std::shared_ptr<UDQASTNode>& getLeft() const;
    const std::shared_ptr<UDQASTNode>& getRight() const;

private:
    UDQTokenType type;
    void func_tokens(std::set<UDQTokenType>& tokens) const;

    std::string string_value;
    std::vector<std::string> selector;
    double scalar_value;
    std::shared_ptr<UDQASTNode> left;
    std::shared_ptr<UDQASTNode> right;
};

}

#endif
