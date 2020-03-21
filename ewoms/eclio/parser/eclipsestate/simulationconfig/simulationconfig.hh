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

#ifndef EWOMS_SIMULATION_CONFIG_H
#define EWOMS_SIMULATION_CONFIG_H

#include <ewoms/eclio/parser/eclipsestate/simulationconfig/rockconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/simulationconfig/thresholdpressure.hh>
#include <ewoms/eclio/parser/eclipsestate/simulationconfig/bcconfig.hh>

namespace Ewoms {

    class Deck;
    class FieldPropsManager;

    class SimulationConfig {

    public:

        SimulationConfig();
        SimulationConfig(bool restart,
                         const Deck& deck,
                         const FieldPropsManager& fp);
        SimulationConfig(const ThresholdPressure& thresholdPressure,
                         const BCConfig& bc,
                         const RockConfig& rock_config,
                         bool useCPR, bool DISGAS, bool VAPOIL, bool isThermal);

        const RockConfig& rock_config() const;
        const ThresholdPressure& getThresholdPressure() const;
        const BCConfig& bcconfig() const;
        bool useThresholdPressure() const;
        bool useCPR() const;
        bool hasDISGAS() const;
        bool hasVAPOIL() const;
        bool isThermal() const;

        bool operator==(const SimulationConfig& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            m_ThresholdPressure.serializeOp(serializer);
            m_bcconfig.serializeOp(serializer);
            m_rock_config.serializeOp(serializer);
            serializer(m_useCPR);
            serializer(m_DISGAS);
            serializer(m_VAPOIL);
            serializer(m_isThermal);
        }

    private:
        ThresholdPressure m_ThresholdPressure;
        BCConfig m_bcconfig;
        RockConfig m_rock_config;
        bool m_useCPR;
        bool m_DISGAS;
        bool m_VAPOIL;
        bool m_isThermal;
    };

} //namespace Ewoms

#endif
