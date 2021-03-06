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
#ifndef WLISTMANAGER_H
#define WLISTMANAGER_H

#include <cstddef>
#include <map>
#include <vector>
#include <string>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wlist.hh>

namespace Ewoms {

class WListManager {
public:
    WListManager() = default;

    static WListManager serializeObject();

    bool hasList(const std::string&) const;
    WList& getList(const std::string& name);
    const WList& getList(const std::string& name) const;
    WList& newList(const std::string& name);
    void delWell(const std::string& well);

    bool operator==(const WListManager& data) const;
    std::vector<std::string> wells(const std::string& wlist_pattern) const;
    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.map(wlists);
    }

private:
    std::map<std::string, WList> wlists;
};

}
#endif
