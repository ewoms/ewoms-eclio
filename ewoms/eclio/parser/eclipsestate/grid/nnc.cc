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
#include <array>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/nnc.hh>
#include <ewoms/eclio/parser/parserkeywords/n.hh>

namespace Ewoms
{
    NNC::NNC(const Deck& deck) {
        GridDims gridDims(deck);
        const auto& nncs = deck.getKeywordList<ParserKeywords::NNC>();
        for (size_t idx_nnc = 0; idx_nnc<nncs.size(); ++idx_nnc) {
            const auto& nnc = *nncs[idx_nnc];
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

                const double trans = nnc.getRecord(i).getItem(6).getSIDouble(0);

                addNNC(global_index1,global_index2,trans);
            }
        }
    }

    NNC NNC::serializeObject()
    {
        NNC result;
        result.m_nnc = {{1,2,1.0},{2,3,2.0}};

        return result;
    }

    void NNC::addNNC(const size_t cell1, const size_t cell2, const double trans) {
        NNCdata tmp;
        tmp.cell1 = cell1;
        tmp.cell2 = cell2;
        tmp.trans = trans;
        m_nnc.push_back(tmp);
    }

    size_t NNC::numNNC() const {
        return(m_nnc.size());
    }

    bool NNC::hasNNC() const {
        return m_nnc.size()>0;
    }

    bool NNC::operator==(const NNC& data) const {
        return m_nnc == data.m_nnc;
    }

} // namespace Ewoms

