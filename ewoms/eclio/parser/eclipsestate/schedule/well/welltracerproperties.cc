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
#include <ewoms/eclio/parser/eclipsestate/schedule/well/welltracerproperties.hh>

#include <string>
#include <vector>
#include <map>

namespace Ewoms {

    WellTracerProperties::WellTracerProperties() {
    }

    WellTracerProperties::WellTracerProperties(const ConcentrationMap& concentrations)
        : m_tracerConcentrations(concentrations)
    { }

    bool WellTracerProperties::operator==(const WellTracerProperties& other) const {
        if (m_tracerConcentrations == other.m_tracerConcentrations)
            return true;
        else
            return false;

    }

    void WellTracerProperties::setConcentration(const std::string& name, const double& concentration) {
        m_tracerConcentrations[name] = concentration;
    }

    double WellTracerProperties::getConcentration(const std::string& name) const {
        auto it = m_tracerConcentrations.find(name);
        if (it == m_tracerConcentrations.end())
            return 0.0;
        return it->second;
    }

    bool WellTracerProperties::operator!=(const WellTracerProperties& other) const {
        return !(*this == other);
    }

    const WellTracerProperties::ConcentrationMap& WellTracerProperties::getConcentrations() const {
        return m_tracerConcentrations;
    }
}