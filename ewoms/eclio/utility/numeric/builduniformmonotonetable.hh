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
#ifndef EWOMS_BUILDUNIFORMMONOTONETABLE_H
#define EWOMS_BUILDUNIFORMMONOTONETABLE_H

#include <ewoms/eclsimulators/deprecated/utility/monotcubicinterpolator.hh>
#include <ewoms/eclsimulators/deprecated/utility/uniformtablelinear.hh>

namespace Ewoms {

    template <typename T>
    void buildUniformMonotoneTable(const std::vector<double>& xv,
                                   const std::vector<T>& yv,
                                   const int samples,
                                   UniformTableLinear<T>& table)
    {
        MonotCubicInterpolator interp(xv, yv);
        std::vector<T> uniform_yv(samples);
        double xmin = xv[0];
        double xmax = xv.back();
        for (int i = 0; i < samples; ++i) {
            double w = double(i)/double(samples - 1);
            double x = (1.0 - w)*xmin + w*xmax;
            uniform_yv[i] = interp(x);
        }
        table = UniformTableLinear<T>(xmin, xmax, uniform_yv);
    }

} // namespace Ewoms

#endif // EWOMS_BUILDUNIFORMMONOTONETABLE_H
