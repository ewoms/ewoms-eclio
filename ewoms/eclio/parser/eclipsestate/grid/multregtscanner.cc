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
#include <stdexcept>
#include <map>
#include <set>

#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/multregtscanner.hh>

namespace Ewoms {

    namespace MULTREGT {

        std::string RegionNameFromDeckValue(const std::string& stringValue) {
            if (stringValue == "O")
                return "OPERNUM";

            if (stringValue == "F")
                return "FLUXNUM";

            if (stringValue == "M")
                return "MULTNUM";

            throw std::invalid_argument("The input string: " + stringValue + " was invalid. Expected: O/F/M");
        }

        NNCBehaviourEnum NNCBehaviourFromString(const std::string& stringValue) {
            if (stringValue == "ALL")
                return ALL;

            if (stringValue == "NNC")
                return NNC;

            if (stringValue == "NONNC")
                return NONNC;

            if (stringValue == "NOAQUNNC")
                return NOAQUNNC;

            throw std::invalid_argument("The input string: " + stringValue + " was invalid. Expected: ALL/NNC/NONNC/NOAQUNNC");
        }

    }

namespace {

std::vector<int> unique(const std::vector<int> data) {
    std::set<int> set_data(data.begin(), data.end());
    return { set_data.begin(), set_data.end() };
}

}

    /*****************************************************************/
    /*
      Observe that the (REGION1 -> REGION2) pairs behave like keys;
      i.e. for the MULTREGT keyword

        MULTREGT
          2  4   0.75    Z   ALL    M /
          2  4   2.50   XY   ALL    F /
        /

      The first record is completely overweritten by the second
      record, this is because the both have the (2 -> 4) region
      identifiers. This behaviourt is ensured by using a map with
      std::pair<region1,region2> as key.

      This function starts with some initial preprocessing to create a
      map which looks like this:

         searchMap = {"MULTNUM" : {std::pair(1,2) : std::tuple(TransFactor , Face , Region),
                                   std::pair(4,7) : std::tuple(TransFactor , Face , Region),
                                   ...},
                      "FLUXNUM" : {std::pair(4,8) : std::tuple(TransFactor , Face , Region),
                                   std::pair(1,4) : std::tuple(TransFactor , Face , Region),
                                   ...}}

      Then it will go through the different regions and looking for
      interface with the wanted region values.
    */
     MULTREGTScanner::MULTREGTScanner(const GridDims& grid,
                                      const FieldPropsManager* fp_arg,
                                      const std::vector< const DeckKeyword* >& keywords) :
         nx(grid.getNX()),
         ny(grid.getNY()),
         nz(grid.getNZ()),
         fp(fp_arg) {

        this->default_region = this->fp->default_region();
        for (size_t idx = 0; idx < keywords.size(); idx++)
            this->addKeyword(*keywords[idx] , this->default_region);

        MULTREGTSearchMap searchPairs;
        for (std::vector<MULTREGTRecord>::const_iterator record = m_records.begin(); record != m_records.end(); ++record) {
            const std::string& region_name = record->region_name;
            if (this->fp->has_int( region_name)) {
                int srcRegion    = record->src_value;
                int targetRegion = record->target_value;

                // the MULTREGT keyword is directional independent
                // i.e. we add it both ways to the lookup map.
                if (srcRegion != targetRegion) {
                    std::pair<int,int> pair1{ srcRegion, targetRegion };
                    std::pair<int,int> pair2{ targetRegion, srcRegion };
                    searchPairs[pair1] = &(*record);
                    searchPairs[pair2] = &(*record);
                }
            }
            else
                throw std::logic_error(
                                "MULTREGT record is based on region: "
                                +  region_name
                                + " which is not in the deck");

            if (this->regions.count(region_name) == 0)
                this->regions[region_name] = this->fp->get_global_int(region_name);
        }

        for (auto iter = searchPairs.begin(); iter != searchPairs.end(); ++iter) {
            const MULTREGTRecord * record = (*iter).second;
            std::pair<int,int> pair = (*iter).first;
            const std::string& keyword = record->region_name;
            if (m_searchMap.count(keyword) == 0)
                m_searchMap[keyword] = MULTREGTSearchMap();

            m_searchMap[keyword][pair] = record;
        }
    }

