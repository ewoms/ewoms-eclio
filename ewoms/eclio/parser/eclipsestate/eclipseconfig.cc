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
#include "config.h"

#include <memory>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipseconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/initconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/restartconfig.hh>

namespace Ewoms {

    EclipseConfig::EclipseConfig(const Deck& deck) :
        m_initConfig(deck),
        io_config(deck)
    {
    }

    EclipseConfig::EclipseConfig(const InitConfig& initConfig, const IOConfig& io_conf):
        m_initConfig(initConfig),
        io_config(io_conf)
    {
    }

    EclipseConfig EclipseConfig::serializeObject()
    {
        EclipseConfig result;
        result.m_initConfig = InitConfig::serializeObject();
        result.io_config = IOConfig::serializeObject();

        return result;
    }

    InitConfig& EclipseConfig::init() {
        return const_cast<InitConfig &>(this->m_initConfig);
    }

    const InitConfig& EclipseConfig::init() const{
        return m_initConfig;
    }

    // [[deprecated]] --- use init()
    const InitConfig& EclipseConfig::getInitConfig() const{
        return init();
    }

    bool EclipseConfig::operator==(const EclipseConfig& data) const {
        return this->init() == data.init();
    }

    IOConfig& EclipseConfig::io() {
        return const_cast<IOConfig &>(this->io_config);
    }

    const IOConfig& EclipseConfig::io() const {
        return this->io_config;
    }
}
