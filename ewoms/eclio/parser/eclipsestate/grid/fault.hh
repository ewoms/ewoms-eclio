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
#ifndef FAULT_HH_
#define FAULT_HH_

#include <string>
#include <memory>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/grid/faultface.hh>

namespace Ewoms {

    class FaultFace;

class Fault {
public:
    Fault() = default;
    explicit Fault(const std::string& faultName);

    static Fault serializeObject();

    const  std::string& getName() const;
    void   setTransMult(double transMult);
    double getTransMult() const;
    void   addFace( FaultFace );
    std::vector< FaultFace >::const_iterator begin() const;
    std::vector< FaultFace >::const_iterator end() const;

    bool operator==( const Fault& rhs ) const;
    bool operator!=( const Fault& rhs ) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(m_name);
        serializer(m_transMult);
        serializer.vector(m_faceList);
    }

private:
    std::string m_name;
    double m_transMult = 0.0;
    std::vector< FaultFace > m_faceList;
};
}

#endif
