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

#include <ewoms/eclio/parser/eclipsestate/schedule/rptconfig.hh>

#include <ewoms/eclio/errormacros.hh>

namespace {

    std::pair<std::string, unsigned> parse_mnemonic(const std::string& mnemonic) {
        const auto pivot { mnemonic.find('=') } ;

        if (pivot == std::string::npos) {
            return { mnemonic, 1 } ;
        } else {
            const auto int_value { std::stoi(mnemonic.substr(pivot + 1)) } ;

            if (!(int_value >= 0)) {
                EWOMS_THROW(std::invalid_argument, "RPTSCHED - " + mnemonic + " - mnemonic value must be an integer greater than 1");
            }

            return { mnemonic.substr(0, pivot), int_value } ;
        }
    }

}

Ewoms::RPTConfig::RPTConfig(const DeckKeyword& keyword) :
    std::unordered_map<std::string,unsigned> {}
{
    const auto& mnemonics { keyword.getStringData() } ;
    for (const auto& mnemonic : mnemonics) {
        if (mnemonic == "NOTHING") {
            clear();
        } else {
            emplace(parse_mnemonic(mnemonic));
        }
    }
}

#if __cplusplus <= 201703L
bool Ewoms::RPTConfig::contains(const std::string& key) const {
    return find(key) != end();
}
#endif
