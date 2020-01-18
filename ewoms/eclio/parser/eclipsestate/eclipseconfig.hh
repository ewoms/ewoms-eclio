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

#ifndef EWOMS_ECLIPSE_CONFIG_H
#define EWOMS_ECLIPSE_CONFIG_H

#include <memory>

#include <ewoms/eclio/parser/eclipsestate/initconfig/initconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/restartconfig.hh>

namespace Ewoms {

    class Deck;
    class ParseContext;
    class ErrorGuard;

    class EclipseConfig
    {
    public:
        EclipseConfig() = default;
        EclipseConfig(const Deck& deck, const ParseContext& parseContext, ErrorGuard& errors);
        EclipseConfig(const IOConfig& ioConfig,
                      const InitConfig& initConfig,
                      const RestartConfig& restartConfig);

        const InitConfig& init() const;
        const IOConfig& io() const;
        IOConfig& io();
        const RestartConfig& restart() const;
        const InitConfig& getInitConfig() const;
        const RestartConfig& getRestartConfig() const;

        bool operator==(const EclipseConfig& data) const;

    private:
        IOConfig m_ioConfig;
        InitConfig m_initConfig;
        RestartConfig m_restartConfig;
    };
}

#endif // EWOMS_ECLIPSE_CONFIG_H
