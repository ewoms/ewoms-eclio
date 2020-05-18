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

#include <chrono>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <stdint.h>

#include <ewoms/common/filesystem.hh>
#include <ewoms/eclio/io/summarynode.hh>

namespace Ewoms { namespace EclIO {

using ArrSourceEntry = std::tuple<std::string, std::string, int, uint64_t>;
using TimeStepEntry = std::tuple<int, int, uint64_t>;

class ESmry
{
public:

    // input is smspec (or fsmspec file)
    explicit ESmry(const std::string& filename, bool loadBaseRunData=false);

    int numberOfVectors() const { return nVect; }

    bool hasKey(const std::string& key) const;

    const std::vector<float>& get(const std::string& name) const;
    const std::vector<float>& get(const SummaryNode& node) const;
    std::vector<std::chrono::system_clock::time_point> dates() const;

    std::vector<float> get_at_rstep(const std::string& name) const;
    std::vector<float> get_at_rstep(const SummaryNode& node) const;
    std::vector<std::chrono::system_clock::time_point> dates_at_rstep() const;

    void LoadData(const std::vector<std::string>& vectList) const;
    void LoadData() const;

    bool make_lodsmry_file();

    std::chrono::system_clock::time_point startdate() const { return startdat; }

    const std::vector<std::string>& keywordList() const;
    std::vector<std::string> keywordList(const std::string& pattern) const;
    const std::vector<SummaryNode>& summaryNodeList() const;

    int timestepIdxAtReportstepStart(const int reportStep) const;

    size_t numberOfTimeSteps() const { return nTstep; }

    const std::string& get_unit(const std::string& name) const;
    const std::string& get_unit(const SummaryNode& node) const;

    void write_rsm(std::ostream&) const;
    void write_rsm_file(Ewoms::optional<Ewoms::filesystem::path> = Ewoms::nullopt) const;

private:
    Ewoms::filesystem::path inputFileName, lodFileName;
    int nI, nJ, nK, nSpecFiles;
    bool fromSingleRun, lodEnabeled;
    uint64_t lod_offset, lod_arr_size;
    size_t nVect, nTstep;

    std::vector<bool> formattedFiles;
    std::vector<std::string> dataFileList;
    mutable std::vector<std::vector<float>> vectorData;
    mutable std::vector<bool> vectorLoaded;
    std::vector<TimeStepEntry> timeStepList;
    std::vector<std::map<int, int>> arrayPos;
    std::vector<std::string> keyword;
    std::map<std::string, int> keyword_index;
    std::vector<int> nParamsSpecFile;

    std::vector<std::vector<std::string>> keywordListSpecFile;

    std::vector<int> seqIndex;

    void ijk_from_global_index(int glob, int &i, int &j, int &k) const;

    std::vector<SummaryNode> summaryNodes;
    std::unordered_map<std::string, std::string> kwunits;

    std::chrono::system_clock::time_point startdat;

    std::vector<std::string> checkForMultipleResultFiles(const Ewoms::filesystem::path& rootN, bool formatted) const;

    void getRstString(const std::vector<std::string>& restartArray,
                      Ewoms::filesystem::path& pathRst,
                      Ewoms::filesystem::path& rootN) const;

    void updatePathAndRootName(Ewoms::filesystem::path& dir, Ewoms::filesystem::path& rootN) const;

    std::string makeKeyString(const std::string& keyword, const std::string& wgname, int num) const;

    std::string unpackNumber(const SummaryNode&) const;
    std::string lookupKey(const SummaryNode&) const;

    void write_block(std::ostream &, bool write_dates, const std::vector<std::string>& time_column, const std::vector<SummaryNode>&) const;

    template <typename T>
    std::vector<T> rstep_vector(const std::vector<T>& full_vector) const {
        std::vector<T> result;
        result.reserve(seqIndex.size());

        for (const auto& ind : seqIndex){
            result.push_back(full_vector[ind]);
        }

        return result;
    }

    std::vector<std::tuple <std::string, uint64_t>> getListOfArrays(std::string filename, bool formatted);
    std::vector<int> makeKeywPosVector(int speInd) const;
    std::string read_string_from_disk(std::fstream& fileH, uint64_t size) const;
    void inspect_lodsmry();
    void Load_from_lodsmry(const std::vector<int>& keywIndVect) const;
};

}} // namespace Ewoms::EclIO

inline std::ostream& operator<<(std::ostream& os, const Ewoms::EclIO::ESmry& smry) {
    smry.write_rsm(os);

    return os;
}

#endif // EWOMS_IO_ESMRY_H
