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

    void loadReportStepNumber(int number);

    template <typename T>
    const std::vector<T>& getRst(const std::string& name, int reportStepNumber, int occurrence);

    template <typename T>
    const std::vector<T>& getRst(int index, int reportStepNumber){
        auto indRange = this->getIndexRange(reportStepNumber);
        return  this->get<T>(index + std::get<0>(indRange));
    }

    int count(const std::string& name, int reportStepNumber) const;

    const std::vector<int>& listOfReportStepNumbers() const { return seqnum; }

    std::vector<EclEntry> listOfRstArrays(int reportStepNumber);

    friend class OutputStream::Restart;

private:
    int nReports;
    std::vector<int> seqnum;                           // report step numbers, from SEQNUM array in restart file
    std::unordered_map<int,bool> reportLoaded;
    std::map<int, std::pair<int,int>> arrIndexRange;   // mapping report step number to array indeces (start and end)

    void initUnified();
    void initSeparate(const int number);

    int getArrayIndex(const std::string& name, int seqnum, int occurrence) const;
    std::tuple<int,int> getIndexRange(int reportStepNumber) const;

    std::streampos
    restartStepWritePosition(const int seqnumValue) const;

};

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_ERST_H
