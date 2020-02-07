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

#ifndef EWOMS_DOUBHEAD_H
#define EWOMS_DOUBHEAD_H

#include <chrono>
#include <cstddef>
#include <vector>

namespace Ewoms {
    class Tuning;
    class Schedule;
    class UDQParams;
}

namespace Ewoms { namespace RestartIO {

    class DoubHEAD
    {
    public:
        struct TimeStamp {
            std::chrono::time_point<std::chrono::system_clock>          start;
            std::chrono::duration<double, std::chrono::seconds::period> elapsed;
        };

        struct guideRate {
            double A;
            double B;
            double C;
            double D;
            double E;
            double F;
            double delay;
            double damping_fact;
        };

        DoubHEAD();

        ~DoubHEAD() = default;
        DoubHEAD(const DoubHEAD& rhs) = default;
        DoubHEAD(DoubHEAD&& rhs) = default;

        DoubHEAD& operator=(const DoubHEAD& rhs) = default;
        DoubHEAD& operator=(DoubHEAD&& rhs) = default;

        DoubHEAD& tuningParameters(const Tuning&     tuning,
                                   const double      cnvT);

        DoubHEAD& timeStamp(const TimeStamp& ts);
        DoubHEAD& nextStep(const double nextTimeStep);

        DoubHEAD& drsdt(const Schedule&   sched,
                        const std::size_t lookup_step,
                        const double      cnvT);

        DoubHEAD& udq_param(const UDQParams& udqPar);
        DoubHEAD& guide_rate_param(const guideRate& guide_rp);

        const std::vector<double>& data() const
        {
            return this->data_;
        }

    private:
        std::vector<double> data_;
    };

}} // Ewoms::RestartIO

#endif  // EWOMS_DOUBHEAD_H
