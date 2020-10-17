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
#ifndef ActionContext_H
#define ActionContext_H

#include <string>
#include <map>

#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wlistmanager.hh>

namespace Ewoms {
namespace Action {

/*
  The Action::Context class is used as context when the ACTIONX condition is
  evaluated. The Action::Context class is mainly just a thin wrapper around the
  SummaryState class.
*/

class Context {
public:
    explicit Context(const SummaryState& summary_state, const WListManager& wlm);

    /*
      The get methods will first check the internal storage in the 'values' map
      and then subsequently query the SummaryState member.
    */
    double get(const std::string& func, const std::string& arg) const;
    void   add(const std::string& func, const std::string& arg, double value);

    double get(const std::string& func) const;
    void   add(const std::string& func, double value);

    std::vector<std::string> wells(const std::string& func) const;
    const WListManager& wlist_manager() const;

private:
    const SummaryState& summary_state;
    const WListManager& wlm;
    std::map<std::string, double> values;
};
}
}
#endif
