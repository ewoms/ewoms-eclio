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

#include <fnmatch.h>

#include <unordered_set>

#include <ewoms/eclio/parser/eclipsestate/schedule/well/wlist.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wlistmanager.hh>
namespace Ewoms {

    WListManager WListManager::serializeObject()
    {
        WListManager result;
        result.wlists = {{"test1", WList({"test2", "test3"})}};

        return result;
    }

    bool WListManager::hasList(const std::string& name) const {
        return (this->wlists.find(name) != this->wlists.end());
    }

    WList& WListManager::newList(const std::string& name) {
        this->wlists.erase(name);
        this->wlists.insert( {name, WList() });
        return this->getList(name);
    }

    WList& WListManager::getList(const std::string& name) {
        return this->wlists.at(name);
    }

    const WList& WListManager::getList(const std::string& name) const {
        return this->wlists.at(name);
    }

    void WListManager::delWell(const std::string& well) {
        for (auto& pair: this->wlists) {
            auto& wlist = pair.second;
            wlist.del(well);
        }
    }

    bool WListManager::operator==(const WListManager& data) const {
        return this->wlists == data.wlists;
    }

    std::vector<std::string> WListManager::wells(const std::string& wlist_pattern) const {
        if (this->hasList(wlist_pattern)) {
            const auto& wlist = this->getList(wlist_pattern);
            return { wlist.begin(), wlist.end() };
        } else {
            std::unordered_set<std::string> well_set;
            auto pattern = wlist_pattern.substr(1);
            for (const auto& [name, wlist] : this->wlists) {
                auto wlist_name = name.substr(1);
                int flags = 0;
                if (fnmatch(pattern.c_str(), wlist_name.c_str(), flags) == 0)
                    well_set.insert(wlist.begin(), wlist.end());
            }
            return { well_set.begin(), well_set.end() };
        }
    }

}
