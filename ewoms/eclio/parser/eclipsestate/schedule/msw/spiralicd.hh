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

#ifndef SPIRALICD_HH_H
#define SPIRALICD_HH_H

#include <map>
#include <utility>
#include <vector>
#include <string>

#include <ewoms/eclio/parser/eclipsestate/schedule/msw/icd.hh>

namespace Ewoms {

    class DeckRecord;
    class DeckKeyword;

    class SpiralICD {
    public:

        SpiralICD();
        explicit SpiralICD(const DeckRecord& record);
        SpiralICD(double strength,
                  double length,
                  double densityCalibration,
                  double viscosityCalibration,
                  double criticalValue,
                  double widthTransitionRegion,
                  double maxViscosityRatio,
                  int methodFlowScaling,
                  double maxAbsoluteRate,
                  ICDStatus status,
                  double scalingFactor);

        // the function will return a map
        // [
        //     "WELL1" : [<seg1, sicd1>, <seg2, sicd2> ...]
        //     ....
        static std::map<std::string, std::vector<std::pair<int, SpiralICD> > >
        fromWSEGSICD(const DeckKeyword& wsegsicd);

        double maxAbsoluteRate() const;
        ICDStatus status() const;
        double strength() const;
        double length() const;
        double densityCalibration() const;
        double viscosityCalibration() const;
        double criticalValue() const;
        double widthTransitionRegion() const;
        double maxViscosityRatio() const;
        int methodFlowScaling() const;

        void updateScalingFactor(const double segment_length, const double completion_length);
        double scalingFactor() const;
        int ecl_status() const;
        bool operator==(const SpiralICD& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_strength);
            serializer(m_length);
            serializer(m_density_calibration);
            serializer(m_viscosity_calibration);
            serializer(m_critical_value);
            serializer(m_width_transition_region);
            serializer(m_max_viscosity_ratio);
            serializer(m_method_flow_scaling);
            serializer(m_max_absolute_rate);
            serializer(m_status);
            serializer(m_scaling_factor);
        }

    private:
        double m_strength;
        double m_length;
        double m_density_calibration;
        double m_viscosity_calibration;
        double m_critical_value;
        double m_width_transition_region;
        double m_max_viscosity_ratio;
        int m_method_flow_scaling;
        double m_max_absolute_rate;
        ICDStatus m_status;
        // scaling factor is the only one can not be gotten from deck directly, needs to be
        // updated afterwards
        double m_scaling_factor;
};

}

#endif
