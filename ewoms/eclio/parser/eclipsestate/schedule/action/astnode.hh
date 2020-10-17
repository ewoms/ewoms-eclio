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
#ifndef ASTNODE_H
#define ASTNODE_H

#include <ewoms/eclio/parser/eclipsestate/schedule/action/actioncontext.hh>

#include "actionvalue.hh"

namespace Ewoms {
namespace Action {

class ActionContext;
class WellSet;
class ASTNode {
public:

    ASTNode();
    ASTNode(TokenType type_arg);
    ASTNode(double value);
    ASTNode(TokenType type_arg, FuncType func_type_arg, const std::string& func_arg, const std::vector<std::string>& arg_list_arg);

    static ASTNode serializeObject();

    Action::Result eval(const Action::Context& context) const;
    Action::Value value(const Action::Context& context) const;
    TokenType type;
    FuncType func_type;
    void add_child(const ASTNode& child);
    size_t size() const;
    std::string func;

    bool operator==(const ASTNode& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(type);
        serializer(func_type);
        serializer(func);
        serializer(arg_list);
        serializer(number);
        serializer.vector(children);
    }

private:
    std::vector<std::string> arg_list;
    double number = 0.0;

    /*
      To have a member std::vector<ASTNode> inside the ASTNode class is
      supposedly borderline undefined behaviour; it compiles without warnings
      and works. Good for enough for me.
    */
    std::vector<ASTNode> children;
};
}
}
#endif
