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
#ifndef EWOMS_PARSER_FAULT_COLLECTION_H
#define EWOMS_PARSER_FAULT_COLLECTION_H

#include <cstddef>
#include <string>

#include <ewoms/eclio/parser/eclipsestate/util/orderedmap.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fault.hh>

namespace Ewoms {

    class DeckRecord;
    class GridDims;
    class GRIDSection;

class FaultCollection {
public:
    FaultCollection();
    FaultCollection(const GRIDSection& gridSection, const GridDims& grid);
    FaultCollection(const OrderedMap<std::string, Fault>& faults);

    size_t size() const;
    bool hasFault(const std::string& faultName) const;
    Fault& getFault(const std::string& faultName);
    const Fault& getFault(const std::string& faultName) const;
    Fault& getFault(size_t faultIndex);
    const Fault& getFault(size_t faultIndex) const;

    /// we construct the fault based on faultName.  To get the fault: getFault
    void addFault(const std::string& faultName);
    void setTransMult(const std::string& faultName , double transMult);

    bool operator==(const FaultCollection& data) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        m_faults.serializeOp(serializer);
    }

private:
    void addFaultFaces(const GridDims& grid,
                       const DeckRecord&  faultRecord,
                       const std::string& faultName);
    OrderedMap<std::string, Fault> m_faults;

};
}

#endif // EWOMS_PARSER_FAULT_COLLECTION_H
