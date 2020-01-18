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

#ifndef EWOMS_INIT_CONFIG_H
#define EWOMS_INIT_CONFIG_H

#include <string>

#include <ewoms/eclio/parser/eclipsestate/initconfig/equil.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/foamconfig.hh>

namespace Ewoms {

    class Deck;

    class InitConfig {

    public:
        InitConfig();
        explicit InitConfig(const Deck& deck);
        InitConfig(const Equil& equil, const FoamConfig& foam,
                   bool filleps, bool gravity, bool restartReq, int restartStep,
                   const std::string& restartRootName);

        void setRestart( const std::string& root, int step);
        bool restartRequested() const;
        int getRestartStep() const;
        const std::string& getRestartRootName() const;

        bool hasEquil() const;
        const Equil& getEquil() const;

        bool hasGravity() const;

        bool hasFoamConfig() const;
        const FoamConfig& getFoamConfig() const;

        bool filleps() const
        {
            return this->m_filleps;
        }

        bool operator==(const InitConfig& config) const;

    private:
        Equil equil;
        FoamConfig foamconfig;
        bool m_filleps;
        bool m_gravity = true;

        bool m_restartRequested = false;
        int m_restartStep = 0;
        std::string m_restartRootName;
    };

} //namespace Ewoms

#endif
