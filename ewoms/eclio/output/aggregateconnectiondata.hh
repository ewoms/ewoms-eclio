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

#ifndef EWOMS_AGGREGATE_CONNECTION_DATA_H
#define EWOMS_AGGREGATE_CONNECTION_DATA_H

#include <ewoms/eclio/output/windowedarray.hh>

#include <cstddef>
#include <string>
#include <vector>

namespace Ewoms {
    class EclipseGrid;
    class Schedule;
    class UnitSystem;
} // namespace Ewoms

namespace Ewoms { namespace data {
    class WellRates;
}}

namespace Ewoms { namespace RestartIO { namespace Helpers {

    class AggregateConnectionData
    {
    public:
        explicit AggregateConnectionData(const std::vector<int>& inteHead);

        void captureDeclaredConnData(const Ewoms::Schedule&        sched,
                                     const Ewoms::EclipseGrid&     grid,
                                     const Ewoms::UnitSystem&      units,
                                     const Ewoms::data::WellRates& xw,
                                     const std::size_t           sim_step);

        const std::vector<int>& getIConn() const
        {
            return this->iConn_.data();
        }

        const std::vector<float>& getSConn() const
        {
            return this->sConn_.data();
        }

        const std::vector<double>& getXConn() const
        {
            return this->xConn_.data();
        }

    private:
        WindowedMatrix<int> iConn_;
        WindowedMatrix<float> sConn_;
        WindowedMatrix<double> xConn_;
    };

}}} // Ewoms::RestartIO::Helpers

#endif // EWOMS_AGGREGATE_CONNECTION_DATA_H
