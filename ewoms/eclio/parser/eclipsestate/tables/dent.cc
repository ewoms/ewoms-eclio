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

#include <ewoms/eclio/parser/eclipsestate/tables/dent.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>

namespace Ewoms {

DenT::DenT(const DeckKeyword& keyword) {
    for (const auto& record : keyword)
        this->m_records.emplace_back(record);
}

DenT::entry::entry(const DeckRecord& record) :
    T0(record.getItem(0).getSIDouble(0)),
    C1(record.getItem(1).getSIDouble(0)),
    C2(record.getItem(2).getSIDouble(0))
{
}

DenT DenT::serializeObject()
{
    DenT result;
    result.m_records = {{1,2,3}, {4,5,6}};

    return result;
}

std::size_t DenT::size() const {
    return this->m_records.size();
}

DenT::entry::entry(double T0_, double C1_, double C2_) :
    T0(T0_),
    C1(C1_),
    C2(C2_)
{}

bool DenT::entry::operator==(const DenT::entry& other) const {
    return this->T0 == other.T0 &&
           this->C1 == other.C1 &&
           this->C2 == other.C2;
}

bool DenT::operator==(const DenT& other) const {
    return this->m_records == other.m_records;
}

const DenT::entry& DenT::operator[](const std::size_t index) const {
    return this->m_records.at(index);
}

}
