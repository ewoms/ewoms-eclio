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

#define BOOST_TEST_MODULE MULTREGTScannerTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/parser.hh>

#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>

#include <ewoms/eclio/parser/eclipsestate/grid/multregtscanner.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

BOOST_AUTO_TEST_CASE(TestRegionName) {
    BOOST_CHECK_EQUAL( "FLUXNUM" , Ewoms::MULTREGT::RegionNameFromDeckValue( "F"));
    BOOST_CHECK_EQUAL( "MULTNUM" , Ewoms::MULTREGT::RegionNameFromDeckValue( "M"));
    BOOST_CHECK_EQUAL( "OPERNUM" , Ewoms::MULTREGT::RegionNameFromDeckValue( "O"));

    BOOST_CHECK_THROW( Ewoms::MULTREGT::RegionNameFromDeckValue("o") , std::invalid_argument);
    BOOST_CHECK_THROW( Ewoms::MULTREGT::RegionNameFromDeckValue("X") , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(TestNNCBehaviourEnum) {
    BOOST_CHECK_EQUAL( Ewoms::MULTREGT::ALL      , Ewoms::MULTREGT::NNCBehaviourFromString( "ALL"));
    BOOST_CHECK_EQUAL( Ewoms::MULTREGT::NNC      , Ewoms::MULTREGT::NNCBehaviourFromString( "NNC"));
    BOOST_CHECK_EQUAL( Ewoms::MULTREGT::NONNC    , Ewoms::MULTREGT::NNCBehaviourFromString( "NONNC"));
    BOOST_CHECK_EQUAL( Ewoms::MULTREGT::NOAQUNNC , Ewoms::MULTREGT::NNCBehaviourFromString( "NOAQUNNC"));

    BOOST_CHECK_THROW(  Ewoms::MULTREGT::NNCBehaviourFromString( "Invalid") , std::invalid_argument);
}

static Ewoms::Deck createInvalidMULTREGTDeck() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 3 3 2 /\n"
        "GRID\n"
        "DX\n"
        "18*0.25 /\n"
        "DY\n"
        "18*0.25 /\n"
        "DZ\n"
        "18*0.25 /\n"
        "TOPS\n"
        "9*0.25 /\n"
        "FLUXNUM\n"
        "1 1 2\n"
        "1 1 2\n"
        "1 1 2\n"
        "3 4 5\n"
        "3 4 5\n"
        "3 4 5\n"
        "/\n"
        "MULTREGT\n"
        "1  2   0.50   G   ALL    M / -- Invalid direction\n"
        "/\n"
        "MULTREGT\n"
        "1  2   0.50   X   ALL    G / -- Invalid region \n"
        "/\n"
        "MULTREGT\n"
        "1  2   0.50   X   ALL    M / -- Region not in deck \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(InvalidInput) {
    Ewoms::Deck deck = createInvalidMULTREGTDeck();
    Ewoms::EclipseGrid grid( deck );
    Ewoms::TableManager tm(deck);
    Ewoms::EclipseGrid eg( deck );
    Ewoms::FieldPropsManager fp(deck, Ewoms::Phases{true, true, true}, eg, tm);

    // Invalid direction
    std::vector<const Ewoms::DeckKeyword*> keywords0;
    const auto& multregtKeyword0 = deck.getKeyword( "MULTREGT", 0 );
    keywords0.push_back( &multregtKeyword0 );
    BOOST_CHECK_THROW( Ewoms::MULTREGTScanner scanner( grid, &fp, keywords0 ); , std::invalid_argument );

    // Not supported region
    std::vector<const Ewoms::DeckKeyword*> keywords1;
    const auto& multregtKeyword1 = deck.getKeyword( "MULTREGT", 1 );
    keywords1.push_back( &multregtKeyword1 );
    BOOST_CHECK_THROW( Ewoms::MULTREGTScanner scanner( grid, &fp, keywords1 ); , std::invalid_argument );

    // The keyword is ok; but it refers to a region which is not in the deck.
    std::vector<const Ewoms::DeckKeyword*> keywords2;
    const auto& multregtKeyword2 = deck.getKeyword( "MULTREGT", 2 );
    keywords2.push_back( &multregtKeyword2 );
    BOOST_CHECK_THROW( Ewoms::MULTREGTScanner scanner( grid, &fp, keywords2 ); , std::logic_error );
}

static Ewoms::Deck createNotSupportedMULTREGTDeck() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 3 3 2 /\n"
        "GRID\n"
        "DX\n"
        "18*0.25 /\n"
        "DY\n"
        "18*0.25 /\n"
        "DZ\n"
        "18*0.25 /\n"
        "TOPS\n"
        "9*0.25 /\n"
        "FLUXNUM\n"
        "1 1 2\n"
        "1 1 2\n"
        "1 1 2\n"
        "3 4 5\n"
        "3 4 5\n"
        "3 4 5\n"
        "/\n"
        "MULTREGT\n"
        "1  2   0.50   X   NOAQUNNC  F / -- Not support NOAQUNNC behaviour \n"
        "/\n"
        "MULTREGT\n"
        "2  2   0.50   X   ALL    M / -- Region values equal \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(NotSupported) {
    Ewoms::Deck deck = createNotSupportedMULTREGTDeck();
    Ewoms::EclipseGrid grid( deck );
    Ewoms::TableManager tm(deck);
    Ewoms::EclipseGrid eg( deck );
    Ewoms::FieldPropsManager fp(deck, Ewoms::Phases{true, true, true}, eg, tm);

    // Not support NOAQUNNC behaviour
    std::vector<const Ewoms::DeckKeyword*> keywords0;
    const auto& multregtKeyword0 = deck.getKeyword( "MULTREGT", 0 );
    keywords0.push_back( &multregtKeyword0 );
    BOOST_CHECK_THROW( Ewoms::MULTREGTScanner scanner( grid, &fp, keywords0 ); , std::invalid_argument );

    // srcValue == targetValue - not supported
    std::vector<const Ewoms::DeckKeyword*> keywords1;
    const Ewoms::DeckKeyword& multregtKeyword1 = deck.getKeyword( "MULTREGT", 1 );
    keywords1.push_back( &multregtKeyword1 );
    BOOST_CHECK_THROW( Ewoms::MULTREGTScanner scanner( grid, &fp, keywords1 ); , std::invalid_argument );
}

