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

#define BOOST_TEST_MODULE COMPSEGUNITS

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parserkeywords/c.hh>

using namespace Ewoms;

inline Deck createCOMPSEGSDeck() {
    const char *deckData =
        "COMPSEGS\n"
        " WELL /\n"
        " 1 1 1 1  100  100  X  10/\n"
        "/\n";

    Parser parser;
    return parser.parseString(deckData);
}

BOOST_AUTO_TEST_CASE(CreateDimension) {
    auto deck = createCOMPSEGSDeck();
    const auto& keyword = deck.getKeyword<ParserKeywords::COMPSEGS>();
    const auto& record = keyword.getRecord(1);
    BOOST_CHECK_NO_THROW( record.getItem<ParserKeywords::COMPSEGS::DISTANCE_START>().getSIDouble(0) );
}

