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

#include <ewoms/eclio/utility/timeservice.hh>

#include <chrono>
#include <ctime>
#include <utility>

namespace {
    std::time_t advance(const std::time_t tp, const double sec)
    {
        using namespace std::chrono;

        using TP      = time_point<system_clock>;
        using DoubSec = duration<double, seconds::period>;

        const auto t = system_clock::from_time_t(tp) +
            duration_cast<TP::duration>(DoubSec(sec));

        return system_clock::to_time_t(t);
    }

    std::time_t makeUTCTime(std::tm timePoint)
    {
        const auto ltime =  std::mktime(&timePoint);
        auto       tmval = *std::gmtime(&ltime); // Mutable.

        // offset =  ltime - tmval
        //        == #seconds by which 'ltime' is AHEAD of tmval.
        const auto offset =
            std::difftime(ltime, std::mktime(&tmval));

        // Advance 'ltime' by 'offset' so that std::gmtime(return value) will
        // have the same broken-down elements as 'tp'.
        return advance(ltime, offset);
    }
}

Ewoms::TimeStampUTC::TimeStampUTC(const std::time_t tp)
{
    auto t = tp;
    const auto tm = *std::gmtime(&t);

    this->ymd_ = YMD { tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday };

    this->hour(tm.tm_hour).minutes(tm.tm_min).seconds(tm.tm_sec);
}

Ewoms::TimeStampUTC& Ewoms::TimeStampUTC::operator=(const std::time_t tp)
{
    auto t = tp;
    const auto tm = *std::gmtime(&t);

    this->ymd_ = YMD { tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday };

    this->hour(tm.tm_hour).minutes(tm.tm_min).seconds(tm.tm_sec);

    return *this;
}

Ewoms::TimeStampUTC::TimeStampUTC(const YMD& ymd)
    : ymd_{ std::move(ymd) }
{}

Ewoms::TimeStampUTC& Ewoms::TimeStampUTC::hour(const int h)
{
    this->hour_ = h;
    return *this;
}

Ewoms::TimeStampUTC& Ewoms::TimeStampUTC::minutes(const int m)
{
    this->minutes_ = m;
    return *this;
}

Ewoms::TimeStampUTC& Ewoms::TimeStampUTC::seconds(const int s)
{
    this->seconds_ = s;
    return *this;
}

Ewoms::TimeStampUTC& Ewoms::TimeStampUTC::microseconds(const int us)
{
    this->usec_ = us;
    return *this;
}

std::time_t Ewoms::asTimeT(const TimeStampUTC& tp)
{
    auto timePoint = std::tm{};

    timePoint.tm_year = tp.year()  - 1900;
    timePoint.tm_mon  = tp.month() -    1;
    timePoint.tm_mday = tp.day();

    timePoint.tm_hour = tp.hour();
    timePoint.tm_min  = tp.minutes();
    timePoint.tm_sec  = tp.seconds();

    return makeUTCTime(timePoint);
}

Ewoms::TimeStampUTC Ewoms::operator+(const Ewoms::TimeStampUTC& lhs, std::chrono::duration<double> delta) {
    return Ewoms::TimeStampUTC( advance(Ewoms::asTimeT(lhs) , delta.count()) );
}

