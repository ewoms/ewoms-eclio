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

#include <numeric>

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

std::string UDQToken::str() const {
    if (Ewoms::holds_alternative<std::string>(this->m_value))
        return Ewoms::get<std::string>(this->m_value) + std::string{" "} + std::accumulate(this->m_selector.begin(), this->m_selector.end(), std::string{},
                                                                                         [](const std::string& s1, const std::string& s2) { return s1 + " " + s2; });
    else
        return std::to_string(Ewoms::get<double>(this->m_value));
}

}
