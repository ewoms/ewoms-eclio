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

#ifndef EWOMS_IO_ECLUTIL_H
#define EWOMS_IO_ECLUTIL_H

#include <ewoms/eclio/io/ecliodata.hh>

#include <string>
#include <tuple>

namespace Ewoms { namespace EclIO {

    int flipEndianInt(int num);
    float flipEndianFloat(float num);
    double flipEndianDouble(double num);

    std::tuple<int, int> block_size_data_binary(eclArrType arrType);
    std::tuple<int, int, int> block_size_data_formatted(eclArrType arrType);

    std::string trimr(const std::string &str1);

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_ECLUTIL_H