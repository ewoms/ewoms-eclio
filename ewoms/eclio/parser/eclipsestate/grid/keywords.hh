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

#ifndef EWOMS_KEYWORDS_H
#define EWOMS_KEYWORDS_H

#include <ewoms/common/optional.hh>

namespace Ewoms
{

namespace Fieldprops
{

namespace keywords {

template <typename T>
struct keyword_info {
    Ewoms::optional<std::string> unit = Ewoms::nullopt;
    Ewoms::optional<T> scalar_init = Ewoms::nullopt;
    bool multiplier = false;
    bool top = false;
    bool global = false;

    keyword_info<T>& init(T init_value) {
        this->scalar_init = init_value;
        return *this;
    }

    keyword_info<T>& unit_string(const std::string& unit_string) {
        this->unit = unit_string;
        return *this;
    }

    keyword_info<T>& distribute_top(bool dtop) {
        this->top = dtop;
        return *this;
    }

    keyword_info<T>& mult(bool m) {
        this->multiplier = m;
        return *this;
    }

    keyword_info<T>& global_kw(bool g) {
        this->global = g;
        return *this;
    }
};
} // end namespace Keywords
} // end namespace Fieldprops
} //end namespace Ewoms
#endif //EWOMS_KEYWORDS_H
