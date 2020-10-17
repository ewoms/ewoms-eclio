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
#ifndef EWOMS_OUTPUT_ECLIPSE_VECTOR_LOGIHEAD_H
#define EWOMS_OUTPUT_ECLIPSE_VECTOR_LOGIHEAD_H

#include <vector>

namespace Ewoms { namespace RestartIO { namespace Helpers { namespace VectorItems {

    // This is a subset of the items in src/ewoms/eclio/output/LogiHEAD.cpp .
    // Promote items from that list to this in order to make them public.
    enum logihead : std::vector<bool>::size_type {
        IsLiveOil  =  0,    // Oil phase w/dissolved gas
        IsWetGas   =  1,    // Gas phase w/vaporised oil
        DirKr      =  2,    // Directional relative permeability
        E100RevKr  =  3,    // Reversible rel. perm. (E100)
        E100Radial =  4,    // Radial model (E100)
        E300Radial =  3,    // Radial model (E300, others)
        E300RevKr  =  4,    // Reversible rel. perm. (E300, others)
        Hyster     =  6,    // Enable hysteresis
        DualPoro   = 14,    // Enable dual porosity
        EndScale   = 16,    // Enable end-point scaling
        DirEPS     = 17,    // Directional end-point scaling
        RevEPS     = 18,    // Reversible end-point scaling
        AltEPS     = 19,    // Alternative (3-pt) end-point scaling
        ConstCo    = 38,    // Constant oil compressibility (PVCDO)
        HasMSWells = 75,    // Whether or not model has MS Wells.
    };
}}}} // Ewoms::RestartIO::Helpers::VectorItems

#endif // EWOMS_OUTPUT_ECLIPSE_VECTOR_LOGIHEAD_H