static Ewoms::Deck createDefaultedRegions() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 3 3 2 /\n"
        "GRID\n"
        "DX\n"
        "18*0.25 /\n"
        "DY\n"
        "18*0.25 /\n"
        "DZ\n"
        "18*0.25 /\n"
        "TOPS\n"
        "9*0.25 /\n"
        "FLUXNUM\n"
        "1 1 2\n"
        "1 1 2\n"
        "1 1 2\n"
        "3 4 5\n"
        "3 4 5\n"
        "3 4 5\n"
        "/\n"
        "MULTREGT\n"
        "3  4   1.25   XYZ   ALL    F /\n"
        "2  -1   0   XYZ   ALL    F / -- Defaulted from region value \n"
        "1  -1   0   XYZ   ALL    F / -- Defaulted from region value \n"
        "2  1   1      XYZ   ALL    F / Override default  \n"
        "/\n"
        "MULTREGT\n"
        "2  *   0.75   XYZ   ALL    F / -- Defaulted to region value \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(DefaultedRegions) {
  Ewoms::Deck deck = createDefaultedRegions();
  Ewoms::EclipseGrid grid( deck );
  Ewoms::TableManager tm(deck);
  Ewoms::EclipseGrid eg( deck );
  Ewoms::FieldPropsManager fp(deck, Ewoms::Phases{true, true, true}, eg, tm);

  std::vector<const Ewoms::DeckKeyword*> keywords0;
  const auto& multregtKeyword0 = deck.getKeyword( "MULTREGT", 0 );
  keywords0.push_back( &multregtKeyword0 );
  Ewoms::MULTREGTScanner scanner0(grid, &fp, keywords0);
  BOOST_CHECK_EQUAL( scanner0.getRegionMultiplier(grid.getGlobalIndex(0,0,1), grid.getGlobalIndex(1,0,1), Ewoms::FaceDir::XPlus ), 1.25);
  BOOST_CHECK_EQUAL( scanner0.getRegionMultiplier(grid.getGlobalIndex(1,0,0), grid.getGlobalIndex(2,0,0), Ewoms::FaceDir::XPlus ), 1.0);
  BOOST_CHECK_EQUAL( scanner0.getRegionMultiplier(grid.getGlobalIndex(2,0,1), grid.getGlobalIndex(2,0,0), Ewoms::FaceDir::ZMinus ), 0.0);

  std::vector<const Ewoms::DeckKeyword*> keywords1;
  const Ewoms::DeckKeyword& multregtKeyword1 = deck.getKeyword( "MULTREGT", 1 );
  keywords1.push_back( &multregtKeyword1 );
  Ewoms::MULTREGTScanner scanner1(grid, &fp, keywords1 );
  BOOST_CHECK_EQUAL( scanner1.getRegionMultiplier(grid.getGlobalIndex(2,0,0), grid.getGlobalIndex(1,0,0), Ewoms::FaceDir::XMinus ), 0.75);
  BOOST_CHECK_EQUAL( scanner1.getRegionMultiplier(grid.getGlobalIndex(2,0,0), grid.getGlobalIndex(2,0,1), Ewoms::FaceDir::ZPlus), 0.75);
}

static Ewoms::Deck createCopyMULTNUMDeck() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        "2 2 2 /\n"
        "GRID\n"
        "DX\n"
        "8*0.25 /\n"
        "DY\n"
        "8*0.25 /\n"
        "DZ\n"
        "8*0.25 /\n"
        "TOPS\n"
        "4*0.25 /\n"
        "FLUXNUM\n"
        "1 2\n"
        "1 2\n"
        "3 4\n"
        "3 4\n"
        "/\n"
        "COPY\n"
        " FLUXNUM  MULTNUM /\n"
        "/\n"
        "MULTREGT\n"
        "1  2   0.50/ \n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(MULTREGT_COPY_MULTNUM) {
    Ewoms::Deck deck = createCopyMULTNUMDeck();
    Ewoms::TableManager tm(deck);
    Ewoms::EclipseGrid eg(deck);
    Ewoms::FieldPropsManager fp(deck, Ewoms::Phases{true, true, true}, eg, tm);

    BOOST_CHECK_NO_THROW(fp.has_int("FLUXNUM"));
    BOOST_CHECK_NO_THROW(fp.has_int("MULTNUM"));
    const auto& fdata = fp.get_global_int("FLUXNUM");
    const auto& mdata = fp.get_global_int("MULTNUM");
    std::vector<int> data = { 1, 2, 1, 2, 3, 4, 3, 4 };

    for (auto i = 0; i < 2 * 2 * 2; i++) {
        BOOST_CHECK_EQUAL(fdata[i], mdata[i]);
        BOOST_CHECK_EQUAL(fdata[i], data[i]);
    }
}
