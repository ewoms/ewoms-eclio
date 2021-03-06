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
#ifndef EWOMS_IO_ERFT_H
#define EWOMS_IO_ERFT_H

#include <ewoms/eclio/io/eclfile.hh>

#include <ctime>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace Ewoms { namespace EclIO {

class ERft : public EclFile
{
public:
    explicit ERft(const std::string &filename);

    using RftDate = std::tuple<int,int,int>;
    template <typename T>
    const std::vector<T>& getRft(const std::string& name, const std::string& wellName,
                                 const RftDate& date) const;

    template <typename T>
    const std::vector<T>& getRft(const std::string& name, const std::string& wellName,
                                 int year, int month, int day) const;
    template <typename T>
    const std::vector<T>& getRft(const std::string& name, int reportIndex) const;

    std::vector<std::string> listOfWells() const;
    std::vector<RftDate> listOfdates() const;

    using RftReportList = std::vector<std::tuple<std::string, RftDate, float>>;
    const RftReportList& listOfRftReports() const { return rftReportList; }

    bool hasRft(const std::string& wellName, const RftDate& date) const;
    bool hasRft(const std::string& wellName, int year, int month, int day) const;

    std::vector<EclEntry> listOfRftArrays(int reportIndex ) const;

    std::vector<EclEntry> listOfRftArrays(const std::string& wellName,
                                          const RftDate& date) const;

    std::vector<EclEntry> listOfRftArrays(const std::string& wellName,
                                          int year, int month, int day) const;

    bool hasArray(const std::string& arrayName, const std::string& wellName,
                  const RftDate& date) const;

    bool hasArray(const std::string& arrayName, int reportInd) const;

    int numberOfReports() { return numReports; }

private:
    std::map<int, std::tuple<int,int>> arrIndexRange;
    int numReports;
    std::vector<float> timeList;

    std::set<std::string> wellList;
    std::set<RftDate> dateList;
    RftReportList rftReportList;

    std::map<std::tuple<std::string,RftDate>,int> reportIndices;  //  mapping report index to wellName and date (tupe)

    int getReportIndex(const std::string& wellName, const RftDate& date) const;

    int getArrayIndex(const std::string& name, int reportIndex) const;
    int getArrayIndex(const std::string& name, const std::string& wellName,
                      const RftDate& date) const;
};

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_ERFT_H
