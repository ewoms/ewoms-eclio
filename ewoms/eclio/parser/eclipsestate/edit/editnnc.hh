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
#ifndef EWOMS_COMMON_EDITNNC_H
#define EWOMS_COMMON_EDITNNC_H

#include <ewoms/eclio/parser/eclipsestate/grid/nnc.hh>
namespace Ewoms
{

/// Represents edit information for non-neighboring connections (NNCs, faults, etc.)
class EDITNNC
{
public:
    EDITNNC() = default;

    /// Construct from input deck
    explicit EDITNNC(const Deck& deck);

    explicit EDITNNC(const std::vector<NNCdata>& data);

    /// \brief Get an ordered set of EDITNNC
    const std::vector<NNCdata>& data() const
    {
        return m_editnnc;
    }
    /// \brief Get the number of entries
    size_t size() const;
    /// \brief Whether EDITNNC was empty.
    bool empty() const;

    bool operator==(const EDITNNC& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.vector(m_editnnc);
    }

private:
    std::vector<NNCdata> m_editnnc;
};
}
#endif // EWOMS_COMMON_EDITNNC_H
