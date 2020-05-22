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

#define BOOST_TEST_MODULE ActiveGridCells
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/utility/activegridcells.hh>
#include <ewoms/eclio/output/summary.hh>

BOOST_AUTO_TEST_SUITE(Default)

BOOST_AUTO_TEST_CASE(testLocal) {
    std::size_t nx=2, ny = 3, nz=2;
    Ewoms::GridDims dims(nx, ny, nz);
    std::vector<int> cells(9);
    std::set<std::array<std::size_t,3> > inactive;
    inactive.insert({0,0,0});
    inactive.insert({1,1,0});
    inactive.insert({1,0,1});

    for(std::size_t k = 0, index = 0; k < nz; ++k)
    {
        for(std::size_t j = 0; j < ny; ++j)
        {
            for(std::size_t i = 0; i < nx; ++i)
            {
                if ( inactive.find({{i,j,k}})==inactive.end())
                {
                    cells[index++] = dims.getGlobalIndex(i,j,k);
                }
            }
        }
    }

    Ewoms::ActiveGridCells active(nx, ny, nz, cells.data(), cells.size());

    const auto& actnum = active.actNum();

    for(std::size_t k = 0, index = 0; k < nz; ++k)
    {
        for(std::size_t j = 0; j < ny; ++j)
        {
            for(std::size_t i = 0; i < nx; ++i)
            {
                auto cartIndex = dims.getGlobalIndex(i,j,k);

                if ( inactive.find({{i,j,k}})!=inactive.end())
                {
                    BOOST_CHECK(actnum[cartIndex] == 0);
                    BOOST_CHECK(active.localCell(cartIndex) == -1);
                    BOOST_CHECK(active.localCell(i,j,k) == -1);
                    BOOST_CHECK(!active.cellActive(cartIndex));
                    BOOST_CHECK(!active.cellActive(i, j, k));
                }
                else
                {
                    BOOST_CHECK(actnum[cartIndex] == 1);
                    BOOST_CHECK(active.localCell(cartIndex) == (int)index);
                    BOOST_CHECK(active.localCell(i,j,k) == (int)index);
                    BOOST_CHECK(active.cellActive(cartIndex));
                    BOOST_CHECK(active.cellActive(i, j, k));
                    ++index;
                }
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
