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
#ifndef EWOMS_IO_ERST_H
#define EWOMS_IO_ERST_H

#include <ewoms/eclio/io/eclfile.hh>

#include <ios>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace Ewoms { namespace EclIO { namespace OutputStream {
    class Restart;
}}}

namespace Ewoms { namespace EclIO {

class ERst : public EclFile
{
public:
    explicit ERst(const std::string& filename);

    bool hasReportStepNumber(int number) const;
    bool hasLGR(const std::string& gridname, int reportStepNumber) const;

    void loadReportStepNumber(int number);

    template <typename T>
    const std::vector<T>& getRestartData(const std::string& name, int reportStepNumber)
    {
        return getRestartData<T>(name,reportStepNumber, 0);
    }

    template <typename T>
    const std::vector<T>& getRestartData(const std::string& name, int reportStepNumber, int occurrence);

    template <typename T>
    const std::vector<T>& getRestartData(const std::string& name, int reportStepNumber, const std::string& lgr_name);

    template <typename T>
    const std::vector<T>& getRestartData(int index, int reportStepNumber)
    {
        auto indRange = this->getIndexRange(reportStepNumber);
        return  this->get<T>(index + std::get<0>(indRange));
    }

    template <typename T>
    const std::vector<T>& getRestartData(int index, int reportStepNumber, const std::string& lgr_name)
    {
        auto indRange = this->getIndexRange(reportStepNumber);

        if ((std::get<0>(indRange) + index) > std::get<1>(indRange))
            EWOMS_THROW(std::invalid_argument, "getRestartData, index out of range");

        int start_ind = get_start_index_lgrname(reportStepNumber, lgr_name);
        return  this->get<T>(index + start_ind);
    }

    int occurrence_count(const std::string& name, int reportStepNumber) const;
    size_t numberOfReportSteps() const { return seqnum.size(); };

    const std::vector<int>& listOfReportStepNumbers() const { return seqnum; }

    std::vector<EclEntry> listOfRstArrays(int reportStepNumber);
    std::vector<EclEntry> listOfRstArrays(int reportStepNumber, const std::string& lgr_name);

    friend class OutputStream::Restart;

private:
    int nReports;
    std::vector<int> seqnum;                           // report step numbers, from SEQNUM array in restart file
    mutable std::unordered_map<int,bool> reportLoaded;
    std::map<int, std::pair<int,int>> arrIndexRange;   // mapping report step number to array indeces (start and end)
    std::vector<std::vector<std::string>> lgr_names;                           // report step numbers, from SEQNUM array in restart file

    void initUnified();
    void initSeparate(const int number);

    int get_start_index_lgrname(int number, const std::string& lgr_name);

    int getArrayIndex(const std::string& name, int seqnum, int occurrence);
    int getArrayIndex(const std::string& name, int number, const std::string& lgr_name);

    std::tuple<int,int> getIndexRange(int reportStepNumber) const;

    std::streampos
    restartStepWritePosition(const int seqnumValue) const;

};

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_ERST_H