    MULTREGTScanner::MULTREGTScanner(const std::array<size_t,3>& size,
                                     const std::vector<MULTREGTRecord>& records,
                                     const ExternalSearchMap& searchMap,
                                     const std::map<std::string, std::vector<int>>& region,
                                     const std::string& defaultRegion) :
        nx(size[0]),
        ny(size[1]),
        nz(size[2]),
        m_records(records),
        regions(region),
        default_region(defaultRegion)
    {
        constructSearchMap(searchMap);
    }

    MULTREGTScanner::MULTREGTScanner(const MULTREGTScanner& data) {
        *this = data;
    }

    void MULTREGTScanner::assertKeywordSupported( const DeckKeyword& deckKeyword) {
        for (const auto& deckRecord : deckKeyword) {
            const auto& srcItem = deckRecord.getItem("SRC_REGION");
            const auto& targetItem = deckRecord.getItem("TARGET_REGION");
            auto nnc_behaviour = MULTREGT::NNCBehaviourFromString(deckRecord.getItem("NNC_MULT").get<std::string>(0));

            if (!srcItem.defaultApplied(0) && !targetItem.defaultApplied(0))
                if (srcItem.get<int>(0) == targetItem.get<int>(0))
                    throw std::invalid_argument("Sorry - MULTREGT applied internally to a region is not yet supported");

            if (nnc_behaviour == MULTREGT::NOAQUNNC)
                throw std::invalid_argument("Sorry - currently we do not support \'NOAQUNNC\' for MULTREGT.");

         }
    }

    void MULTREGTScanner::addKeyword( const DeckKeyword& deckKeyword , const std::string& defaultRegion) {
        assertKeywordSupported( deckKeyword );
        for (const auto& deckRecord : deckKeyword) {
            std::vector<int> src_regions;
            std::vector<int> target_regions;
            std::string region_name = defaultRegion;

            const auto& srcItem = deckRecord.getItem("SRC_REGION");
            const auto& targetItem = deckRecord.getItem("TARGET_REGION");
            const auto& regionItem = deckRecord.getItem("REGION_DEF");

            double trans_mult = deckRecord.getItem("TRAN_MULT").get<double>(0);
            auto directions = FaceDir::FromMULTREGTString( deckRecord.getItem("DIRECTIONS").get<std::string>(0));
            auto nnc_behaviour = MULTREGT::NNCBehaviourFromString(deckRecord.getItem("NNC_MULT").get<std::string>(0));

            if (regionItem.defaultApplied(0)) {
                if (!m_records.empty())
                    region_name = m_records.back().region_name;
            } else
                region_name = MULTREGT::RegionNameFromDeckValue( regionItem.get<std::string>(0) );

            if (srcItem.defaultApplied(0) || srcItem.get<int>(0) < 0)
                src_regions = unique(this->fp->get_int(region_name));
            else
                src_regions.push_back(srcItem.get<int>(0));

            if (targetItem.defaultApplied(0) || targetItem.get<int>(0) < 0)
                target_regions = unique(fp->get_int(region_name));
            else
                target_regions.push_back(targetItem.get<int>(0));

            for (int src_region : src_regions) {
                for (int target_region : target_regions)
                    m_records.push_back({src_region, target_region, trans_mult, directions, nnc_behaviour, region_name});
            }
        }

    }

