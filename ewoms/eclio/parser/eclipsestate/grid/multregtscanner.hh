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

#ifndef EWOMS_PARSER_MULTREGTSCANNER_H
#define EWOMS_PARSER_MULTREGTSCANNER_H

#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>
#include <ewoms/eclio/parser/eclipsestate/util/value.hh>

namespace Ewoms {

    template< typename > class GridProperties;

    class DeckRecord;
    class DeckKeyword;

    namespace MULTREGT {

        enum NNCBehaviourEnum {
            NNC = 1,
            NONNC = 2,
            ALL = 3,
            NOAQUNNC = 4
        };

        std::string RegionNameFromDeckValue(const std::string& stringValue);
        NNCBehaviourEnum NNCBehaviourFromString(const std::string& stringValue);
    }

    struct MULTREGTRecord {
        int src_value;
        int target_value;
        double trans_mult;
        int directions;
        MULTREGT::NNCBehaviourEnum nnc_behaviour;
        std::string region_name;

        bool operator==(const MULTREGTRecord& data) const {
            return src_value == data.src_value &&
                   target_value == data.target_value &&
                   trans_mult == data.trans_mult &&
                   directions == data.directions &&
                   nnc_behaviour == data.nnc_behaviour &&
                   region_name == data.region_name;
        }
    };

    typedef std::map< std::pair<int , int> , const MULTREGTRecord * >  MULTREGTSearchMap;
    typedef std::tuple<size_t , FaceDir::DirEnum , double> MULTREGTConnection;

    class MULTREGTScanner {

    public:
        using ExternalSearchMap = std::map<std::string, std::map<std::pair<int,int>, int>>;

        MULTREGTScanner() = default;
        MULTREGTScanner(const MULTREGTScanner& data);
        MULTREGTScanner(const GridDims& grid,
                        const FieldPropsManager* fp_arg,
                        const std::vector< const DeckKeyword* >& keywords);
        MULTREGTScanner(const std::array<size_t,3>& size,
                        const std::vector<MULTREGTRecord>& records,
                        const ExternalSearchMap& searchMap,
                        const std::map<std::string, std::vector<int>>& region,
                        const std::string& defaultRegion);

        double getRegionMultiplier(size_t globalCellIdx1, size_t globalCellIdx2, FaceDir::DirEnum faceDir) const;

        std::array<size_t,3> getSize() const;
        const std::vector<MULTREGTRecord>& getRecords() const;
        ExternalSearchMap getSearchMap() const;
        const std::map<std::string, std::vector<int>>& getRegions() const;
        const std::string& getDefaultRegion() const;

        bool operator==(const MULTREGTScanner& data) const;
        MULTREGTScanner& operator=(const MULTREGTScanner& data);

    private:
        void constructSearchMap(const ExternalSearchMap& searchMap);

        void addKeyword( const DeckKeyword& deckKeyword, const std::string& defaultRegion);
        void assertKeywordSupported(const DeckKeyword& deckKeyword);
        std::size_t nx = 0,ny = 0, nz = 0;
        const FieldPropsManager* fp = nullptr;
        std::vector< MULTREGTRecord > m_records;
        std::map<std::string , MULTREGTSearchMap> m_searchMap;
        std::map<std::string, std::vector<int>> regions;
        std::string default_region;
    };

}

#endif // EWOMS_PARSER_MULTREGTSCANNER_H