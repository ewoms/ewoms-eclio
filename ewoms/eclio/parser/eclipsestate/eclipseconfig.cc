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
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipseconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/initconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/restartconfig.hh>

namespace Ewoms {

    EclipseConfig::EclipseConfig(const Deck& deck, const ParseContext& parseContext, ErrorGuard& errors) :
            m_initConfig(      deck),
            m_restartConfig(   deck, parseContext, errors )
    {
    }

    EclipseConfig::EclipseConfig(const InitConfig& initConfig,
                                 const RestartConfig& restartConfig) :
        m_initConfig(initConfig),
        m_restartConfig(restartConfig)
    {
    }

    const InitConfig& EclipseConfig::init() const {
        return m_initConfig;
    }

    const IOConfig& EclipseConfig::io() const {
        return this->m_restartConfig.ioConfig();
    }

    IOConfig& EclipseConfig::io() {
        return this->m_restartConfig.ioConfig();
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

    bool EclipseConfig::operator==(const EclipseConfig& data) const {
        return this->init() == data.init() &&
               this->restart() == data.restart();
    }

}
