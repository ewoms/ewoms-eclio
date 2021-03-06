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
#ifndef WLIST_H
#define WLIST_H

#include <cstddef>
#include <unordered_set>
#include <vector>
#include <string>

namespace Ewoms {

class WList {
public:
    using storage = std::unordered_set<std::string>;

    WList() = default;
    WList(const storage& wlist);
    std::size_t size() const;
    void add(const std::string& well);
    void del(const std::string& well);
    bool has(const std::string& well) const;

    std::vector<std::string> wells() const;
    storage::const_iterator begin() const;
    storage::const_iterator end() const;

    bool operator==(const WList& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(well_list);
    }

private:
    storage well_list;
};

}

#endif
