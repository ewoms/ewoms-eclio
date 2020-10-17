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

#define BOOST_TEST_MODULE FoamTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/foamconfig.hh>

using namespace Ewoms;

static Deck createDeck() {
    // Using a raw string literal with xxx as delimiter.
    const char *deckData = R"xxx(
RUNSPEC

DIMENS
 10 10 10 /
TABDIMS
3 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'F1'  1  1  1  4   1  4  'X' /
  'F2'  5  5  1  4   1  4  'X-' /
/
MULTFLT
  'F1' 0.50 /
  'F2' 0.50 /
/
PORO
   1000*0.15/
EDIT
MULTFLT /
  'F2' 0.25 /
/
WATER

OIL

GAS

FOAM

TITLE
The title

START
8 MAR 1998 /

PROPS

FOAMOPTS
GAS TAB /

FOAMFSC
1 2 0.3 /
4 5 /
6 /

FOAMROCK
1 2000 /
2 1800 /
2 2400 /

REGIONS
SWAT
1000*1 /
SATNUM
1000*2 /
)xxx"; // End of raw string literal with xxx as delimiter.
    Parser parser;
    return parser.parseString( deckData );
}

static Deck createFailingDeck() {
    // Using a raw string literal with xxx as delimiter.
    const char *deckData = R"xxx(
RUNSPEC

DIMENS
 10 10 10 /
TABDIMS
3 /
GRID
DX
1000*0.25 /
DY
1000*0.25 /
DZ
1000*0.25 /
TOPS
100*0.25 /
FAULTS
  'F1'  1  1  1  4   1  4  'X' /
  'F2'  5  5  1  4   1  4  'X-' /
/
MULTFLT
  'F1' 0.50 /
  'F2' 0.50 /
/
PORO
   1000*0.15/
EDIT
MULTFLT /
  'F2' 0.25 /
/
WATER

OIL

GAS

FOAM

TITLE
The title

START
8 MAR 1998 /

PROPS
FOAMFSC
1 2 0.3 /
4 5 /
6 /

-- This will fail, as FOAMROCK is missing

REGIONS
SWAT
1000*1 /
SATNUM
1000*2 /
)xxx"; // End of raw string literal with xxx as delimiter.
    Parser parser;
    return parser.parseString( deckData );
}

BOOST_AUTO_TEST_CASE(FoamConfigTest) {
    auto deck = createDeck();
    EclipseState state(deck);
    const FoamConfig& fc = state.getInitConfig().getFoamConfig();
    BOOST_REQUIRE_EQUAL(fc.size(), 3U);
    BOOST_CHECK_EQUAL(fc.getRecord(0).referenceSurfactantConcentration(), 1.0);
    BOOST_CHECK_EQUAL(fc.getRecord(0).exponent(), 2.0);
    BOOST_CHECK_EQUAL(fc.getRecord(0).minimumSurfactantConcentration(), 0.3);
    BOOST_CHECK(fc.getRecord(0).allowDesorption());
    BOOST_CHECK_EQUAL(fc.getRecord(0).rockDensity(), 2000.0);

    BOOST_CHECK_EQUAL(fc.getRecord(1).referenceSurfactantConcentration(), 4.0);
    BOOST_CHECK_EQUAL(fc.getRecord(1).exponent(), 5.0);
    BOOST_CHECK_EQUAL(fc.getRecord(1).minimumSurfactantConcentration(), 1e-20); // Defaulted.
    BOOST_CHECK(!fc.getRecord(1).allowDesorption());
    BOOST_CHECK_EQUAL(fc.getRecord(1).rockDensity(), 1800.0);

    BOOST_CHECK_EQUAL(fc.getRecord(2).referenceSurfactantConcentration(), 6.0);
    BOOST_CHECK_EQUAL(fc.getRecord(2).exponent(), 1.0); // Defaulted.
    BOOST_CHECK_EQUAL(fc.getRecord(2).minimumSurfactantConcentration(), 1e-20); // Defaulted.
    BOOST_CHECK(!fc.getRecord(2).allowDesorption());
    BOOST_CHECK_EQUAL(fc.getRecord(2).rockDensity(), 2400.0);
}

BOOST_AUTO_TEST_CASE(FoamConfigFailureTest) {
    auto deck = createFailingDeck();
    BOOST_CHECK_THROW(EclipseState state(deck), std::runtime_error);
}

