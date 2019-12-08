/*

   This file is part of the eWoms project.

   eWoms is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   eWoms is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
   */

#include <ewoms/eclio/io/erft.hh>

#include <ewoms/eclio/errormacros.hh>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iterator>
#include <string>
#include <sstream>

namespace Ewoms { namespace EclIO {

ERft::ERft(const std::string &filename) : EclFile(filename)
{
    loadData();
    std::vector<int> first;

    std::vector<std::string> wellName;
    std::vector<RftDate> dates;

    auto listOfArrays = getList();

    for (size_t i = 0; i < listOfArrays.size(); i++) {
        std::string name = std::get<0>(listOfArrays[i]);

        if (name == "TIME") {
            first.push_back(i);
            auto vect1 = get<float>(i);
            timeList.push_back(vect1[0]);
        }

        if (name == "DATE") {
            auto vect1 = get<int>(i);
            RftDate date(vect1[2],vect1[1],vect1[0]);
            dateList.insert(date);
            dates.push_back(date);
        }

        if (name == "WELLETC"){
            auto vect1 = get<std::string>(i);
            wellList.insert(vect1[1]);
            wellName.push_back(vect1[1]);
        }
    }

    for (size_t i = 0; i < first.size(); i++) {
        std::pair<int,int> range;
        range.first = first[i];

        if (i == first.size() - 1) {
            range.second = listOfArrays.size();
        } else {
            range.second = first[i+1];
        }

        arrIndexRange[i] = range;
    }

    numReports = first.size();

    for (size_t i = 0; i < wellName.size(); i++) {
        std::pair<std::string,RftDate> wellDatePair(wellName[i],dates[i]);
        reportIndex[wellDatePair] = i;
        rftReportList.push_back(wellDatePair);
    }
}

bool ERft::hasRft(const std::string& wellName, const RftDate& date) const
{
    return reportIndex.find({wellName, date}) != reportIndex.end();
}

bool ERft::hasRft(const std::string& wellName, int year, int month, int day) const
{
    RftDate date(year, month, day);
    return reportIndex.find({wellName,date}) != reportIndex.end();
}

int ERft::getReportIndex(const std::string& wellName, const RftDate& date) const
{
    std::pair<std::string,std::tuple<int,int,int>> wellDatePair(wellName,date);
    auto rIndIt = reportIndex.find(wellDatePair);

    if (rIndIt == reportIndex.end()) {
        int y = std::get<0>(date);
        int m = std::get<1>(date);
        int d = std::get<2>(date);

        std::string dateStr=std::to_string(y) + "/" + std::to_string(m) + "/" + std::to_string(d);
        std::string message="RFT data not found for well  " + wellName + " at date: " + dateStr;
        EWOMS_THROW(std::invalid_argument, message);
    }

    return rIndIt->second;
}

bool ERft::hasArray(const std::string& arrayName, const std::string& wellName,
                    const RftDate& date) const
{
    int reportInd = getReportIndex(wellName, date);

    auto searchInd = arrIndexRange.find(reportInd);

    int fromInd = searchInd->second.first;
    int toInd = searchInd->second.second;

    auto it = std::find(array_name.begin()+fromInd,array_name.begin()+toInd,arrayName);
    return it != array_name.begin() + toInd;
}

int ERft::getArrayIndex(const std::string& name, const std::string& wellName,
                        const RftDate& date) const
{
    int rInd= getReportIndex(wellName, date);

    auto searchInd = arrIndexRange.find(rInd);

    int fromInd =searchInd->second.first;
    int toInd = searchInd->second.second;
    auto it=std::find(array_name.begin()+fromInd,array_name.begin()+toInd,name);

    if (std::distance(array_name.begin(),it) == toInd) {
        int y = std::get<0>(date);
        int m = std::get<1>(date);
        int d = std::get<2>(date);

        std::string dateStr = std::to_string(y) + "/" + std::to_string(m) + "/" + std::to_string(d);
        std::string message = "Array " + name + " not found for RFT, well: " + wellName + " date: " + dateStr;
        EWOMS_THROW(std::invalid_argument, message);
    }

    return std::distance(array_name.begin(),it);
}

template<> const std::vector<float>&
ERft::getRft<float>(const std::string& name, const std::string &wellName,
                    const RftDate& date) const
{
    int arrInd = getArrayIndex(name, wellName, date);

    if (array_type[arrInd] != REAL) {
        std::string message = "Array " + name + " found in RFT file for selected date and well, but called with wrong type";
        EWOMS_THROW(std::runtime_error, message);
    }

    auto search_array = real_array.find(arrInd);
    return search_array->second;
}

template<> const std::vector<double>&
ERft::getRft<double>(const std::string& name, const std::string& wellName,
                     const RftDate& date) const
{
    int arrInd = getArrayIndex(name, wellName, date);

    if (array_type[arrInd] != DOUB) {
        std::string message = "Array " + name + " found in RFT file for selected date and well, but called with wrong type";
        EWOMS_THROW(std::runtime_error, message);
    }

    auto search_array = doub_array.find(arrInd);
    return search_array->second;
}

template<> const std::vector<int>&
ERft::getRft<int>(const std::string& name, const std::string& wellName,
                  const RftDate& date) const
{
    int arrInd = getArrayIndex(name, wellName, date);

    if (array_type[arrInd] != INTE) {
        std::string message = "Array " + name + " found in RFT file for selected date and well, but called with wrong type";
        EWOMS_THROW(std::runtime_error, message);
    }

    auto search_array = inte_array.find(arrInd);
    return search_array->second;
}

template<> const std::vector<bool>&
ERft::getRft<bool>(const std::string& name, const std::string& wellName,
                   const RftDate& date) const
{
    int arrInd = getArrayIndex(name, wellName, date);

    if (array_type[arrInd] != LOGI) {
        std::string message = "Array " + name + " found in RFT file for selected date and well, but called with wrong type";
        EWOMS_THROW(std::runtime_error, message);
    }

    auto search_array = logi_array.find(arrInd);
    return search_array->second;
}

template<> const std::vector<std::string>&
ERft::getRft<std::string>(const std::string& name, const std::string& wellName,
                          const RftDate& date) const
{
    int arrInd = getArrayIndex(name, wellName, date);

    if (array_type[arrInd] != CHAR) {
        std::string message = "Array " + name + " found in RFT file for selected date and well, but called with wrong type";
        EWOMS_THROW(std::runtime_error, message);
    }

    auto search_array = char_array.find(arrInd);
    return search_array->second;
}

template<> const std::vector<int>&
ERft::getRft<int>(const std::string& name, const std::string& wellName,
                  int year, int month, int day) const
{
    return getRft<int>(name, wellName, RftDate{year, month, day});
}

template<> const std::vector<float>&
ERft::getRft<float>(const std::string& name, const std::string& wellName,
                    int year, int month, int day) const
{
    return getRft<float>(name, wellName, RftDate{year, month, day});
}

template<> const std::vector<double>&
ERft::getRft<double>(const std::string& name, const std::string& wellName,
                     int year, int month, int day) const
{
    return getRft<double>(name, wellName, RftDate{year, month, day});
}

template<> const std::vector<std::string>&
ERft::getRft<std::string>(const std::string& name, const std::string& wellName,
                          int year, int month, int day) const
{
    return getRft<std::string>(name, wellName, RftDate{year, month, day});
}

template<> const std::vector<bool>&
ERft::getRft<bool>(const std::string& name, const std::string& wellName,
                   int year, int month, int day) const
{
    return getRft<bool>(name, wellName, RftDate{year, month, day});
}

std::vector<EclFile::EclEntry> ERft::listOfRftArrays(const std::string& wellName,
                                                     const RftDate& date) const
{
    std::vector<EclEntry> list;
    int rInd = getReportIndex(wellName, date);

    auto searchInd = arrIndexRange.find(rInd);
    for (int i = searchInd->second.first; i < searchInd->second.second; i++) {
        list.emplace_back(array_name[i], array_type[i], array_size[i]);
    }

    return list;
}

std::vector<EclFile::EclEntry> ERft::listOfRftArrays(const std::string& wellName,
                                                     int year, int month, int day) const
{
    return listOfRftArrays(wellName, RftDate{year, month, day});
}

std::vector<std::string> ERft::listOfWells() const
{
    return { this->wellList.begin(), this->wellList.end() };
}

std::vector<ERft::RftDate> ERft::listOfdates() const
{
    return { this->dateList.begin(), this->dateList.end() };
}

}} // namespace Ewoms::ecl
