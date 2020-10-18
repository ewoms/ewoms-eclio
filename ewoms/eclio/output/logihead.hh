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
#ifndef EWOMS_LOGIHEAD_H
#define EWOMS_LOGIHEAD_H

#include <vector>

namespace Ewoms { namespace RestartIO {

    class LogiHEAD
    {
    public:
        /// Key characteristics of simulation run's PVT model.
        struct PVTModel
        {
            /// Whether or not simulation run uses a live oil model (with
            /// dissolved gas).
            bool isLiveOil { false };

            /// Whether or not simulation run uses a wet gas model (with
            /// vaporised oil).
            bool isWetGas { false };

            /// Whether or not simulation run uses a constant
            /// compressibility oil model (keyword PVCDO).
            bool constComprOil { false };
        };

        /// Key characteristics of simulation model's saturation functions.
        struct SatfuncFlags
        {
            /// Whether or not simulation run uses directionally dependent
            /// relative permeability.
            bool useDirectionalRelPerm { false };

            /// Whether or not simulation run uses reversible relative
            /// permeability functions.
            bool useReversibleRelPerm { true };

            /// Whether or not simulation run uses end-point scaling.
            bool useEndScale { false };

            /// Whether or not simulation run uses directionally dependent
            /// end-point scaling.
            bool useDirectionalEPS { false };

            /// Whether or not simulation run uses reversible end-point
            /// scaling.
            bool useReversibleEPS { true  };

            /// Whether or not simulation run activates the alternative
            /// (three-point) end-point scaling feature.
            bool useAlternateEPS { false };
        };

        LogiHEAD();
        ~LogiHEAD() = default;

        LogiHEAD(const LogiHEAD& rhs) = default;
        LogiHEAD(LogiHEAD&& rhs) = default;

        LogiHEAD& operator=(const LogiHEAD& rhs) = default;
        LogiHEAD& operator=(LogiHEAD&& rhs) = default;

        LogiHEAD& variousParam(const bool e300_radial,
                               const bool e100_radial,
                               const int  nswlmx,
			       const bool enableHyster
			      );

        /// Assign PVT model characteristics.
        ///
        /// \param[in] pvt Current run's PVT model characteristics.
        ///
        /// \return \code *this \endcode.
        LogiHEAD& pvtModel(const PVTModel& pvt);

        /// Assign saturation function characteristics.
        ///
        /// \param[in] satfunc Current run's saturation function
        ///    characteristics.
        ///
        /// \return \code *this \endcode.
        LogiHEAD& saturationFunction(const SatfuncFlags& satfunc);

        const std::vector<bool>& data() const
        {
            return this->data_;
        }

    private:
        std::vector<bool> data_;
    };

}} // Ewoms::RestartIO

#endif // EWOMS_LOGIHEAD_H
