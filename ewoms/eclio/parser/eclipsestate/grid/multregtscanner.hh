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
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
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
    };

    typedef std::map< std::pair<int , int> , const MULTREGTRecord * >  MULTREGTSearchMap;
    typedef std::tuple<size_t , FaceDir::DirEnum , double> MULTREGTConnection;

    class MULTREGTScanner {

    public:
        MULTREGTScanner(const GridDims& grid,
                        const FieldPropsManager& fp_arg,
                        const Eclipse3DProperties& e3DProps,
                        const std::vector< const DeckKeyword* >& keywords);
        double getRegionMultiplier(size_t globalCellIdx1, size_t globalCellIdx2, FaceDir::DirEnum faceDir) const;

    private:
        void addKeyword( const DeckKeyword& deckKeyword, const std::string& defaultRegion);
        void assertKeywordSupported(const DeckKeyword& deckKeyword);
        std::size_t nx,ny,nz;
        const FieldPropsManager& fp;
        const Eclipse3DProperties& m_e3DProps;
        std::vector< MULTREGTRecord > m_records;
        std::map<std::string , MULTREGTSearchMap> m_searchMap;
        std::map<std::string, std::vector<int>> regions;
        std::string default_region;
    };

}

#endif // EWOMS_PARSER_MULTREGTSCANNER_H
