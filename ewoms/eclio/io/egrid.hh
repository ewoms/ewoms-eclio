/*

   This file is part of the eWoms project.

   eWoms is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eWoms is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
   */

#ifndef EWOMS_IO_EGRID_H
#define EWOMS_IO_EGRID_H

#include <ewoms/eclio/io/eclfile.hh>

#include <array>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>
#include <map>

namespace Ewoms { namespace EclIO {

class EGrid : public EclFile
{
public:
    explicit EGrid(const std::string& filename);

    int global_index(int i, int j, int k) const;
    int active_index(int i, int j, int k) const;

    const std::array<int, 3>& dimension() const { return nijk; }

    std::array<int, 3> ijk_from_active_index(int actInd) const;
    std::array<int, 3> ijk_from_global_index(int globInd) const;

    void getCellCorners(int globindex, std::array<double,8>& X, std::array<double,8>& Y, std::array<double,8>& Z) const;
    void getCellCorners(const std::array<int, 3>& ijk, std::array<double,8>& X, std::array<double,8>& Y, std::array<double,8>& Z) const;

    int activeCells() const { return nactive; }
    int totalNumberOfCells() const { return nijk[0] * nijk[1] * nijk[2]; }

private:
    std::array<int, 3> nijk;
    int nactive;

    std::vector<int> act_index;
    std::vector<int> glob_index;
    std::vector<float> coord_array;
    std::vector<float> zcorn_array;
};

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_EGRID_H
