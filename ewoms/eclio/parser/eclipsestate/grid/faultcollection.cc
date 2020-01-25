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

#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultcollection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultface.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fault.hh>
#include <ewoms/eclio/parser/parserkeywords/f.hh>

namespace Ewoms {

    FaultCollection::FaultCollection()
    {}

    FaultCollection::FaultCollection(const GRIDSection& gridSection,
                                     const GridDims& grid) {
        const auto& faultKeywords = gridSection.getKeywordList<ParserKeywords::FAULTS>();

        for (auto keyword_iter = faultKeywords.begin(); keyword_iter != faultKeywords.end(); ++keyword_iter) {
            const auto& faultsKeyword = *keyword_iter;
            for (auto iter = faultsKeyword->begin(); iter != faultsKeyword->end(); ++iter) {
                const auto& faultRecord = *iter;
                const std::string& faultName = faultRecord.getItem(0).get< std::string >(0);

                addFaultFaces(grid, faultRecord, faultName);
            }
        }
    }

    FaultCollection::FaultCollection(const OrderedMap<std::string, Fault>& faults) :
        m_faults(faults)
    {
    }

    void FaultCollection::addFaultFaces(const GridDims& grid,
                                        const DeckRecord& faultRecord,
                                        const std::string& faultName)
    {
        int I1 = faultRecord.getItem(1).get<int>(0) - 1;
        int I2 = faultRecord.getItem(2).get<int>(0) - 1;
        int J1 = faultRecord.getItem(3).get<int>(0) - 1;
        int J2 = faultRecord.getItem(4).get<int>(0) - 1;
        int K1 = faultRecord.getItem(5).get<int>(0) - 1;
        int K2 = faultRecord.getItem(6).get<int>(0) - 1;
        FaceDir::DirEnum faceDir = FaceDir::FromString(faultRecord.getItem(7).get<std::string>(0));
        FaultFace face { grid.getNX(), grid.getNY(), grid.getNZ(),
                         size_t(I1), size_t(I2),
                         size_t(J1), size_t(J2),
                         size_t(K1), size_t(K2),
                         faceDir };

        if (!hasFault(faultName))
            addFault(faultName);

        getFault( faultName ).addFace( face );
    }

    size_t FaultCollection::size() const {
        return m_faults.size();
    }

    bool FaultCollection::hasFault(const std::string& faultName) const {
        return m_faults.count( faultName ) > 0;
    }

    const Fault& FaultCollection::getFault(const std::string& faultName) const {
        return m_faults.get( faultName );
    }

    Fault& FaultCollection::getFault(const std::string& faultName) {
        return m_faults.get( faultName );
    }

    Fault& FaultCollection::getFault(size_t faultIndex) {
        return m_faults.iget( faultIndex );
    }

    const Fault& FaultCollection::getFault(size_t faultIndex) const {
        return m_faults.iget( faultIndex );
    }

    void FaultCollection::addFault(const std::string& faultName) {
        Fault fault(faultName);
        m_faults.insert(std::make_pair(fault.getName() , fault));
    }

    void FaultCollection::setTransMult(const std::string& faultName , double transMult) {
        Fault& fault = getFault( faultName );
        fault.setTransMult( transMult );
    }

    const OrderedMap<std::string, Fault>& FaultCollection::getFaults() const {
        return m_faults;
    }

    bool FaultCollection::operator==(const FaultCollection& data) const {
        return this->getFaults() == data.getFaults();
    }
}
