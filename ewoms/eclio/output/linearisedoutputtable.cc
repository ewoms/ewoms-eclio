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

#include <ewoms/eclio/output/linearisedoutputtable.hh>

#include <algorithm>
#include <cmath>
#include <cassert>
#include <utility>

Ewoms::LinearisedOutputTable::
LinearisedOutputTable(const std::size_t numTables0,
                      const std::size_t numPrimary0,
                      const std::size_t numRows0,
                      const std::size_t numCols0)
    : LinearisedOutputTable(numTables0, numPrimary0,
                            numRows0, numCols0, 1.0e20)
{}

Ewoms::LinearisedOutputTable::
LinearisedOutputTable(const std::size_t numTables0,
                      const std::size_t numPrimary0,
                      const std::size_t numRows0,
                      const std::size_t numCols0,
                      const double      fillValue)
    : data      (numTables0 * numPrimary0 * numRows0 * numCols0, fillValue)
    , numTables (numTables0)
    , numPrimary(numPrimary0)
    , numRows   (numRows0)
{}

std::vector<double>::iterator
Ewoms::LinearisedOutputTable::column(const std::size_t tableID,
                                   const std::size_t primID,
                                   const std::size_t colID)
{
    // Table format: numRows * numPrimary * numTables values for first
    // column (ID == 0), followed by same number of entries for second
    // column &c.
    const auto offset =
        0 + this->numRows*(primID + this->numPrimary*(tableID + this->numTables*colID));

    assert (offset + this->numRows <= this->data.size());

    return this->data.begin() + offset;
}

const std::vector<double>&
Ewoms::LinearisedOutputTable::getData() const
{
    return this->data;
}

std::vector<double>
Ewoms::LinearisedOutputTable::getDataDestructively()
{
    return std::move(this->data);
}

// ---------------------------------------------------------------------

void
Ewoms::DifferentiateOutputTable::calcSlopes(const std::size_t      nDep,
                                          const Descriptor&      desc,
                                          LinearisedOutputTable& table)
{
    if ((nDep == 0) || (desc.numActRows < 2)) {
        // No dependent columns or too few rows to compute any derivatives.
        // Likely to be user error.  Can't do anything here.
        return;
    }

    auto x = table.column(desc.tableID, desc.primID, 0);

    using colIT = decltype(x);

    auto y  = std::vector<colIT>{};  y .reserve(nDep);
    auto dy = std::vector<colIT>{};  dy.reserve(nDep);
    auto y0 = std::vector<double>(nDep);
    auto y1 = y0;
    for (auto j = 0*nDep; j < nDep; ++j) {
        y .push_back(table.column(desc.tableID, desc.primID, j + 1 + 0*nDep));
        dy.push_back(table.column(desc.tableID, desc.primID, j + 1 + 1*nDep));

        y1[j] = *y.back();  ++y.back();

        // Store derivatives at right interval end-point.
        ++dy.back();
    }

    using std::swap;

    auto x1 = *x;  ++x;  auto x0 = 0 * x1;

    // Recall: Number of slope intervals one less than number of active
    // table rows.
    for (auto n = desc.numActRows - 1, i = 0*n; i < n; ++i) {
        // Save previous right-hand point as this left-hand point, clear
        // space for new right-hand point.
        swap(x0, x1);  x1 = *x;  ++x;
        swap(y0, y1);

        const auto dx = x1 - x0;

        for (auto j = 0*nDep; j < nDep; ++j) {
            y1[j] = *y[j];

            const auto delta = y1[j] - y0[j];

            // Choice for dx==0 somewhat debatable.
            *dy[j] = (std::abs(dx) > 0.0) ? (delta / dx) : 0.0;

            // Advance column iterators;
            ++y[j];  ++dy[j];
        }
    }
}
