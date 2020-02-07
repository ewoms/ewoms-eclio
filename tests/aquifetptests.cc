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

#define BOOST_TEST_MODULE AquifetpTests

#include <boost/test/unit_test.hpp>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/aquifetp.hh>

using namespace Ewoms;

inline Deck createAquifetpDeck() {
  const char *deckData =
  "DIMENS\n"
  "3 3 3 /\n"
  "\n"
  "AQUDIMS\n"
  "1* 1* 2 100 1 1000 /\n"
  "GRID\n"
  "\n"
  "ACTNUM\n"
  " 0 8*1 0 8*1 0 8*1 /\n"
  "DXV\n"
  "1 1 1 /\n"
  "\n"
  "DYV\n"
  "1 1 1 /\n"
  "\n"
  "DZV\n"
  "1 1 1 /\n"
  "\n"
  "TOPS\n"
  "9*100 /\n"
  "\n"
  "PROPS\n"
  "AQUTAB\n"
  " 0.01 0.112 \n"
  " 0.05 0.229 /\n"
  "SOLUTION\n"
  "\n"
  "AQUFETP\n"
  "1  70000.0  4.0e3 2.0e9 1.0e-5	500 1 0	0 /\n"
  "/\n";

  Parser parser;
  return parser.parseString(deckData);
}

inline Deck createNullAquifetpDeck(){
  const char *deckData =
  "DIMENS\n"
  "3 3 3 /\n"
  "\n"
  "AQUDIMS\n"
  "1* 1* 2 100 1 1000 /\n"
  "GRID\n"
  "\n"
  "ACTNUM\n"
  " 0 8*1 0 8*1 0 8*1 /\n"
  "DXV\n"
  "1 1 1 /\n"
  "\n"
  "DYV\n"
  "1 1 1 /\n"
  "\n"
  "DZV\n"
  "1 1 1 /\n"
  "\n"
  "TOPS\n"
  "9*100 /\n"
  "\n"
  "PROPS\n"
  "AQUTAB\n"
  " 0.01 0.112 \n"
  " 0.05 0.229 /\n"
  "SOLUTION\n"
  ;

  Parser parser;
  return parser.parseString(deckData);
}

inline Deck createAquifetpDeck_defaultPressure(){
  const char *deckData =
  "DIMENS\n"
  "3 3 3 /\n"
  "\n"
  "AQUDIMS\n"
  "1* 1* 2 100 1 1000 /\n"
  "GRID\n"
  "\n"
  "ACTNUM\n"
  " 0 8*1 0 8*1 0 8*1 /\n"
  "DXV\n"
  "1 1 1 /\n"
  "\n"
  "DYV\n"
  "1 1 1 /\n"
  "\n"
  "DZV\n"
  "1 1 1 /\n"
  "\n"
  "TOPS\n"
  "9*100 /\n"
  "\n"
  "PROPS\n"
  "AQUTAB\n"
  " 0.01 0.112 \n"
  " 0.05 0.229 /\n"
  "SOLUTION\n"
  "\n"
  "AQUFETP\n"
  "1  70000.0  1* 2.0e9 1.0e-5	500 1 0	0 /\n"
  "/\n";

  Parser parser;
  return parser.parseString(deckData);
}

inline Aquifetp init_aquifetp(Deck& deck){
  Aquifetp aqufetp(deck);
  return aqufetp;
}

BOOST_AUTO_TEST_CASE(AquifetpTest){
  auto aqufetp_deck = createAquifetpDeck();
  const auto& aquifetp = init_aquifetp(aqufetp_deck);
  for (const auto& it : aquifetp){
    BOOST_CHECK_EQUAL(it.aquiferID , 1);
    BOOST_CHECK_EQUAL(it.V0, 2.0e9);
    BOOST_CHECK_EQUAL(it.J, 500/86400e5);
    BOOST_CHECK( it.p0.first );
  }
  const auto& data = aquifetp.data();
  Aquifetp aq2(data);
  BOOST_CHECK(aq2 == aquifetp);

  auto aqufetp_deck_null = createNullAquifetpDeck();
  const auto& aquifetp_null = init_aquifetp(aqufetp_deck_null);
  BOOST_CHECK_EQUAL(aquifetp_null.size(), 0);

  auto aqufetp_deck_default = createAquifetpDeck_defaultPressure();
  const auto& aquifetp_default = init_aquifetp(aqufetp_deck_default);
  for (const auto& it : aquifetp_default){
    BOOST_CHECK_EQUAL(it.aquiferID , 1);
    BOOST_CHECK_EQUAL(it.V0, 2.0e9);
    BOOST_CHECK_EQUAL(it.J, 500/86400e5);
    BOOST_CHECK( !it.p0.first );
  }

}
