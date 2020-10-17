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
#ifndef ECLIPSE_IO_UTIL_H
#define ECLIPSE_IO_UTIL_H

#include <vector>
#include <string>
#include <iostream>

namespace Ewoms
{
namespace EclipseIOUtil
{

    template <typename T>
    void addToStripedData(const std::vector<T>& data, std::vector<T>& result, size_t offset, size_t stride) {
        int dataindex = 0;
        for (size_t index = offset; index < result.size(); index += stride) {
            result[index] = data[dataindex];
            ++dataindex;
        }
    }

    template <typename T>
    void extractFromStripedData(const std::vector<T>& data, std::vector<T>& result, size_t offset, size_t stride) {
        for (size_t index = offset; index < data.size(); index += stride) {
            result.push_back(data[index]);
        }
    }

} //namespace EclipseIOUtil
} //namespace Ewoms

#endif //ECLIPSE_IO_UTIL_H