    /*
      This function will check the region values in globalIndex1 and
      globalIndex and see if they match the regionvalues specified in
      the deck. The function checks both directions:

      Assume the relevant MULTREGT record looks like:

         1  2   0.10  XYZ  ALL M /

      I.e. we are checking for the boundary between regions 1 and
      2. We assign the transmissibility multiplier to the correct face
      of the cell with value 1:

         -----------
         | 1  | 2  |   =>  MultTrans( i,j,k,FaceDir::XPlus ) *= 0.50
         -----------

         -----------
         | 2  | 1  |   =>  MultTrans( i+1,j,k,FaceDir::XMinus ) *= 0.50
         -----------

    */
    double MULTREGTScanner::getRegionMultiplier(size_t globalIndex1 , size_t globalIndex2, FaceDir::DirEnum faceDir) const {

        for (auto iter = m_searchMap.begin(); iter != m_searchMap.end(); iter++) {
            const auto& region_data = this->regions.at( iter->first );
            const MULTREGTSearchMap& map = (*iter).second;

            int regionId1 = region_data[globalIndex1];
            int regionId2 = region_data[globalIndex2];

            std::pair<int,int> pair{ regionId1, regionId2 };
            if (map.count(pair) != 1 || !(map.at(pair)->directions & faceDir)) {
                pair = std::pair<int,int>{regionId2 , regionId1};
                if (map.count(pair) != 1 || !(map.at(pair)->directions & faceDir))
                    continue;
            }
            const MULTREGTRecord* record = map.at(pair);

            bool applyMultiplier = true;
            int i1 = globalIndex1 % this->nx;
            int i2 = globalIndex2 % this->nx;
            int j1 = globalIndex1 / this->nx % this->nz;
            int j2 = globalIndex2 / this->nx % this->nz;

            if (record->nnc_behaviour == MULTREGT::NNC){
                applyMultiplier = true;
                if ((std::abs(i1-i2) == 0 && std::abs(j1-j2) == 1) || (std::abs(i1-i2) == 1 && std::abs(j1-j2) == 0))
                    applyMultiplier = false;
            }
            else if (record->nnc_behaviour == MULTREGT::NONNC){
                applyMultiplier = false;
                if ((std::abs(i1-i2) == 0 && std::abs(j1-j2) == 1) || (std::abs(i1-i2) == 1 && std::abs(j1-j2) == 0))
                    applyMultiplier = true;
            }

            if (applyMultiplier)
                return record->trans_mult;

        }
        return 1;
    }

    std::array<size_t,3> MULTREGTScanner::getSize() const {
        return {nx, ny, nz};
    }

    const std::vector<MULTREGTRecord>& MULTREGTScanner::getRecords() const {
        return m_records;
    }

    const std::map<std::string, std::vector<int>>& MULTREGTScanner::getRegions() const {
        return regions;
    }

    const std::string& MULTREGTScanner::getDefaultRegion() const {
        return default_region;
    }

    MULTREGTScanner::ExternalSearchMap MULTREGTScanner::getSearchMap() const {
        ExternalSearchMap result;
        for (const auto& it : m_searchMap) {
            std::map<std::pair<int,int>, int> res;
            for (const auto& it2 : it.second) {
                auto ffunc = [&](const Ewoms::MULTREGTRecord& a)
                {
                    return &a == it2.second;
                };
                auto rIt = std::find_if(m_records.begin(), m_records.end(), ffunc);
                res[it2.first] = std::distance(m_records.begin(), rIt);
            }
            result[it.first] = res;
        }
        return result;
    }

    void MULTREGTScanner::constructSearchMap(const ExternalSearchMap& searchMap) {
        for (const auto& it : searchMap) {
            std::map<std::pair<int,int>, const Ewoms::MULTREGTRecord*> res;
            for (const auto& it2 : it.second) {
                res[it2.first] = &m_records[it2.second];
            }
            m_searchMap.insert({it.first, res});
        }
    }

    bool MULTREGTScanner::operator==(const MULTREGTScanner& data) const {
        return this->getSize() == data.getSize() &&
               this->getRecords() == data.getRecords() &&
               this->getRegions() == data.getRegions() &&
               this->getSearchMap() == data.getSearchMap() &&
               this->getDefaultRegion() == data.getDefaultRegion();
    }

    MULTREGTScanner& MULTREGTScanner::operator=(const MULTREGTScanner& data) {
        nx = data.nx;
        ny = data.ny;
        nz = data.nz;
        fp = data.fp;
        m_records = data.m_records;
        regions = data.regions;
        default_region = data.default_region;
        m_searchMap.clear();
        constructSearchMap(data.getSearchMap());

        return *this;
    }
}
