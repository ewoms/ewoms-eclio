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

#include <ctime>
#include <stddef.h>

#include <ewoms/eclio/utility/timeservice.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>

namespace Ewoms {

namespace {
    const std::map<std::string, int> month_indices = {{"JAN", 1},
                                                      {"FEB", 2},
                                                      {"MAR", 3},
                                                      {"APR", 4},
                                                      {"MAI", 5},
                                                      {"MAY", 5},
                                                      {"JUN", 6},
                                                      {"JUL", 7},
                                                      {"JLY", 7},
                                                      {"AUG", 8},
                                                      {"SEP", 9},
                                                      {"OCT", 10},
                                                      {"OKT", 10},
                                                      {"NOV", 11},
                                                      {"DEC", 12},
                                                      {"DES", 12}};
}

    TimeMap::TimeMap(std::time_t startTime) {
        m_timeList.push_back(startTime);
    }

    TimeMap::TimeMap( const Deck& deck) {

        std::time_t time;
        if (deck.hasKeyword("START")) {
            // Use the 'START' keyword to find out the start date (if the
            // keyword was specified)
            const auto& keyword = deck.getKeyword("START");
            time = timeFromEclipse(keyword.getRecord(0));
        } else {
            // The default start date is not specified in the Eclipse
            // reference manual. We hence just assume it is same as for
            // the START keyword for Eclipse R100, i.e., January 1st,
            // 1983...
            time = mkdate(1983, 1, 1);
        }
        m_timeList.push_back(time);
        auto timestamp = TimeStampUTC{time};
        m_first_timestep_months.push_back({0, timestamp});
        m_first_timestep_years.push_back({0, timestamp});

        // find all "TSTEP" and "DATES" keywords in the deck and deal
        // with them one after another
        for( const auto& keyword : deck ) {
            // We're only interested in "TSTEP" and "DATES" keywords,
            // so we ignore everything else here...
            if (keyword.name() != "TSTEP" &&
                keyword.name() != "DATES")
            {
                continue;
            }

            if (keyword.name() == "TSTEP")
                addFromTSTEPKeyword(keyword);
            else if (keyword.name() == "DATES")
                addFromDATESKeyword(keyword);
        }
    }

    size_t TimeMap::numTimesteps() const {
        return m_timeList.size() - 1;
    }

    std::time_t TimeMap::getStartTime(size_t tStepIdx) const {
        return this->operator[]( tStepIdx );
    }

    std::time_t TimeMap::getEndTime() const {
        return this->operator[]( this->size( ) - 1);
    }

    double TimeMap::seconds(size_t timeStep) const {
        return std::difftime( this->operator[](timeStep), this->operator[](0));
    }

    double TimeMap::getTotalTime() const
    {
        if (m_timeList.size() < 2)
            return 0.0;

        return std::difftime(this->m_timeList.back(),
                             this->m_timeList.front());
    }

    void TimeMap::addTime(std::time_t newTime) {
        const std::time_t lastTime = m_timeList.back();
        const size_t step = m_timeList.size();
        if (newTime > lastTime) {
            const auto nw   = TimeStampUTC{ newTime };
            const auto last = TimeStampUTC{ lastTime };

            const auto new_month  = nw  .month();
            const auto last_month = last.month();

            const auto new_year  = nw  .year();
            const auto last_year = last.year();

            if (new_month != last_month || new_year != last_year)
                m_first_timestep_months.push_back({step, nw});

            if (new_year != last_year)
                m_first_timestep_years.push_back({step, nw});

            m_timeList.push_back(newTime);
        } else
            throw std::invalid_argument("Times added must be in strictly increasing order.");
    }

    void TimeMap::addTStep(int64_t step) {
        addTime(forward(m_timeList.back(), step));
    }

    size_t TimeMap::size() const {
        return m_timeList.size();
    }

    size_t TimeMap::last() const {
        return this->numTimesteps();
    }

    const std::map<std::string , int>& TimeMap::eclipseMonthIndices() {
        return month_indices;
    }

    std::time_t TimeMap::timeFromEclipse(const DeckRecord &dateRecord) {
        const auto &dayItem = dateRecord.getItem(0);
        const auto &monthItem = dateRecord.getItem(1);
        const auto &yearItem = dateRecord.getItem(2);
        const auto &timeItem = dateRecord.getItem(3);

        int hour = 0, min = 0, second = 0;
        if (timeItem.hasValue(0)) {
            if (sscanf(timeItem.get<std::string>(0).c_str(), "%d:%d:%d" , &hour,&min,&second) != 3) {
                hour = min = second = 0;
            }
        }

        std::time_t date = mkdatetime(yearItem.get<int>(0),
                                      eclipseMonthIndices().at(monthItem.get<std::string>(0)),
                                      dayItem.get<int>(0),
                                      hour,
                                      min,
                                      second);
        return date;
    }

    void TimeMap::addFromDATESKeyword(const DeckKeyword &DATESKeyword) {
        if (DATESKeyword.name() != "DATES")
            throw std::invalid_argument("Method requires DATES keyword input.");

        for (size_t recordIndex = 0; recordIndex < DATESKeyword.size(); recordIndex++) {
            const auto &record = DATESKeyword.getRecord(recordIndex);
            const std::time_t nextTime = TimeMap::timeFromEclipse(record);
            addTime(nextTime);
        }
    }

    void TimeMap::addFromTSTEPKeyword(const DeckKeyword &TSTEPKeyword) {
        if (TSTEPKeyword.name() != "TSTEP")
            throw std::invalid_argument("Method requires TSTEP keyword input.");
        {
            const auto &item = TSTEPKeyword.getRecord(0).getItem(0);

            for (size_t itemIndex = 0; itemIndex < item.data_size(); itemIndex++) {
                const int64_t seconds = static_cast<int64_t>(item.getSIDouble(itemIndex));
                addTStep(seconds);
            }
        }
    }

