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

#ifndef AICD_HH_H
#define AICD_HH_H

#include <map>
#include <utility>
#include <vector>
#include <string>

#include <ewoms/eclio/parser/eclipsestate/schedule/msw/sicd.hh>

namespace Ewoms {

    class DeckRecord;
    class DeckKeyword;

    class AutoICD : public SICD {
    public:
        AutoICD() = default;
        AutoICD(const DeckRecord& record);

        static AutoICD serializeObject();

        // the function will return a map
        // [
        //     "WELL1" : [<seg1, aicd1>, <seg2, aicd2> ...]
        //     ....
        static std::map<std::string, std::vector<std::pair<int, AutoICD> > >
        fromWSEGAICD(const DeckKeyword& wsegaicd);

        bool operator==(const AutoICD& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            AutoICD::serializeOp(serializer);
        }

    private:
        double m_flow_rate_exponent;
        double m_visc_exponent;
        double m_oil_density_exponent;
        double m_water_density_exponent;
        double m_gas_density_exponent;
        double m_oil_viscosity_exponent;
        double m_water_viscosity_exponent;
        double m_gas_viscosity_exponent;
    };
}

#endif