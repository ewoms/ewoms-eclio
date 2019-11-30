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

#ifndef ACTION_PARSER_H
#define ACTION_PARSER_H

#include <vector>
#include <string>

#include "astnode.hh"
#include "actionvalue.hh"

namespace Ewoms {

namespace Action {

struct ParseNode {
    ParseNode(TokenType type_arg, const std::string& value_arg) :
        type(type_arg),
        value(value_arg)
    {}

    // Implicit converting constructor.
    ParseNode(TokenType type_arg) : ParseNode(type_arg, "")
    {}

    TokenType type;
    std::string value;
};

class Parser {
public:
    static Action::ASTNode parse(const std::vector<std::string>& tokens);
    static TokenType get_type(const std::string& arg);

private:
    explicit Parser(const std::vector<std::string>& tokens);

    ParseNode current() const;
    ParseNode next();
    size_t pos() const;
    Action::ASTNode parse_cmp();
    Action::ASTNode parse_op();
    Action::ASTNode parse_left();
    Action::ASTNode parse_right();
    Action::ASTNode parse_and();
    Action::ASTNode parse_or();

    const std::vector<std::string>& tokens;
    ssize_t current_pos = -1;
};

}
}
#endif
