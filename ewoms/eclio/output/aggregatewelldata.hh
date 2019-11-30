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

#ifndef EWOMS_AGGREGATE_WELL_DATA_H
#define EWOMS_AGGREGATE_WELL_DATA_H

#include <ewoms/eclio/output/windowedarray.hh>

#include <ewoms/eclio/io/paddedoutputstring.hh>

#include <cstddef>
#include <string>
#include <vector>

namespace Ewoms {
    class Schedule;
    class SummaryState;
    class UnitSystem;
} // namespace Ewoms

namespace Ewoms { namespace data {
    class WellRates;
}} // Ewoms::data

namespace Ewoms { namespace RestartIO { namespace Helpers {

    class AggregateWellData
    {
    public:
        explicit AggregateWellData(const std::vector<int>& inteHead);

	void captureDeclaredWellData(const Schedule&   	sched,
                        const UnitSystem& 		units,
                        const std::size_t 		sim_step,
			const ::Ewoms::SummaryState&  	smry,
			const std::vector<int>& 	inteHead);

        void captureDynamicWellData(const Ewoms::Schedule&        sched,
                                    const std::size_t           sim_step,
                                    const Ewoms::data::WellRates& xw,
                                    const Ewoms::SummaryState&    smry);

        /// Retrieve Integer Well Data Array.
        const std::vector<int>& getIWell() const
        {
            return this->iWell_.data();
        }

        /// Retrieve Floating-Point (Real) Well Data Array.
        const std::vector<float>& getSWell() const
        {
            return this->sWell_.data();
        }

        /// Retrieve Floating-Point (Double Precision) Well Data Array.
        const std::vector<double>& getXWell() const
        {
            return this->xWell_.data();
        }

        /// Retrieve Character Well Data Array.
        const std::vector<EclIO::PaddedOutputString<8>>& getZWell() const
        {
            return this->zWell_.data();
        }

    private:
        /// Aggregate 'IWEL' array (Integer) for all wells.
        WindowedArray<int> iWell_;

        /// Aggregate 'SWEL' array (Real) for all wells.
        WindowedArray<float> sWell_;

        /// Aggregate 'XWEL' array (Double Precision) for all wells.
        WindowedArray<double> xWell_;

        /// Aggregate 'ZWEL' array (Character) for all wells.
        WindowedArray<EclIO::PaddedOutputString<8>> zWell_;

        /// Maximum number of groups in model.
        int nWGMax_;
    };

}}} // Ewoms::RestartIO::Helpers

#endif // EWOMS_AGGREGATE_WELL_DATA_H
