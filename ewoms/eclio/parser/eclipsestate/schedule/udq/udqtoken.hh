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
#ifndef UDQ_TOKEN_H
#define UDQ_TOKEN_H

#include <string>
#include <vector>
#include <ewoms/common/variant.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

class UDQToken {
public:
    UDQToken(const std::string& string_token, UDQTokenType token_type);
    UDQToken(const std::string& string_token, const std::vector<std::string>& selector);

    const std::vector<std::string>& selector() const;
    const Ewoms::variant<std::string, double>& value() const;
    UDQTokenType type() const;
    std::string str() const;
private:
    UDQTokenType token_type;
    Ewoms::variant<std::string,double> m_value;
    std::vector<std::string> m_selector;
};

}

#endif
