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

#include <utility>

#include <ewoms/eclio/parser/parserkeywords/a.hh>
#include <ewoms/eclio/parser/eclipsestate/aquifetp.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>

namespace Ewoms {

using AQUFETP = ParserKeywords::AQUFETP;

Aquifetp::AQUFETP_data::AQUFETP_data(const DeckRecord& record) :
    aquiferID( record.getItem<AQUFETP::AQUIFER_ID>().get<int>(0)),
    pvttableID( record.getItem<AQUFETP::TABLE_NUM_WATER_PRESS>().get<int>(0)),
    J( record.getItem<AQUFETP::PI>().getSIDouble(0)),
    C_t( record.getItem<AQUFETP::C_T>().getSIDouble(0)),
    V0( record.getItem<AQUFETP::V0>().getSIDouble(0)),
    d0( record.getItem<AQUFETP::DAT_DEPTH>().getSIDouble(0)),
    p0(false, 0)
{
    if (record.getItem<AQUFETP::P0>().hasValue(0) )
        this->p0 = std::make_pair(true, record.getItem<AQUFETP::P0>().getSIDouble(0));
}

bool Aquifetp::AQUFETP_data::operator==(const Aquifetp::AQUFETP_data& other) const {
    return this->aquiferID == other.aquiferID &&
           this->pvttableID == other.pvttableID &&
           this->J == other.J &&
           this->C_t == other.C_t &&
           this->V0 == other.V0 &&
           this->d0 == other.d0 &&
           this->p0 == other.p0;
}

Aquifetp::Aquifetp(const std::vector<Aquifetp::AQUFETP_data>& data) :
    m_aqufetp(data)
{}

Aquifetp::Aquifetp(const Deck& deck)
{
    if (!deck.hasKeyword<AQUFETP>())
        return;

    const auto& aqufetpKeyword = deck.getKeyword<AQUFETP>();
    for (auto& record : aqufetpKeyword)
        this->m_aqufetp.emplace_back(record);
}

const std::vector<Aquifetp::AQUFETP_data>& Aquifetp::data() const
{
    return m_aqufetp;
}

bool Aquifetp::operator==(const Aquifetp& other) const {
    return this->m_aqufetp == other.m_aqufetp;
}

std::size_t Aquifetp::size() const {
    return this->m_aqufetp.size();
}

std::vector<Aquifetp::AQUFETP_data>::const_iterator Aquifetp::begin() const {
    return this->m_aqufetp.begin();
}

std::vector<Aquifetp::AQUFETP_data>::const_iterator Aquifetp::end() const {
    return this->m_aqufetp.end();
}

}
