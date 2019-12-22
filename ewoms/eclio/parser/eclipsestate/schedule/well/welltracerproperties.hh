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

#ifndef WELLTRACERPROPERTIES_HH_H
#define WELLTRACERPROPERTIES_HH_H
#include <vector>
#include <string>
#include <map>

namespace Ewoms {

    class WellTracerProperties {

    public:
        using ConcentrationMap = std::map<std::string,double>;
        WellTracerProperties();
        WellTracerProperties(const std::map<std::string,double>& concentrations);

        void setConcentration(const std::string& name, const double& concentration);
        double getConcentration(const std::string& name) const;

        bool operator==(const WellTracerProperties& other) const;
        bool operator!=(const WellTracerProperties& other) const;

        const ConcentrationMap& getConcentrations() const;

    private:
        ConcentrationMap m_tracerConcentrations;
    };

}

#endif
