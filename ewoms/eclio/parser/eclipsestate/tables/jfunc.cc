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

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/jfunc.hh>
#include <ewoms/eclio/parser/parserkeywords/j.hh>

namespace Ewoms {

    JFunc::JFunc()
        : m_flag(Flag::BOTH)
        , m_owSurfaceTension(0.0)
        , m_goSurfaceTension(0.0)
        , m_alphaFactor(0.5)
        , m_betaFactor(0.5)
        , m_direction(Direction::XY)
    {
    }

    JFunc::JFunc(const Deck& deck)
    {
        const auto& kw = *deck.getKeywordList<ParserKeywords::JFUNC>()[0];
        const auto& rec = kw.getRecord(0);
        const auto& kw_flag = rec.getItem("FLAG").get<std::string>(0);
        if (kw_flag == "BOTH")
            m_flag = Flag::BOTH;
        else if (kw_flag == "WATER")
            m_flag = Flag::WATER;
        else if (kw_flag == "GAS")
            m_flag = Flag::GAS;
        else
            throw std::invalid_argument("Illegal JFUNC FLAG, must be BOTH, WATER, or GAS.  Was \"" + kw_flag + "\".");

        if (m_flag != Flag::WATER)
            m_goSurfaceTension = rec.getItem("GO_SURFACE_TENSION").get<double>(0);

        if (m_flag != Flag::GAS)
            m_owSurfaceTension = rec.getItem("OW_SURFACE_TENSION").get<double>(0);

        m_alphaFactor = rec.getItem("ALPHA_FACTOR").get<double>(0);
        m_betaFactor = rec.getItem("BETA_FACTOR").get<double>(0);

        const auto kw_dir = rec.getItem("DIRECTION").get<std::string>(0);
        if (kw_dir == "XY")
            m_direction = Direction::XY;
        else if (kw_dir == "X")
            m_direction = Direction::X;
        else if (kw_dir == "Y")
            m_direction = Direction::Y;
        else if (kw_dir == "Z")
            m_direction = Direction::Z;
        else
            throw std::invalid_argument("Illegal JFUNC DIRECTION, must be XY, X, Y, or Z.  Was \"" + kw_dir + "\".");
    }

    JFunc JFunc::serializeObject()
    {
        JFunc result;
        result.m_flag = Flag::BOTH;
        result.m_owSurfaceTension = 1.0;
        result.m_goSurfaceTension = 2.0;
        result.m_alphaFactor = 3.0;
        result.m_betaFactor = 4.0;
        result.m_direction = Direction::X;

        return result;
    }

    double JFunc::alphaFactor() const {
        return m_alphaFactor;
    }

    double JFunc::betaFactor() const {
        return m_betaFactor;
    }

    double JFunc::goSurfaceTension() const {
        if (m_flag == JFunc::Flag::WATER)
            throw std::invalid_argument("Cannot get gas-oil with WATER JFUNC");
        return m_goSurfaceTension;
    }

    double JFunc::owSurfaceTension() const {
        if (m_flag == JFunc::Flag::GAS)
            throw std::invalid_argument("Cannot get oil-water with GAS JFUNC");
        return m_owSurfaceTension;
    }

    const JFunc::Flag& JFunc::flag() const {
        return m_flag;
    }

    const JFunc::Direction& JFunc::direction() const {
        return m_direction;
    }

    bool JFunc::operator==(const JFunc& data) const {
        return this->flag() == data.flag() &&
               this->owSurfaceTension() == data.owSurfaceTension() &&
               this->goSurfaceTension() == data.goSurfaceTension() &&
               this->alphaFactor() == data.alphaFactor() &&
               this->betaFactor() == data.betaFactor() &&
               this->direction() == data.direction();
    }

} // Ewoms::
