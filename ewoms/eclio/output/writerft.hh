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
#ifndef EWOMS_WRITE_RFT_H
#define EWOMS_WRITE_RFT_H

namespace Ewoms {

    class EclipseGrid;
    class Schedule;
    class UnitSystem;

} // namespace Ewoms

namespace Ewoms { namespace data {

    class WellRates;

}} // namespace Ewoms::data

namespace Ewoms { namespace EclIO { namespace OutputStream {

    class RFT;

}}} // namespace Ewoms::EclIO::OutputStream

namespace Ewoms { namespace RftIO {

    /// Collect RFT data and output to pre-opened output stream.
    ///
    /// RFT data is output for all affected wells at a given timestep,
    /// and consists of
    ///
    ///   1) Time stamp (elapsed and date)
    ///   2) Metadata about the output (units of measure, well types,
    ///      data type identifier)
    ///   3) Depth, pressure, Sw, Sg, (I,J,K), and hostgrid for each
    ///      reservoir connection of the affected well.
    ///
    /// If no RFT output is requested at given timestep, then this
    /// function returns with no output written to the RFT file.
    ///
    /// \param[in] reportStep Report step time point for which to output
    ///    RFT data.
    ///
    /// \param[in] elapsed Number of seconds of simulated time until
    ///    this report step (\p reportStep).
    ///
    /// \param[in] usys Unit system conventions for output.  Typically
    ///    corresponds to run's active unit system (i.e., \code
    ///    EclipseState::getUnits() \endcode).
    ///
    /// \param[in] grid Run's active grid.  Used to determine which
    ///    reservoir connections are in active grid cells.
    ///
    /// \param[in] schedule Run's SCHEDULE section from which to access
    ///    the active wells and the RFT configuration.
    ///
    /// \param[in,out] rftFile RFT output stream.  On input, appropriately
    ///    positioned for new content (i.e., at end-of-file).  On output,
    ///    containing new RFT output (if applicable) and positioned after
    ///    new contents.
    void write(const int                        reportStep,
               const double                     elapsed,
               const ::Ewoms::UnitSystem&         usys,
               const ::Ewoms::EclipseGrid&        grid,
               const ::Ewoms::Schedule&           schedule,
               const ::Ewoms::data::WellRates&    wellSol,
               ::Ewoms::EclIO::OutputStream::RFT& rftFile);

}} // namespace Ewoms::RftIO

#endif // EWOMS_WRITE_RFT_H
