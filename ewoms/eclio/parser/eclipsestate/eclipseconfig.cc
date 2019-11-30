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

#include <memory>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/section.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipseconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/initconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/restartconfig.hh>

namespace Ewoms {

    EclipseConfig::EclipseConfig(const Deck& deck, const ParseContext& parseContext, ErrorGuard& errors) :
            m_ioConfig(        deck),
            m_initConfig(      deck),
            m_restartConfig(   deck, parseContext, errors )
    {
    }

    const InitConfig& EclipseConfig::init() const {
        return m_initConfig;
    }

    const IOConfig& EclipseConfig::io() const {
        return m_ioConfig;
    }

    IOConfig& EclipseConfig::io() {
        return m_ioConfig;
    }

    const RestartConfig& EclipseConfig::restart() const {
        return this->m_restartConfig;
    }

    // [[deprecated]] --- use restart()
    const RestartConfig& EclipseConfig::getRestartConfig() const {
        return this->restart();
    }

    // [[deprecated]] --- use init()
    const InitConfig& EclipseConfig::getInitConfig() const {
        return init();
    }

}
