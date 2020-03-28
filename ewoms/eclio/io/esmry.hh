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

#ifndef EWOMS_IO_ESMRY_H
#define EWOMS_IO_ESMRY_H

#include <string>
#include <unordered_map>
#include <vector>

#include <ewoms/common/filesystem.hh>
#include <ewoms/eclio/io/summarynode.hh>

namespace Ewoms { namespace EclIO {

class ESmry
{
public:

    // input is smspec (or fsmspec file)
    explicit ESmry(const std::string& filename, bool loadBaseRunData=false);

    int numberOfVectors() const { return nVect; }

    bool hasKey(const std::string& key) const;

    const std::vector<float>& get(const std::string& name) const;
    const std::vector<float>& get(const SummaryNode& node) const;

    std::vector<float> get_at_rstep(const std::string& name) const;
    std::vector<float> get_at_rstep(const SummaryNode& node) const;

    const std::vector<int>& get_startdat() const { return startdat; }

    const std::vector<std::string>& keywordList() const;
    const std::vector<SummaryNode>& summaryNodeList() const;

    int timestepIdxAtReportstepStart(const int reportStep) const;

    size_t numberOfTimeSteps() const { return param[0].size(); }

    const std::string& get_unit(const std::string& name) const;
    const std::string& get_unit(const SummaryNode& node) const;

private:
    int nVect, nI, nJ, nK;

    void ijk_from_global_index(int glob, int &i, int &j, int &k) const;
    std::vector<std::vector<float>> param;
    std::vector<std::string> keyword;
    std::vector<SummaryNode> summaryNodes;
    std::unordered_map<std::string, std::string> kwunits;

    std::vector<int> seqIndex;
    std::vector<float> seqTime;

    // start of simulation year, month, day, hr, min, microsec*1000000
    std::vector<int> startdat;

    std::vector<std::string> checkForMultipleResultFiles(const Ewoms::filesystem::path& rootN, bool formatted) const;

    void getRstString(const std::vector<std::string>& restartArray,
                      Ewoms::filesystem::path& pathRst,
                      Ewoms::filesystem::path& rootN) const;

    void updatePathAndRootName(Ewoms::filesystem::path& dir, Ewoms::filesystem::path& rootN) const;

    std::string makeKeyString(const std::string& keyword, const std::string& wgname, int num) const;

    std::string unpackNumber(const SummaryNode&) const;
    std::string lookupKey(const SummaryNode&) const;
};

}} // namespace Ewoms::EclIO

#endif // EWOMS_IO_ESMRY_H
