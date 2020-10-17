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

#include <array>
#include <sstream>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/edit/editnnc.hh>
#include <ewoms/eclio/parser/parserkeywords/e.hh>
#include <ewoms/eclio/opmlog/opmlog.hh>

namespace Ewoms
{

bool isNeighbor(const std::array<size_t, 3>& ijk1, const std::array<size_t, 3>& ijk2)
{
    if ( (ijk1[0] + 1) == ijk2[0] || (ijk1[0] - 1) == ijk2[0] )
    {
        return ijk1[1] == ijk2[1] && ijk1[2] == ijk2[2];
    }
    if  ( (ijk1[1] + 1) == ijk2[1] || (ijk1[1] - 1) == ijk2[1] )
    {
        return ijk1[0] == ijk2[0] && ijk1[2] == ijk2[2];
    }
    if( (ijk1[2] + 1) == ijk2[2] || (ijk1[2] - 1) == ijk2[2] )
    {
        return ijk1[1] == ijk2[1] && ijk1[1] == ijk2[1];
    }
    return false;
}

void readEditNncs(const std::vector< const DeckKeyword* >& editNncsKw, std::vector<NNCdata>& editNncs, const GridDims& gridDims)
{
    for (size_t idx_nnc = 0; idx_nnc<editNncsKw.size(); ++idx_nnc) {
        const auto& nnc = *editNncsKw[idx_nnc];
        editNncs.reserve(editNncs.size()+nnc.size());
        for (size_t i = 0; i < nnc.size(); ++i) {
            std::array<size_t, 3> ijk1;
            ijk1[0] = static_cast<size_t>(nnc.getRecord(i).getItem(0).get< int >(0)-1);
            ijk1[1] = static_cast<size_t>(nnc.getRecord(i).getItem(1).get< int >(0)-1);
            ijk1[2] = static_cast<size_t>(nnc.getRecord(i).getItem(2).get< int >(0)-1);
            size_t global_index1 = gridDims.getGlobalIndex(ijk1[0],ijk1[1],ijk1[2]);

            std::array<size_t, 3> ijk2;
            ijk2[0] = static_cast<size_t>(nnc.getRecord(i).getItem(3).get< int >(0)-1);
            ijk2[1] = static_cast<size_t>(nnc.getRecord(i).getItem(4).get< int >(0)-1);
            ijk2[2] = static_cast<size_t>(nnc.getRecord(i).getItem(5).get< int >(0)-1);
            size_t global_index2 = gridDims.getGlobalIndex(ijk2[0],ijk2[1],ijk2[2]);

            const double trans = nnc.getRecord(i).getItem(6).get<double>(0);
            using std::abs;
            if ( !isNeighbor(ijk1, ijk2) )
            {
                editNncs.emplace_back(global_index1, global_index2, trans);
            }
            else
            {
                std::ostringstream sstr;
                sstr << "Cannot edit neighboring connection from " << global_index1 <<" to "<<
                     global_index2<< " with EDITNNC";
                Ewoms::OpmLog::warning(sstr.str());
            }
        }
    }
}

EDITNNC::EDITNNC(const Deck& deck)
{
    GridDims gridDims(deck);
    const auto& tmpEditNncs = deck.getKeywordList<ParserKeywords::EDITNNC>();
    readEditNncs(tmpEditNncs, m_editnnc, gridDims);
    auto compare = [](const NNCdata& d1, const NNCdata& d2)
        { return d1.cell1 < d2.cell1 ||
          ( d1.cell1 == d2.cell1 && d1.cell2 < d2.cell2 );};
    std::sort(m_editnnc.begin(), m_editnnc.end(), compare);
}

EDITNNC EDITNNC::serializeObject()
{
    EDITNNC result;
    result.m_editnnc = {{1,2,1.0},{2,3,2.0}};

    return result;
}

size_t EDITNNC::size() const {
    return(m_editnnc.size());
}

bool EDITNNC::empty() const {
    return m_editnnc.empty();
}

bool EDITNNC::operator==(const EDITNNC& data) const {
    return m_editnnc == data.m_editnnc;
}

} // namespace Ewoms
