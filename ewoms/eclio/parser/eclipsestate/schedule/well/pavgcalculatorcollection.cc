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

#include <unordered_set>
#include <ewoms/common/fmt/format.h>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/pavgcalculatorcollection.hh>

namespace Ewoms {

bool PAvgCalculatorCollection::empty() const {
    return true;
}

bool PAvgCalculatorCollection::has(const std::string& wname) const {
    return this->calculators.count(wname) > 0;
}

const PAvgCalculator& PAvgCalculatorCollection::get(const std::string& wname) const {
    return this->calculators.at(wname);
}

void PAvgCalculatorCollection::add(const PAvgCalculator& calculator) {
    this->calculators.emplace( calculator.wname(), calculator );
    this->indexlist = Ewoms::nullopt;
}

const std::vector<std::size_t>& PAvgCalculatorCollection::index_list() const {
    if (!static_cast<bool>(this->indexlist)) {
        std::unordered_set<std::size_t> il;
        for ( const auto& cPair : this->calculators) {
            const auto& calculator = cPair.second;
            const auto& calc_il = calculator.index_list();
            il.insert(calc_il.begin(), calc_il.end());
        }
        this->indexlist = std::vector<std::size_t>{ il.begin(), il.end() };
    }
    return this->indexlist.value();
}

void PAvgCalculatorCollection::add_pressure(std::size_t index, double pressure) {
    for (auto& cPair : this->calculators) {
        auto& calculator = cPair.second;
        calculator.add_pressure(index, pressure);
    }
}

}
