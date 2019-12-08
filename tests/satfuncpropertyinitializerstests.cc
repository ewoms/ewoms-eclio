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

#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE SatfuncPropertyInitializersTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/parser.hh>

using namespace Ewoms;

inline void check_property(const Eclipse3DProperties& props1,
                           const Eclipse3DProperties& props2,
                           const std::string& propertyName) {
    auto data1 = props1.getDoubleGridProperty(propertyName).getData();
    auto data2 = props2.getDoubleGridProperty(propertyName).getData();

    BOOST_CHECK_CLOSE(data1[0], data2[0], 1e-12);
}

BOOST_AUTO_TEST_CASE(SaturationFunctionFamilyTests) {
    const char * deckdefault =
            "RUNSPEC\n"
            "OIL\n"
            "GAS\n"
            "WATER\n"
            "DIMENS\n"
            " 1 1 1 /\n"
            "TABDIMS\n"
            "1 /\n"
            "\n"
            "GRID\n"
            "DX\n"
            "1*0.25 /\n"
            "DYV\n"
            "1*0.25 /\n"
            "DZ\n"
            "1*0.25 /\n"
            "TOPS\n"
            "1*0.25 /\n"
            "PORO \n"
            "1*0.10 /\n"
            "PERMX \n"
            "10.25 /\n";

    const char *family1 =
        "SWOF\n"
        " .2  .0 1.0 .0\n"   // So = 0.8
        " .3  .0  .8 .0\n"   // So = 0.7
        " .5  .5  .5 .0\n"   // So = 0.5
        " .8  .8  .0 .0\n"   // So = 0.2
        " 1.0 1.0 .0 .0 /\n" // So = 0.0
        "SGOF\n"
        " .0  .0 1.0 .0\n"   // So = 0.8
        " .1  .0  .3 .0\n"   // So = 0.7
        " .3  1*  .1 .0\n"   // So = 0.5
        " .5  .5  .0 .0\n"   // So = 0.3
        " .8 1.0  .0 .0/\n"; // So = 0.0

    const char *family2 =
        "SWFN\n"
        " .2  .0  .0\n"
        " .3  .0  .0\n"
        " .5  .5  .0\n"
        " .8  .8  .0\n"
        " 1.0 1.0 .0 /\n"
        "SGFN\n"
        " .0  .0  .0\n"
        " .1  .0  .0\n"
        " .5  .5  .0\n"
        " .7  .8  .0\n"
        " .8 1.0  .0/\n"
        "SOF3\n"
        " .0  .0  .0\n"
        " .2  .0  .0\n"
        " .3  1*  .0\n"
        " .5  .5  .1\n"
        " .7  .8  .3\n"
        " .8 1.0  1.0/\n";

    Parser parser;

    char family1Deck[500] = " ";
    strcat(family1Deck , deckdefault);
    strcat(family1Deck , family1);
    Deck deck1 = parser.parseString(family1Deck) ;
    Ewoms::TableManager tm1( deck1 );
    Ewoms::EclipseGrid grid1( deck1 );
    Ewoms::Eclipse3DProperties prop1( deck1, tm1, grid1 );

    char family2Deck[700] = " ";
    strcat(family2Deck , deckdefault);
    strcat(family2Deck , family2);
    Deck deck2 = parser.parseString(family2Deck) ;
    Ewoms::TableManager tm2( deck2 );
    Ewoms::EclipseGrid grid2( deck2 );
    Ewoms::Eclipse3DProperties prop2( deck2, tm2, grid2 );

    check_property(prop1, prop2, "SWL");
    check_property(prop1, prop2, "SWU");
    check_property(prop1, prop2, "SWCR");

    check_property(prop1, prop2, "SGL");
    check_property(prop1, prop2, "SGU");
    check_property(prop1, prop2, "SGCR");

    check_property(prop1, prop2, "SOWCR");
    check_property(prop1, prop2, "SOGCR");

    check_property(prop1, prop2, "PCW");
    check_property(prop1, prop2, "PCG");

    check_property(prop1, prop2, "KRW");
    check_property(prop1, prop2, "KRO");
    check_property(prop1, prop2, "KRG");

    char familyMixDeck[1000] = " ";
    strcat(familyMixDeck , deckdefault);
    strcat(familyMixDeck , family1);
    strcat(familyMixDeck , family2);

    Deck deckMix = parser.parseString(familyMixDeck) ;
    Ewoms::TableManager tmMix( deckMix );
    Ewoms::EclipseGrid gridMix( deckMix );
    Ewoms::Eclipse3DProperties propMix( deckMix, tmMix, gridMix );
    BOOST_CHECK_THROW(propMix.getDoubleGridProperty("SGCR") , std::invalid_argument);
}