    double TimeMap::getTimeStepLength(size_t tStepIdx) const
    {
        assert(tStepIdx < numTimesteps());

        return std::difftime(this->m_timeList[tStepIdx + 1],
                             this->m_timeList[tStepIdx + 0]);
    }

    double TimeMap::getTimePassedUntil(size_t tLevelIdx) const
    {
        assert(tLevelIdx < m_timeList.size());

        return std::difftime(this->m_timeList[tLevelIdx],
                             this->m_timeList.front());
    }

    bool TimeMap::isTimestepInFirstOfMonthsYearsSequence(size_t timestep, bool years, size_t start_timestep, size_t frequency) const {
        bool timestep_first_of_month_year = false;
        const auto& timesteps = (years) ? m_first_timestep_years : m_first_timestep_months;

        auto same_step = [timestep](const StepData& sd) { return sd.stepnumber == timestep; };
        auto ci_timestep = std::find_if(timesteps.begin(), timesteps.end(), same_step);
        if (ci_timestep != timesteps.end() && ci_timestep != timesteps.begin()) {
            if (1 >= frequency) {
                timestep_first_of_month_year = true;
            } else { //Frequency given
                timestep_first_of_month_year = isTimestepInFreqSequence(timestep, start_timestep, frequency, years);
            }
        }
        return timestep_first_of_month_year;
    }

    // Return true if the step is the first of each n-month or n-month
    // period, starting from start_timestep - 1, with n = frequency.
    bool TimeMap::isTimestepInFreqSequence (size_t timestep, size_t start_timestep, size_t frequency, bool years) const {
        // Find iterator to data for 'start_timestep' or first
        // in-sequence step following it, set start_year and
        // start_month.
        const auto& timesteps = (years) ? m_first_timestep_years : m_first_timestep_months;
        auto compare_stepnumber = [](const StepData& sd, size_t value) { return sd.stepnumber < value; };
        auto ci_start_timestep = std::lower_bound(timesteps.begin(), timesteps.end(), start_timestep - 1, compare_stepnumber);
        if (ci_start_timestep == timesteps.end()) {
            // We are after the end of the sequence.
            return false;
        }
        const int start_year = ci_start_timestep->timestamp.year();
        const int start_month = ci_start_timestep->timestamp.month() - 1; // For 0-indexing.

        // Find iterator to data for 'timestep'.
        auto same_step = [timestep](const StepData& sd) { return sd.stepnumber == timestep; };
        auto ci_timestep = std::find_if(timesteps.begin(), timesteps.end(), same_step);
        // The ci_timestep can be assumed to be different from
        // timesteps.end(), or we would not be in this function.
        // If, however, it is at or before the first timestep we should
        // always return false.
        if (ci_timestep <= ci_start_timestep) {
            return false;
        }

        if (years) {
            // Year logic.
            const int my_year = ci_timestep->timestamp.year();
            if ((my_year - start_year) % frequency == 0) {
                return true;
            } else {
                // Check if we are in a new (frequency-year) period.
                const auto prev_it = ci_timestep - 1;
                const int prev_year = prev_it->timestamp.year();
                return (my_year - start_year)/frequency > (prev_year - start_year)/frequency;
            }
        } else {
            // Month logic.
            const int my_year = ci_timestep->timestamp.year();
            const int my_month = (my_year - start_year) * 12 + ci_timestep->timestamp.month() - 1;
            // my_month is now the count of months since start_month.
            assert(my_month > start_month);
            if ((my_month - start_month) % frequency == 0) {
                return true;
            } else {
                // Check if we are in a new (frequency-month) period.
                const auto prev_it = ci_timestep - 1;
                const int prev_year = prev_it->timestamp.year();
                const int prev_month = (prev_year - start_year) * 12 + prev_it->timestamp.month() - 1;
                return (my_month - start_month)/frequency > (prev_month - start_month)/frequency;
            }
        }
    }

    // vec is assumed to be sorted
    size_t TimeMap::closest(const std::vector<size_t> & vec, size_t value) const
    {
        std::vector<size_t>::const_iterator ci =
            std::lower_bound(vec.begin(), vec.end(), value);
        if (ci != vec.end()) {
            return *ci;
        }
        return 0;
    }

    std::time_t TimeMap::operator[] (size_t index) const {
        if (index < m_timeList.size()) {
            return m_timeList[index];
        } else
            throw std::invalid_argument("Index out of range");
    }

    std::time_t TimeMap::mkdate(int in_year, int in_month, int in_day) {
        return mkdatetime(in_year , in_month , in_day, 0,0,0);
    }

    std::time_t TimeMap::mkdatetime(int in_year, int in_month, int in_day, int hour, int minute, int second) {
        const auto tp = TimeStampUTC{ TimeStampUTC::YMD { in_year, in_month, in_day } }
            .hour(hour).minutes(minute).seconds(second);

        std::time_t t = asTimeT(tp);
        {
            /*
               The underlying mktime( ) function will happily wrap
               around dates like January 33, this function will check
               that no such wrap-around has taken place.
            */
            const auto check = TimeStampUTC{ t };
            if ((in_day != check.day()) || (in_month != check.month()) || (in_year != check.year()))
                throw std::invalid_argument("Invalid input arguments for date.");
        }
        return t;
    }

    std::time_t TimeMap::forward(std::time_t t, int64_t seconds) {
        return t + seconds;
    }

    std::time_t TimeMap::forward(std::time_t t, int64_t hours, int64_t minutes, int64_t seconds) {
        return t + seconds + minutes * 60 + hours * 3600;
    }
}

