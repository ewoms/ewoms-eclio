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

#ifndef EWOMS_AGGREGATE_Actionx_DATA_H
#define EWOMS_AGGREGATE_Actionx_DATA_H

#include <ewoms/eclio/output/windowedarray.hh>
#include <ewoms/eclio/io/paddedoutputstring.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqinput.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqdefine.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqactive.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqassign.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqfunctiontable.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <cstddef>
#include <string>
#include <vector>
#include <map>

namespace Ewoms {
    class Schedule;
    class UDQInput;
    class UDQActive;

    namespace Action {
        class State;
    }
} // namespace Ewoms

namespace Ewoms { namespace RestartIO { namespace Helpers {

class AggregateActionxData
{
public:
    explicit AggregateActionxData(const std::vector<int>& actDims);

    void captureDeclaredActionxData(    const Ewoms::Schedule&      sched,
                                        const Ewoms::Action::State& action_state,
                                        const Ewoms::SummaryState&  st,
                                        const std::vector<int>&   actDims,
                                        const std::size_t         simStep);

    const std::vector<int>& getIACT() const
    {
        return this->iACT_.data();
    }

    const std::vector<float>& getSACT() const
    {
        return this->sACT_.data();
    }

    const std::vector<EclIO::PaddedOutputString<8>>& getZACT() const
    {
        return this->zACT_.data();
    }

    const std::vector<EclIO::PaddedOutputString<8>>& getZLACT() const
    {
        return this->zLACT_.data();
    }

    const std::vector<EclIO::PaddedOutputString<8>>& getZACN() const
    {
        return this->zACN_.data();
    }

    const std::vector<int>& getIACN() const
    {
        return this->iACN_.data();
    }

    const std::vector<double>& getSACN() const
    {
        return this->sACN_.data();
    }

private:
    /// Aggregate 'IACT' array (Integer) for all ACTIONX data  (9 integers pr UDQ)
    WindowedArray<int> iACT_;

    /// Aggregate 'SACT' array (Integer) for all ACTIONX data  (5 integers pr ACTIONX - currently all zero - meaning unknown)
    WindowedArray<float> sACT_;

    /// Aggregate 'ZACT' array (Character) for all ACTIONX data. (4 * 8 chars pr ACIONX keyword - name of Action)
    WindowedArray<EclIO::PaddedOutputString<8>> zACT_;

    /// Aggregate 'ZLACT' array (Character) for all Actionx data.  (max 16 * 8 characters pr line (default 80 chars pr line)
    WindowedArray<EclIO::PaddedOutputString<8>> zLACT_;

    /// Aggregate 'ZACN' array (Character) for all Actionx data  (length equal to max no of conditions pr Actionx * the number of Actiox kwords)
    WindowedArray<EclIO::PaddedOutputString<8>> zACN_;

    /// Aggregate 'IACN' array (Integer) for all Actionx data  (length 26* the max number of conditoins pr Actionx * the number of Actionx kwords)
    WindowedArray<int> iACN_;

    /// Aggregate 'SACN' array (Integer) for all Actionx data  (16 * max number of Actionx conditions)
    WindowedArray<double> sACN_;

};

}}} // Ewoms::RestartIO::Helpers

#endif //EWOMS_AGGREGATE_WELL_DATA_H
