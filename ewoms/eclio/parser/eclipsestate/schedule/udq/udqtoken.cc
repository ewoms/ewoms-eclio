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

#include "udqtoken.hh"

namespace Ewoms {

UDQToken::UDQToken(const std::string& string_token, UDQTokenType token_type_) :
    token_type(token_type_)
{
    if (this->token_type == UDQTokenType::number)
        this->m_value = stod(string_token);
    else
        this->m_value = string_token;

}

UDQToken::UDQToken(const std::string& string_token, const std::vector<std::string>& selector_):
    token_type(UDQTokenType::ecl_expr),
    m_value(string_token),
    m_selector(selector_)
{
}

const Ewoms::variant<std::string, double>& UDQToken::value() const {
    return this->m_value;
}

const std::vector<std::string>& UDQToken::selector() const {
    return this->m_selector;
}

UDQTokenType UDQToken::type() const {
    return this->token_type;
}

}