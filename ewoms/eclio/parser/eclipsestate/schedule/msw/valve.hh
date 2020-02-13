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

#ifndef VALVE_HH_H
#define VALVE_HH_H

#include <map>
#include <utility>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/msw/icd.hh>

namespace Ewoms {

    class DeckRecord;
    class DeckKeyword;
    class Segment;

    class Valve {
    public:

        Valve();
        explicit Valve(const DeckRecord& record);
        Valve(double conEFlowCoeff,
              double conCrossA,
              double conMaxCrossA,
              double pipeAddLength,
              double pipeDiam,
              double pipeRough,
              double pipeCrossA,
              ICDStatus stat);

        // the function will return a map
        // [
        //     "WELL1" : [<seg1, valv1>, <seg2, valv2> ...]
        //     ....
        static std::map<std::string, std::vector<std::pair<int, Valve> > > fromWSEGVALV(const DeckKeyword& keyword);

        // parameters for constriction pressure loss
        double conEFlowCoefficient() const;
        double conCrossArea() const;
        double conMaxCrossArea() const;

        // parameters for pressure loss along the pipe
        double pipeAdditionalLength() const;
        double pipeDiameter() const;
        double pipeRoughness() const;
        double pipeCrossArea() const;

        // Status: OPEN or SHUT
        ICDStatus status() const;

        void setConMaxCrossArea(const double area);

        void setPipeAdditionalLength(const double length);
        void setPipeDiameter(const double dia);
        void setPipeRoughness(const double rou);
        void setPipeCrossArea(const double area);

        bool operator==(const Valve& data) const;

    private:
        double m_con_flow_coeff;
        double m_con_cross_area;
        double m_con_max_cross_area;

        double m_pipe_additional_length;
        double m_pipe_diameter;
        double m_pipe_roughness;
        double m_pipe_cross_area;
        ICDStatus m_status;
    };

}

#endif
