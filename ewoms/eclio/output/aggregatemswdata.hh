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
#ifndef EWOMS_AGGREGATE_MSW_DATA_H
#define EWOMS_AGGREGATE_MSW_DATA_H

#include <ewoms/eclio/output/data/wells.hh>
#include <ewoms/eclio/output/windowedarray.hh>

#include <string>
#include <vector>

namespace Ewoms {
    class Phases;
    class Schedule;
    class EclipseGrid;
    class UnitSystem;
    class SummaryState;
} // namespace Ewoms

namespace Ewoms { namespace RestartIO { namespace Helpers {

    struct BranchSegmentPar {
      int outletS;
      int noSegInBranch;
      int firstSeg;
      int lastSeg;
      int branch;
    };

    struct SegmentSetSourceSinkTerms {
      std::vector<double> qosc;
      std::vector<double> qwsc;
      std::vector<double> qgsc;
    };

    struct SegmentSetFlowRates {
      std::vector<double> sofr;
      std::vector<double> swfr;
      std::vector<double> sgfr;
    };

    class AggregateMSWData
    {
    public:
        explicit AggregateMSWData(const std::vector<int>& inteHead);

        void captureDeclaredMSWData(const Ewoms::Schedule& sched,
                                     const std::size_t    rptStep,
				     const Ewoms::UnitSystem& units,
				     const std::vector<int>& inteHead,
				     const Ewoms::EclipseGrid&  grid,
				     const Ewoms::SummaryState& smry,
				     const Ewoms::data::WellRates&  wr
				   );

        /// Retrieve Integer Multisegment well data Array.
        const std::vector<int>& getISeg() const
        {
            return this->iSeg_.data();
        }

        /// Retrieve Double precision segment data Array.
        const std::vector<double>& getRSeg() const
        {
            return this->rSeg_.data();
        }

        /// Retrieve Integer multisegment well data Array for lateral branches  (ILBS)
        const std::vector<int>& getILBs() const
        {
            return this->iLBS_.data();
        }

        /// Retrieve Integer multisegment well data Array for lateral branches (ILBR)
        const std::vector<int>& getILBr() const
        {
            return this->iLBR_.data();
        }

    private:
        /// Aggregate 'ISEG' array (Integer) for all multisegment wells
        WindowedArray<int> iSeg_;

        /// Aggregate 'RSEG' array (Double) for all multisegment wells
        WindowedArray<double> rSeg_;

        /// Aggregate 'ILBS' array (Integer) for all multisegment wells
        WindowedArray<int> iLBS_;

        /// Aggregate 'ILBR' array (Integer) for all multisegment wells
        WindowedArray<int> iLBR_;

    };

}}} // Ewoms::RestartIO::Helpers

#endif // EWOMS_AGGREGATE_WELL_DATA_H
