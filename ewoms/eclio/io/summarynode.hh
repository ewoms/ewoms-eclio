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

#ifndef EWOMS_IO_SUMMARYNODE_H
#define EWOMS_IO_SUMMARYNODE_H

#include <functional>
#include <string>
#include <unordered_set>

namespace Ewoms::EclIO {

struct SummaryNode {
    enum class Category {
        Aquifer,
        Well,
        Group,
        Field,
        Region,
        Block,
        Connection,
        Segment,
        Miscellaneous,
    };

    enum class Type {
        Rate,
        Total,
        Ratio,
        Pressure,
        Count,
        Mode,
        Undefined,
    };

    std::string keyword;
    Category    category;
    Type        type;
    std::string wgname;
    int         number;

    constexpr static int default_number { std::numeric_limits<int>::min() };

    std::string unique_key() const;

    using number_renderer = std::function<std::string(const SummaryNode&)>;
    std::string unique_key(number_renderer) const;

    bool is_user_defined() const;

    static Category category_from_keyword(const std::string&, const std::unordered_set<std::string> &miscellaneous_keywords = {});
};

} // namespace Ewoms::EclIO

#endif // EWOMS_IO_SUMMARYNODE_H
