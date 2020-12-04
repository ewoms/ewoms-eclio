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
#include "config.h"

#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <cstdlib>

#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionast.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actioncontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionvalue.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/astnode.hh>

#include "actionparser.hh"

namespace Ewoms {
namespace Action {

AST::AST(const std::vector<std::string>& tokens) {
    auto condition_node = Action::Parser::parse(tokens);
    this->condition.reset( new Action::ASTNode(condition_node) );
}

AST AST::serializeObject()
{
    AST result;
    result.condition = std::make_shared<ASTNode>(ASTNode::serializeObject());

    return result;
}

Action::Result AST::eval(const Action::Context& context) const {
    if (this->condition)
        return this->condition->eval(context);
    else
        return Action::Result(false);
}

bool AST::operator==(const AST& data) const {
    if ((condition && !data.condition) ||
        (!condition && data.condition))
        return false;

    return !condition || (*condition == *data.condition);
}

void AST::required_summary(std::unordered_set<std::string>& required_summary) const {
    this->condition->required_summary(required_summary);
}

}
}
