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
#ifndef EWOMS_IO_SUMMARYNODE_H
#define EWOMS_IO_SUMMARYNODE_H

#include <ewoms/common/optional.hh>

#include <functional>
#include <string>
#include <unordered_set>

namespace Ewoms { namespace EclIO {

struct SummaryNode {
    enum class Category {
        Well,
        Group,
        Field,
        Region,
        Block,
        Connection,
        Segment,
        Aquifer,
        Node,
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
    std::string fip_region;

    constexpr static int default_number { std::numeric_limits<int>::min() };

    std::string unique_key() const;

    using number_renderer = std::function<std::string(const SummaryNode&)>;
    std::string unique_key(number_renderer) const;

    bool is_user_defined() const;

    static Category category_from_keyword(const std::string&, const std::unordered_set<std::string> &miscellaneous_keywords = {});

    Ewoms::optional<std::string> display_name() const;
    Ewoms::optional<std::string> display_number() const;
    Ewoms::optional<std::string> display_number(number_renderer) const;
};

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_SUMMARYNODE_H
