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

#include <vector>
#include <algorithm>
#include <stdexcept>

#include <ewoms/eclio/parser/eclipsestate/schedule/action/state.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionx.hh>

namespace Ewoms {
namespace Action {

namespace {
std::pair<std::string, std::size_t> action_id(const ActionX& action) {
    return std::make_pair(action.name(), action.id());
}
}

std::size_t State::run_count(const ActionX& action) const {
    auto count_iter = this->run_state.find(action_id(action));
    if (count_iter == this->run_state.end())
        return 0;

    return count_iter->second.run_count;
}

std::time_t State::run_time(const ActionX& action) const {
    auto state = this->run_state.at(action_id(action));
    return state.last_run;
}

void State::add_run(const ActionX& action, std::time_t run_time) {
    const auto& id  = action_id(action);
    auto count_iter = this->run_state.find(id);
    if (count_iter == this->run_state.end())
        this->run_state.insert( std::make_pair(id, run_time) );
    else
        count_iter->second.add_run(run_time);
}

}
}
