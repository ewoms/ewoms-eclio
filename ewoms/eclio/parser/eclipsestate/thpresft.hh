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
#ifndef EWOMS_ECLIO_EDITNNC_H
#define EWOMS_ECLIO_EDITNNC_H

#include <string>

namespace Ewoms {

struct ThpresftItem
{
    std::string faultName;
    double thresholdPressure;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(faultName);
        serializer(thresholdPressure);
    }
};

/// Represents edit information for non-neighboring connections (NNCs, faults, etc.)
class Thpresft
{
public:
    Thpresft() = default;

    /// Construct from input deck
    explicit Thpresft(const Deck& deck)
    {
        if (!deck.hasKeyword<ParserKeywords::THPRESFT>())
            return;

        const auto& thpresftKeyword = deck.getKeyword<ParserKeywords::THPRESFT>();

        for (size_t recordIdx = 0; recordIdx < thpresftKeyword.size(); ++ recordIdx) {
            const Ewoms::DeckRecord& record = thpresftKeyword.getRecord(recordIdx);

            ThpresftItem item;
            item.faultName = record.getItem("FAULT_NAME").getTrimmedString(0);
            item.thresholdPressure = record.getItem("VALUE").getSIDouble(0);
        }
    }

    /// \brief Get the array of threshold pressures accross faults
    const std::vector<ThpresftItem>& data() const
    { return htpresftItems_; }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    { serializer.vector(htpresftItems_); }

private:
    std::vector<ThpresftItem> htpresftItems_;
};

}

#endif
