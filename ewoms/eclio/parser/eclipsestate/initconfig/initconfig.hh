// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
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

        static InitConfig serializeObject();

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

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            equil.serializeOp(serializer);
            foamconfig.serializeOp(serializer);
            serializer(m_filleps);
            serializer(m_gravity);
            serializer(m_restartRequested);
            serializer(m_restartStep);
            serializer(m_restartRootName);
        }

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
