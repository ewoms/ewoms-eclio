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
  along with eWoms.
*/

#include <string>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parserkeywords/b.hh>
#include <ewoms/eclio/parser/eclipsestate/simulationconfig/bcconfig.hh>

namespace Ewoms {
namespace {

namespace fromstring {

BCType bctype(const std::string& s) {
    if (s == "RATE")
        return BCType::RATE;

    if (s == "FREE")
        return BCType::FREE;

    throw std::invalid_argument("Not recognized boundary condition type: " + s);
}

BCComponent component(const std::string& s) {
    if (s == "OIL")
        return BCComponent::OIL;

    if (s == "GAS")
        return BCComponent::GAS;

    if (s == "WATER")
        return BCComponent::WATER;

    if (s == "SOLVENT")
        return BCComponent::SOLVENT;

    if (s == "POLYMER")
        return BCComponent::POLYMER;

    if (s == "NONE")
        return BCComponent::NONE;

    throw std::invalid_argument("Not recognized boundary condition compononet: " + s);
}

}
}

BCConfig::BCFace::BCFace(const DeckRecord& record) :
    i1(record.getItem("I1").get<int>(0) - 1),
    i2(record.getItem("I2").get<int>(0) - 1),
    j1(record.getItem("J1").get<int>(0) - 1),
    j2(record.getItem("J2").get<int>(0) - 1),
    k1(record.getItem("K1").get<int>(0) - 1),
    k2(record.getItem("K2").get<int>(0) - 1),
    bctype(fromstring::bctype(record.getItem("TYPE").get<std::string>(0))),
    dir(FaceDir::FromString(record.getItem("DIRECTION").get<std::string>(0))),
    component(fromstring::component(record.getItem("COMPONENT").get<std::string>(0))),
    rate(record.getItem("RATE").getSIDouble(0))
{
}

BCConfig::BCFace::BCFace(std::size_t i1_arg, std::size_t i2_arg,
                         std::size_t j1_arg, std::size_t j2_arg,
                         std::size_t k1_arg, std::size_t k2_arg,
                         BCType type_arg,
                         FaceDir::DirEnum dir_arg,
                         BCComponent comp_arg,
                         double rate_arg) :
    i1(i1_arg),
    i2(i2_arg),
    j1(j1_arg),
    j2(j2_arg),
    k1(k1_arg),
    k2(k2_arg),
    bctype(type_arg),
    dir(dir_arg),
    component(comp_arg),
    rate(rate_arg)
{}

bool BCConfig::BCFace::operator==(const BCConfig::BCFace& other) const {
    return this->i1 == other.i1 &&
           this->i2 == other.i2 &&
           this->j1 == other.j1 &&
           this->j2 == other.j2 &&
           this->k1 == other.k1 &&
           this->k2 == other.k2 &&
           this->bctype == other.bctype &&
           this->dir == other.dir &&
           this->component == other.component &&
           this->rate == other.rate;
}

BCConfig::BCConfig(const Deck& deck) {
    for (const auto& kw: deck.getKeywordList<ParserKeywords::BC>()) {
        for (const auto& record : *kw)
            this->m_faces.emplace_back( record );
    }
}

BCConfig::BCConfig(const std::vector<BCFace>& input_faces):
    m_faces(input_faces)
{}

const std::vector<BCConfig::BCFace>& BCConfig::faces() const {
    return this->m_faces;
}

std::size_t BCConfig::size() const {
    return this->m_faces.size();
}

std::vector<BCConfig::BCFace>::const_iterator BCConfig::begin() const {
    return this->m_faces.begin();
}

std::vector<BCConfig::BCFace>::const_iterator BCConfig::end() const {
    return this->m_faces.end();
}

bool BCConfig::operator==(const BCConfig& other) const {
    return this->m_faces == other.m_faces;
}

} //namespace Ewoms

