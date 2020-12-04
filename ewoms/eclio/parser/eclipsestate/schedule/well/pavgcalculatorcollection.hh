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
#ifndef PAVE_CALC_COLLECTIONHPP
#define PAVE_CALC_COLLECTIONHPP

#include <ewoms/common/optional.hh>
#include <string>
#include <unordered_map>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/well/pavg.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/pavgcalculator.hh>

namespace Ewoms {

class PAvgCalculatorCollection {
public:
    bool empty() const;
    void add(const PAvgCalculator& calculator);
    bool has(const std::string& wname) const;
    const PAvgCalculator& get(const std::string& wname) const;
    const std::vector<std::size_t>& index_list() const;
    void add_pressure(std::size_t index, double pressure);
private:
    std::unordered_map<std::string, PAvgCalculator> calculators;
    mutable Ewoms::optional<std::vector<std::size_t>> indexlist;
};

}
#endif
