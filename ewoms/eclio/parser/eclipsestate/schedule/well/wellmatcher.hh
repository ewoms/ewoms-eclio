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
#ifndef WELL_MATCHER_H
#define WELL_MATCHER_H

#include <vector>
#include <string>

#include <ewoms/eclio/parser/eclipsestate/schedule/well/wlistmanager.hh>

namespace Ewoms {

class WellMatcher {
public:
    WellMatcher() = default;
    explicit WellMatcher(const std::vector<std::string>& wells);
    WellMatcher(const std::vector<std::string>& wells, const WListManager& wlm);
    const std::vector<std::string>& wells() const;
    std::vector<std::string> wells(const std::string& pattern) const;

private:
    std::vector<std::string> m_wells;
    WListManager m_wlm;
};

}
#endif
