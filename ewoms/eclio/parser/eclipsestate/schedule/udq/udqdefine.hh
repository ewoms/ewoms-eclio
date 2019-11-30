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

#ifndef UDQ_DEFINE_H
#define UDQ_DEFINE_H

#include <string>
#include <vector>
#include <set>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqset.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqcontext.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqfunctiontable.hh>

namespace Ewoms {

class UDQASTNode;
class ParseContext;
class ErrorGuard;

class UDQDefine{
public:
    UDQDefine(const UDQParams& udq_params,
              const std::string& keyword,
              const std::vector<std::string>& deck_data);

    UDQDefine(const UDQParams& udq_params,
              const std::string& keyword,
              const std::vector<std::string>& deck_data,
              const ParseContext& parseContext,
              ErrorGuard& errors);

    template <typename T>
    UDQDefine(const UDQParams& udq_params,
              const std::string& keyword,
              const std::vector<std::string>& deck_data,
              const ParseContext& parseContext,
              T&& errors);

    UDQSet eval(const UDQContext& context) const;
    const std::string& keyword() const;
    const std::string& input_string() const;
    UDQVarType  var_type() const;
    std::set<UDQTokenType> func_tokens() const;
private:
    const UDQParams& udq_params;  // Beacuse of the shared RNG stream this must be a reference.
    std::string m_keyword;
    std::shared_ptr<UDQASTNode> ast;
    UDQVarType m_var_type;
    std::string string_data;
};
}

#endif
