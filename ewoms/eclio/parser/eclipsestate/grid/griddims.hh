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

#ifndef EWOMS_PARSER_GRIDDIMS_H
#define EWOMS_PARSER_GRIDDIMS_H

#include <array>
#include <stdexcept>
#include <vector>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>

namespace Ewoms {
    class GridDims
    {
    public:

        GridDims();
        explicit GridDims(std::array<int, 3> xyz);

        GridDims(size_t nx, size_t ny, size_t nz);

        explicit GridDims(const Deck& deck);

        size_t getNX() const;

        size_t getNY() const;
        size_t getNZ() const;
        size_t operator[](int dim) const;

        const std::array<int, 3> getNXYZ() const;

        size_t getGlobalIndex(size_t i, size_t j, size_t k) const;

        const std::array<int, 3> getIJK(size_t globalIndex) const;

        size_t getCartesianSize() const;

        void assertGlobalIndex(size_t globalIndex) const;

        void assertIJK(size_t i, size_t j, size_t k) const;

        bool operator==(const GridDims& data) const;

    protected:
        size_t m_nx;
        size_t m_ny;
        size_t m_nz;

    private:
        void init(const DeckKeyword& keyword);
        void binary_init(const Deck& deck);
    };
}

#endif /* EWOMS_PARSER_GRIDDIMS_HH */
