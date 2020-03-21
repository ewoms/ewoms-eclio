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

#ifndef ActionCOnfig_H
#define ActionCOnfig_H

#include <string>
#include <ctime>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionx.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/pyaction.hh>

namespace Ewoms {
namespace Action {

/*
  The Actions class is a container of ACTIONX keywords. The main functionality
  is to provide a list of ACTIONX keywords which are ready to be evaluated.
*/

class Actions {
public:
    Actions() = default;
    Actions(const std::vector<ActionX>& action, const std::vector<PyAction>& pyactions);
    size_t size() const;
    int max_input_lines() const;
    bool empty() const;
    void add(const ActionX& action);
    void add(const PyAction& pyaction);
    bool ready(std::time_t sim_time) const;
    const ActionX& get(const std::string& name) const;
    const ActionX& get(std::size_t index) const;
    std::vector<const ActionX *> pending(std::time_t sim_time) const;

    std::vector<ActionX>::const_iterator begin() const;
    std::vector<ActionX>::const_iterator end() const;

    bool operator==(const Actions& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.vector(actions);
        serializer.vector(pyactions);
    }

private:
    std::vector<ActionX> actions;
    std::vector<PyAction> pyactions;
};
}
}
#endif
