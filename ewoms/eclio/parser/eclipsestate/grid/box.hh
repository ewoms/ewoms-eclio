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
#ifndef BOX_HH_
#define BOX_HH_

#include <cstddef>
#include <limits>
#include <vector>

namespace Ewoms {
    class DeckRecord;
    class EclipseGrid;

    class Box {
    public:

        struct cell_index {
            std::size_t global_index;
            std::size_t active_index;
            std::size_t data_index;

            cell_index(std::size_t g,std::size_t a, std::size_t d) :
                global_index(g),
                active_index(a),
                data_index(d)
            {}

            /*
              This constructor should is used by the global_index_list() member
              which will return a list of *all* the cells in the box. In this
              case the active_index will be set to the global_index. This is a
              hack to simplify the treatment of global fields in the FieldProps
              implementation.
            */
            cell_index(std::size_t g, std::size_t d) :
                global_index(g),
                active_index(g),
                data_index(d)
            {}
        };

        Box(const EclipseGrid& grid);
        Box(const EclipseGrid& grid , int i1 , int i2 , int j1 , int j2 , int k1 , int k2);
        void update(const DeckRecord& deckRecord);
        void reset();

        size_t size() const;
        bool   isGlobal() const;
        size_t getDim(size_t idim) const;
        const std::vector<cell_index>& index_list() const;
        const std::vector<Box::cell_index>& global_index_list() const;
        bool equal(const Box& other) const;

        int I1() const;
        int I2() const;
        int J1() const;
        int J2() const;
        int K1() const;
        int K2() const;

    private:
        void init(int i1, int i2, int j1, int j2, int k1, int k2);
        void initIndexList();
        const EclipseGrid& grid;
        size_t m_stride[3];
        size_t m_dims[3] = { 0, 0, 0 };
        size_t m_offset[3];

        bool   m_isGlobal;
        std::vector<cell_index> m_active_index_list;
        std::vector<cell_index> m_global_index_list;

        int lower(int dim) const;
        int upper(int dim) const;
    };
}

#endif
