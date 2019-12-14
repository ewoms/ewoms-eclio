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

#include <iostream>
#include <cstring>

#include <ewoms/eclio/parser/parsecontext.hh>
#include "udqparser.hh"

namespace Ewoms {

UDQTokenType UDQParser::get_type(const std::string& arg) const {
    auto func_type = UDQ::funcType(arg);
    if (func_type == UDQTokenType::table_lookup)
        throw std::invalid_argument("Table lookup function TU*[] is not supported in UDQ");

    if (func_type != UDQTokenType::error)
        return func_type;

    if (arg == "(")
        return UDQTokenType::open_paren;

    if (arg == ")")
        return UDQTokenType::close_paren;

    {
        char * end_ptr;
        std::strtod(arg.c_str(), &end_ptr);
        if (std::strlen(end_ptr) == 0)
            return UDQTokenType::number;
    }

    return UDQTokenType::ecl_expr;
}

std::size_t UDQParser::current_size() const {
    if (this->tokens.size() == static_cast<std::size_t>(this->current_pos))
        return 0;

    if (this->current_pos < 0)
        return 1;

    const std::string& first_arg = this->tokens[this->current_pos];
    if (this->get_type(first_arg) != UDQTokenType::ecl_expr)
        return 1;

    std::size_t offset = this->current_pos;
    while (true) {
        const std::string& arg = this->tokens[offset];
        if (this->get_type(arg) != UDQTokenType::ecl_expr)
            break;

        offset += 1;

        if (offset == this->tokens.size())
            break;
    }

    return offset - this->current_pos;
}

UDQParseNode UDQParser::next() {
    this->current_pos += this->current_size();
    return this->current();
}

UDQParseNode UDQParser::current() const {
    if (static_cast<size_t>(this->current_pos) == this->tokens.size())
        return UDQTokenType::end;

    const std::string& arg = this->tokens[this->current_pos];
    auto type = this->get_type(arg);
    if (type != UDQTokenType::ecl_expr)
        return UDQParseNode(type, arg);

    std::size_t selector_size = this->current_size() - 1;
    std::vector<std::string> selector;
    if (selector_size > 0) {
        const auto * token_ptr = std::addressof(this->tokens[this->current_pos + 1]);
        selector.assign( token_ptr, token_ptr + selector_size);
    }
    return UDQParseNode(type, arg, selector);
}

UDQASTNode UDQParser::parse_factor() {
    auto current = this->current();

    if (current.type == UDQTokenType::open_paren) {
        this->next();
        auto inner_expr = this->parse_cmp();

        current = this->current();
        if (current.type != UDQTokenType::close_paren)
            return UDQTokenType::error;

        this->next();
        return inner_expr;
    }

    if (UDQ::scalarFunc(current.type) || UDQ::elementalUnaryFunc(current.type)) {
        auto func_node = current;
        auto next = this->next();
        if (next.type == UDQTokenType::open_paren) {
            this->next();
            auto arg_expr = this->parse_cmp();

            current = this->current();
            if (current.type != UDQTokenType::close_paren)
                return UDQTokenType::error;

            this->next();
            return UDQASTNode(func_node.type, func_node.value, arg_expr);
        } else
            return UDQTokenType::error;
    }

    UDQASTNode node(current.type, current.value, current.selector);
    this->next();
    return node;
}

UDQASTNode UDQParser::parse_pow() {
    auto left = this->parse_factor();
    auto current = this->current();
    if (current.type == UDQTokenType::end)
        return left;

    if (current.type == UDQTokenType::binary_op_pow) {
        auto func_node = current;
        this->next();
        auto right = this->parse_mul();
        if (right.type == UDQTokenType::end)
            return UDQASTNode(UDQTokenType::error);

        return UDQASTNode(current.type, current.value, left, right);
    }

    return left;
}

UDQASTNode UDQParser::parse_mul() {
    auto left = this->parse_pow();
    auto current = this->current();
    if (current.type == UDQTokenType::end)
        return left;

    if (current.type == UDQTokenType::binary_op_mul || current.type == UDQTokenType::binary_op_div) {
        auto func_node = current;
        this->next();
        auto right = this->parse_mul();
        if (right.type == UDQTokenType::end)
            return UDQASTNode(UDQTokenType::error);

        return UDQASTNode(current.type, current.value, left, right);
    }

    return left;
}

UDQASTNode UDQParser::parse_add() {
    auto left = this->parse_mul();
    auto current = this->current();
    if (current.type == UDQTokenType::end)
        return left;

    if (current.type == UDQTokenType::binary_op_add || current.type == UDQTokenType::binary_op_sub) {
        auto func_node = current;
        auto next = this->next();
        auto right = this->parse_add();
        if (right.type == UDQTokenType::end)
            return UDQASTNode(UDQTokenType::error);

        return UDQASTNode(current.type, current.value, left, right);
    }
    return left;
}

/*
  A bit uncertain on the presedence of the comparison operators. In normal C the
  comparion operators bind weaker than addition, i.e. for the assignment:

     auto cmp = a + b < c;

  The sum (a+b) is evaluated and then compared with c, that is the order of
  presedence implemented here. But reading the eclipse UDQ manual one can get
  the imporession that the relation operators should bind "very strong", i.e.
  that (b < c) should be evaluated first, and then the result of the comparison
  added to a.
*/

UDQASTNode UDQParser::parse_cmp() {
    auto left = this->parse_add();
    auto current = this->current();
    if (current.type == UDQTokenType::end)
        return left;

    if (UDQ::cmpFunc(current.type)) {
        auto func_node = current;
        this->next();
        auto right = this->parse_cmp();
        if (right.type == UDQTokenType::end)
            return UDQASTNode(UDQTokenType::error);

        return UDQASTNode(current.type, current.value, left, right);
    }
    return left;
}

namespace {
    void dump_tokens(const std::string& target_var, const std::vector<std::string>& tokens) {
        std::cout << target_var << " = ";
        for (const auto& token : tokens)
            std::cout << token << " ";
        std::cout << std::endl;
    }

/*
  This function is extremely weak - hopefully it can be improved in the future.
  See the comment in UDQEnums.hpp about 'UDQ type system'.
*/
bool static_type_check(UDQVarType lhs, UDQVarType rhs) {
    if (lhs == rhs)
        return true;

    if (rhs == UDQVarType::SCALAR)
        return true;

    /*
      This does not check if the rhs evaluates to a scalar.
    */
    if (rhs == UDQVarType::WELL_VAR)
        return (lhs == UDQVarType::WELL_VAR);

    return true;
}
}

UDQASTNode UDQParser::parse(const UDQParams& udq_params, UDQVarType target_type, const std::string& target_var, const std::vector<std::string>& tokens, const ParseContext& parseContext, ErrorGuard& errors)
{
    UDQParser parser(udq_params, target_type, tokens);
    parser.next();
    auto tree = parser.parse_cmp();
    auto current = parser.current();

    if (current.type != UDQTokenType::end || tree.type == UDQTokenType::error) {
        if (current.type != UDQTokenType::end) {
            size_t index = parser.current_pos;
            std::string msg = "Extra unhandled data starting with token[" + std::to_string(index) + "] = " + current.value;
            parseContext.handleError(ParseContext::UDQ_PARSE_ERROR, msg, errors);
        }

        if (tree.type == UDQTokenType::error) {
            std::string msg = "Failed to parse UDQ expression";
            parseContext.handleError(ParseContext::UDQ_PARSE_ERROR, msg, errors);
        }
        return UDQASTNode( udq_params.undefinedValue() );
    }

    if (!static_type_check(target_type, tree.var_type)) {
        std::string msg = "Invalid compile-time type conversion detected in UDQ expression target type: " + UDQ::typeName(target_type) + " expr type: " + UDQ::typeName(tree.var_type);
        parseContext.handleError(ParseContext::UDQ_TYPE_ERROR, msg, errors);
        if (parseContext.get(ParseContext::UDQ_TYPE_ERROR) != InputError::IGNORE)
            dump_tokens(target_var, tokens);

        return UDQASTNode( udq_params.undefinedValue() );
    }

    if (tree.var_type == UDQVarType::NONE) {
        std::string msg = "Parse error when evaluating UDQ define expression - could not determine expression type";
        parseContext.handleError(ParseContext::UDQ_TYPE_ERROR, msg, errors);
        if (parseContext.get(ParseContext::UDQ_TYPE_ERROR) != InputError::IGNORE)
            dump_tokens(target_var, tokens);

        return UDQASTNode( udq_params.undefinedValue() );
    }

    return tree;
}

}
