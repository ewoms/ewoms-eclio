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

#include <ewoms/eclio/parser/eclipsestate/schedule/action/actdims.hh>
#include <ewoms/eclio/parser/parserkeywords/a.hh>

namespace Ewoms {

Actdims::Actdims():
    keywords(ParserKeywords::ACTDIMS::MAX_ACTION::defaultValue),
    line_count(ParserKeywords::ACTDIMS::MAX_ACTION_LINES::defaultValue),
    characters(ParserKeywords::ACTDIMS::MAX_ACTION_LINE_CHARACTERS::defaultValue),
    conditions(ParserKeywords::ACTDIMS::MAX_ACTION_COND::defaultValue)
{}

Actdims::Actdims(const Deck& deck)
    : Actdims()
{
    if (deck.hasKeyword<ParserKeywords::ACTDIMS>()) {
        const auto& keyword = deck.getKeyword<ParserKeywords::ACTDIMS>();
        const auto& record = keyword.getRecord(0);

        this->keywords   = record.getItem<ParserKeywords::ACTDIMS::MAX_ACTION>().get<int>(0);
        this->line_count = record.getItem<ParserKeywords::ACTDIMS::MAX_ACTION_LINES>().get<int>(0);
        this->characters = record.getItem<ParserKeywords::ACTDIMS::MAX_ACTION_LINE_CHARACTERS>().get<int>(0);
        this->conditions = record.getItem<ParserKeywords::ACTDIMS::MAX_ACTION_COND>().get<int>(0);
    }
}

Actdims Actdims::serializeObject()
{
    Actdims result;
    result.keywords = 1;
    result.line_count = 2;
    result.characters = 3;
    result.conditions = 4;

    return result;
}

std::size_t Actdims::max_keywords() const {
    return this->keywords;
}

std::size_t Actdims::max_line_count() const {
    return this->line_count;
}

std::size_t Actdims::max_characters() const {
    return this->characters;
}

std::size_t Actdims::max_conditions() const {
    return this->conditions;
}

bool Actdims::operator==(const Actdims& data) const {
    return this->max_keywords() == data.max_keywords() &&
           this->max_line_count() == data.max_line_count() &&
           this->max_characters() == data.max_characters() &&
           this->max_conditions() == data.max_conditions();
}

}
