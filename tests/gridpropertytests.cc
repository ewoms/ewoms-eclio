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
#include <memory>

#define BOOST_TEST_MODULE EclipseGridTests

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/parser.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/deck/section.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

static const Ewoms::DeckKeyword createSATNUMKeyword( ) {
    const char* deckData =
    "SATNUM \n"
    "  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 / \n"
    "\n";

    Ewoms::Parser parser;
    Ewoms::Deck deck = parser.parseString(deckData);
    return deck.getKeyword("SATNUM");
}

static const Ewoms::DeckKeyword createTABDIMSKeyword( ) {
    const char* deckData =
    "TABDIMS\n"
    "  0 1 2 3 4 5 / \n"
    "\n";

    Ewoms::Parser parser;
    Ewoms::Deck deck = parser.parseString(deckData);
    return deck.getKeyword("TABDIMS");
}

BOOST_AUTO_TEST_CASE(Empty) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo("SATNUM" , 77, "1");
    Ewoms::GridProperty<int> gridProperty( 5 , 5 , 4 , keywordInfo);
    const std::vector<int>& data = gridProperty.getData();
    BOOST_CHECK_EQUAL( 100U , data.size());
    BOOST_CHECK_EQUAL( 100U , gridProperty.getCartesianSize());
    BOOST_CHECK_EQUAL( 5U , gridProperty.getNX());
    BOOST_CHECK_EQUAL( 5U , gridProperty.getNY());
    BOOST_CHECK_EQUAL( 4U , gridProperty.getNZ());
    for (size_t k=0; k < 4; k++) {
        for (size_t j=0; j < 5; j++) {
            for (size_t i=0; i < 5; i++) {
                size_t g = i + j*5 + k*25;
                BOOST_CHECK_EQUAL( 77 , data[g] );
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(HasNAN) {
    double nan = std::numeric_limits<double>::quiet_NaN();
    typedef Ewoms::GridProperty<double>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo("PORO" , nan , "1");
    Ewoms::GridProperty<double> poro( 2 , 2 , 1 , keywordInfo);

    BOOST_CHECK( poro.containsNaN() );
    auto data = poro.getData();
    data[0] = 0.15;
    data[1] = 0.15;
    data[2] = 0.15;
    poro.assignData(data);
    BOOST_CHECK( poro.containsNaN() );

    data[3] = 0.15;
    poro.assignData(data);
    BOOST_CHECK( !poro.containsNaN() );
}

BOOST_AUTO_TEST_CASE(EmptyDefault) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo("SATNUM" , 0, "1");
    Ewoms::GridProperty<int> gridProperty( /*nx=*/10,
                                         /*ny=*/10,
                                         /*nz=*/1 ,
                                         keywordInfo);
    const std::vector<int>& data = gridProperty.getData();
    BOOST_CHECK_EQUAL( 100U , data.size());
    for (size_t i=0; i < data.size(); i++)
        BOOST_CHECK_EQUAL( 0 , data[i] );
}

BOOST_AUTO_TEST_CASE(SetFromDeckKeyword_notData_Throws) {
    const auto& tabdimsKw = createTABDIMSKeyword();
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo("TABDIMS" , 100, "1");
    Ewoms::GridProperty<int> gridProperty( 6 ,1,1 , keywordInfo);
    BOOST_CHECK_THROW( gridProperty.loadFromDeckKeyword( tabdimsKw, false ) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(SetFromDeckKeyword_wrong_size_throws) {
    const auto& satnumKw = createSATNUMKeyword();
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo("SATNUM" , 66, "1");
    Ewoms::GridProperty<int> gridProperty( 15 ,1,1, keywordInfo);
    BOOST_CHECK_THROW( gridProperty.loadFromDeckKeyword( satnumKw, false ) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(SetFromDeckKeyword) {
    const auto& satnumKw = createSATNUMKeyword();
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo("SATNUM" , 99, "1");
    Ewoms::GridProperty<int> gridProperty( 4 , 4 , 2 , keywordInfo);
    gridProperty.loadFromDeckKeyword( satnumKw, false );
    const std::vector<int>& data = gridProperty.getData();
    for (size_t k=0; k < 2; k++) {
        for (size_t j=0; j < 4; j++) {
            for (size_t i=0; i < 4; i++) {
                size_t g = i + j*4 + k*16;

                BOOST_CHECK_EQUAL( g , data[g] );

            }
        }
    }
}

BOOST_AUTO_TEST_CASE(copy) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo1("P1", 0, "1");
    SupportedKeywordInfo keywordInfo2("P2", 9, "1");
    Ewoms::GridProperty<int> prop1(4, 4, 2, keywordInfo1);
    Ewoms::GridProperty<int> prop2(4, 4, 2, keywordInfo2);
    Ewoms::EclipseGrid grid(4,4,2);
    Ewoms::Box global(grid);
    Ewoms::Box layer0(grid, 0, 3, 0, 3, 0, 0);

    prop2.copyFrom(prop1, layer0);
    const auto& prop2_data = prop2.getData();

    for (size_t j = 0; j < 4; j++) {
        for (size_t i = 0; i < 4; i++) {
            std::size_t g1 = i + j*4;
            std::size_t g2 = g1 + 16;

            BOOST_CHECK_EQUAL(prop2_data[g1], 0);
            BOOST_CHECK_EQUAL(prop2_data[g2], 9);
        }
    }
}

BOOST_AUTO_TEST_CASE(SCALE) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo1( "P1", 1, "1" );
    SupportedKeywordInfo keywordInfo2( "P2", 9, "1" );

    Ewoms::GridProperty<int> prop1( 4, 4, 2, keywordInfo1 );
    Ewoms::GridProperty<int> prop2( 4, 4, 2, keywordInfo2 );

    Ewoms::EclipseGrid grid(4,4,2);
    Ewoms::Box global(grid);
    Ewoms::Box layer0(grid, 0, 3, 0, 3, 0, 0);

    prop2.copyFrom( prop1, layer0 );
    prop2.scale( 2, global );
    prop2.scale( 2, layer0 );
    const auto& prop2_data = prop2.getData();

    for (size_t j = 0; j < 4; j++) {
        for (size_t i = 0; i < 4; i++) {
            std::size_t g1 = i + j*4;
            std::size_t g2 = g1 + 16;

            BOOST_CHECK_EQUAL( prop2_data[g1], 4 );
            BOOST_CHECK_EQUAL( prop2_data[g2], 18 );
        }
    }
}

BOOST_AUTO_TEST_CASE(SET) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo( "P1", 1, "1" );
    Ewoms::GridProperty<int> prop( 4, 4, 2, keywordInfo );

    Ewoms::EclipseGrid grid(4,4,2);
    Ewoms::Box global(grid);
    Ewoms::Box layer0(grid, 0, 3, 0, 3, 0, 0);

    prop.setScalar( 2, global );
    prop.setScalar( 4, layer0 );
    const auto& prop_data = prop.getData();

    for (size_t j = 0; j < 4; j++) {
        for (size_t i = 0; i < 4; i++) {
            std::size_t g1 = i + j*4;
            std::size_t g2 = g1 + 16;

            BOOST_CHECK_EQUAL( prop_data[g1], 4 );
            BOOST_CHECK_EQUAL( prop_data[g2], 2 );
        }
    }
}

BOOST_AUTO_TEST_CASE(ADD) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo1( "P1", 1, "1", true );
    SupportedKeywordInfo keywordInfo2( "P2", 9, "1", true );
    Ewoms::GridProperty<int> prop1( 4, 4, 2, keywordInfo1 );
    Ewoms::GridProperty<int> prop2( 4, 4, 2, keywordInfo2 );

    Ewoms::EclipseGrid grid(4,4,2);
    Ewoms::Box global(grid);
    Ewoms::Box layer0(grid, 0, 3, 0, 3, 0, 0);

    prop2.copyFrom( prop1, layer0 );
    prop2.add( 2, global );
    prop2.add( 2, layer0 );
    const auto& prop2_data = prop2.getData();

    for (size_t j = 0; j < 4; j++) {
        for (size_t i = 0; i < 4; i++) {
            std::size_t g1 = i + j*4;
            std::size_t g2 = g1 + 16;

            BOOST_CHECK_EQUAL( prop2_data[g1], 5 );
            BOOST_CHECK_EQUAL( prop2_data[g2], 11 );
        }
    }
}

BOOST_AUTO_TEST_CASE(GridPropertyInitialization) {
    const char* deckString =
        "RUNSPEC\n"
        "\n"
        "OIL\n"
        "GAS\n"
        "WATER\n"
        "TABDIMS\n"
        "3 /\n"
        "\n"
        "METRIC\n"
        "\n"
        "DIMENS\n"
        "3 3 3 /\n"
        "\n"
        "GRID\n"
        "\n"
        "PERMX\n"
        " 27*1000 /\n"
        "MAXVALUE\n"
        "  PERMX 100 4* 1  1/\n"
        "/\n"
        "MINVALUE\n"
        "  PERMX 10000 4* 3  3/\n"
        "/\n"
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
        "\n"
        "SWOF\n"
        // table 1
        // S_w    k_r,w    k_r,o    p_c,ow
        "  0.1    0        1.0      2.0\n"
        "  0.15   0        0.9      1.0\n"
        "  0.2    0.01     0.5      0.5\n"
        "  0.93   0.91     0.0      0.0\n"
        "/\n"
        // table 2
        // S_w    k_r,w    k_r,o    p_c,ow
        "  0.00   0        1.0      2.0\n"
        "  0.05   0.01     1.0      2.0\n"
        "  0.10   0.02     0.9      1.0\n"
        "  0.15   0.03     0.5      0.5\n"
        "  0.852  1.00     0.0      0.0\n"
        "/\n"
        // table 3
        // S_w    k_r,w    k_r,o    p_c,ow
        "  0.00   0.00     0.9      2.0\n"
        "  0.05   0.02     0.8      1.0\n"
        "  0.10   0.03     0.5      0.5\n"
        "  0.801  1.00     0.0      0.0\n"
        "/\n"
        "\n"
        "SGOF\n"
        // table 1
        // S_g    k_r,g    k_r,o    p_c,og
        "  0.00   0.00     0.9      2.0\n"
        "  0.05   0.02     0.8      1.0\n"
        "  0.10   0.03     0.5      0.5\n"
        "  0.80   1.00     0.0      0.0\n"
        "/\n"
        // table 2
        // S_g    k_r,g    k_r,o    p_c,og
        "  0.05   0.00     1.0      2\n"
        "  0.10   0.02     0.9      1\n"
        "  0.15   0.03     0.5      0.5\n"
        "  0.85   1.00     0.0      0\n"
        "/\n"
        // table 3
        // S_g    k_r,g    k_r,o    p_c,og
        "  0.1    0        1.0      2\n"
        "  0.15   0        0.9      1\n"
        "  0.2    0.01     0.5      0.5\n"
        "  0.9    0.91     0.0      0\n"
        "/\n"
        "\n"
        "SWU\n"
        "27* /\n"
        "\n"
        "ISGU\n"
        "27* /\n"
        "\n"
        "SGCR\n"
        "27* /\n"
        "\n"
        "ISGCR\n"
        "27* /\n"
        "\n"
        "REGIONS\n"
        "\n"
        "SATNUM\n"
        "9*1 9*2 9*3 /\n"
        "\n"
        "IMBNUM\n"
        "9*3 9*2 9*1 /\n"
        "\n"
        "SOLUTION\n"
        "\n"
        "SCHEDULE\n";

    Ewoms::Parser parser;

    auto deck = parser.parseString(deckString);
    Ewoms::TableManager tm(deck);
    Ewoms::EclipseGrid eg(deck);
    Ewoms::Eclipse3DProperties props(deck, tm, eg);

    // make sure that Eclipse3DProperties throws if it is bugged about an _unsupported_ keyword
    BOOST_CHECK_THROW(props.hasDeckIntGridProperty("ISWU"), std::logic_error);
    BOOST_CHECK_THROW(props.hasDeckDoubleGridProperty("FLUXNUM"), std::logic_error);

    // make sure that Eclipse3DProperties does not throw if it is asked for a supported
    // grid property that is not contained in the deck
    BOOST_CHECK_NO_THROW(props.hasDeckDoubleGridProperty("ISWU"));
    BOOST_CHECK_NO_THROW(props.hasDeckIntGridProperty("FLUXNUM"));

    BOOST_CHECK(!props.hasDeckDoubleGridProperty("ISWU"));
    BOOST_CHECK(!props.hasDeckIntGridProperty("FLUXNUM"));

    BOOST_CHECK(props.hasDeckIntGridProperty("SATNUM"));
    BOOST_CHECK(props.hasDeckIntGridProperty("IMBNUM"));

    BOOST_CHECK(props.hasDeckDoubleGridProperty("SWU"));
    BOOST_CHECK(props.hasDeckDoubleGridProperty("ISGU"));
    BOOST_CHECK(props.hasDeckDoubleGridProperty("SGCR"));
    BOOST_CHECK(props.hasDeckDoubleGridProperty("ISGCR"));

    const auto& swuPropData = props.getDoubleGridProperty("SWU").getData();
    BOOST_CHECK_EQUAL(swuPropData[0 * 3*3], 0.93);
    BOOST_CHECK_EQUAL(swuPropData[1 * 3*3], 0.852);
    BOOST_CHECK_EQUAL(swuPropData[2 * 3*3], 0.801);

    const auto& sguPropData = props.getDoubleGridProperty("ISGU").getData();
    BOOST_CHECK_EQUAL(sguPropData[0 * 3*3], 0.9);
    BOOST_CHECK_EQUAL(sguPropData[1 * 3*3], 0.85);
    BOOST_CHECK_EQUAL(sguPropData[2 * 3*3], 0.80);

    const auto& satnum = props.getIntGridProperty("SATNUM");
    {
        const auto& activeMap = eg.getActiveMap();
        const auto cells1 = satnum.cellsEqual(1 , activeMap);
        const auto cells2 = satnum.cellsEqual(2 , activeMap);
        const auto cells3 = satnum.cellsEqual(3 , activeMap);

        BOOST_CHECK_EQUAL( cells1.size() , 8 );
        BOOST_CHECK_EQUAL( cells2.size() , 8 );
        BOOST_CHECK_EQUAL( cells3.size() , 8 );

        for (size_t i = 0; i < 8; i++) {
            BOOST_CHECK_EQUAL( cells1[i] , i );
            BOOST_CHECK_EQUAL( cells2[i] , i + 8);
            BOOST_CHECK_EQUAL( cells3[i] , i + 16);
        }
    }
    {
        const auto cells1 = satnum.indexEqual(1 );
        const auto cells2 = satnum.indexEqual(2 );
        const auto cells3 = satnum.indexEqual(3 );

        BOOST_CHECK_EQUAL( cells1.size() , 9 );
        BOOST_CHECK_EQUAL( cells2.size() , 9 );
        BOOST_CHECK_EQUAL( cells3.size() , 9 );

        for (size_t i = 0; i < 9; i++) {
            BOOST_CHECK_EQUAL( cells1[i] , i );
            BOOST_CHECK_EQUAL( cells2[i] , i + 9);
            BOOST_CHECK_EQUAL( cells3[i] , i + 18);
        }
    }

    {
        const auto cells3_a = satnum.cellsEqual(3 , eg);
        const auto cells3_g = satnum.cellsEqual(3 , eg , false);

        for (size_t i = 0; i < 8; i++) {
            BOOST_CHECK_EQUAL( cells3_a[i] , i + 16);
            BOOST_CHECK_EQUAL( cells3_g[i] , i + 18);
        }
        BOOST_CHECK_EQUAL( cells3_g[8] , 26);
    }

    const auto compressedSatnum = satnum.compressedCopy( eg );
    BOOST_CHECK_EQUAL( compressedSatnum.size() , eg.getNumActive());
    for (size_t i=0; i < eg.getNumActive(); i++) {
        size_t g = eg.getGlobalIndex( i );
        BOOST_CHECK_EQUAL( compressedSatnum[i] , satnum.getData()[g]);
    }

    {
        const auto& double_props = props.getDoubleProperties( );
        BOOST_CHECK( !double_props.hasKeyword( "NTG" ));
        double_props.assertKeyword("NTG");
        BOOST_CHECK( double_props.hasKeyword( "NTG" ));
    }
    {
        const auto& double_props = props.getDoubleProperties( );
        const auto& units = deck.getActiveUnitSystem();
        const auto& permx = double_props.getKeyword("PERMX").getData();
        BOOST_CHECK_EQUAL(permx[0], units.to_si(Ewoms::UnitSystem::measure::permeability, 100));
        BOOST_CHECK_EQUAL(permx[9], units.to_si(Ewoms::UnitSystem::measure::permeability, 1000));
        BOOST_CHECK_EQUAL(permx[18], units.to_si(Ewoms::UnitSystem::measure::permeability, 10000));
    }
}

inline void TestPostProcessorMul(std::vector< double >& values,
        const Ewoms::TableManager*,
        const Ewoms::EclipseGrid*,
        Ewoms::GridProperties<int>*,
        Ewoms::GridProperties<double>*)
{
    for( size_t g = 0; g < values.size(); g++ )
        values[g] *= 2.0;
}

BOOST_AUTO_TEST_CASE(multiply) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo("P" , 10 , "1");
    Ewoms::GridProperty<int> p1( 5 , 5 , 4 , keywordInfo);
    Ewoms::GridProperty<int> p2( 5 , 5 , 5 , keywordInfo);
    Ewoms::GridProperty<int> p3( 5 , 5 , 4 , keywordInfo);

    BOOST_CHECK_THROW( p1.multiplyWith(p2) , std::invalid_argument );
    p1.multiplyWith(p3);

    const auto& data = p1.getData();
    for (size_t g = 0; g < p1.getCartesianSize(); g++)
        BOOST_CHECK( 100 == data[g]);

}

BOOST_AUTO_TEST_CASE(mask_test) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo1("P" , 10 , "1");
    SupportedKeywordInfo keywordInfo2("P" , 20 , "1");
    Ewoms::GridProperty<int> p1( 5 , 5 , 4 , keywordInfo1);
    Ewoms::GridProperty<int> p2( 5 , 5 , 4 , keywordInfo2);

    std::vector<bool> mask;

    p1.initMask(10 , mask);
    p2.maskedSet( 10 , mask);
    const auto& d1 = p1.getData();
    const auto& d2 = p2.getData();
    BOOST_CHECK(d1 == d2);
}

BOOST_AUTO_TEST_CASE(CheckLimits) {
    typedef Ewoms::GridProperty<int>::SupportedKeywordInfo SupportedKeywordInfo;
    SupportedKeywordInfo keywordInfo1("P" , 1 , "1");
    Ewoms::GridProperty<int> p1( 5 , 5 , 4 , keywordInfo1);

    p1.checkLimits(0,2);
    BOOST_CHECK_THROW( p1.checkLimits(-2,0) , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(PropertiesEmpty) {
    typedef Ewoms::GridProperties<int>::SupportedKeywordInfo SupportedKeywordInfo;
    std::vector<SupportedKeywordInfo> supportedKeywords = {
        SupportedKeywordInfo("SATNUM" , 0, "1"),
        SupportedKeywordInfo("FIPNUM" , 2, "1")
    };

    const Ewoms::EclipseGrid grid(10, 7, 9);
    Ewoms::GridProperties<int> gridProperties(grid, std::move(supportedKeywords));

    BOOST_CHECK( gridProperties.supportsKeyword("SATNUM") );
    BOOST_CHECK( gridProperties.supportsKeyword("FIPNUM") );
    BOOST_CHECK( !gridProperties.supportsKeyword("FLUXNUM") );
    BOOST_CHECK( !gridProperties.hasKeyword("SATNUM"));
    BOOST_CHECK( !gridProperties.hasKeyword("FLUXNUM"));

    BOOST_CHECK_THROW( gridProperties.getDeckKeyword("SATNUM") , std::invalid_argument);
    BOOST_CHECK_THROW( gridProperties.getDeckKeyword("NONONO") , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(addKeyword) {
    typedef Ewoms::GridProperties<int>::SupportedKeywordInfo SupportedKeywordInfo;
    std::vector<SupportedKeywordInfo> supportedKeywords = {
        SupportedKeywordInfo("SATNUM" , 0, "1")
    };
    Ewoms::EclipseGrid grid(10,7,9);
    Ewoms::GridProperties<int> gridProperties(grid, std::move( supportedKeywords ));

    BOOST_CHECK_THROW( gridProperties.addKeyword("NOT-SUPPORTED"), std::invalid_argument);

    BOOST_CHECK(  gridProperties.addKeyword("SATNUM"));
    BOOST_CHECK( !gridProperties.addKeyword("SATNUM"));
    BOOST_CHECK(  gridProperties.hasKeyword("SATNUM"));
}

BOOST_AUTO_TEST_CASE(hasKeyword_assertKeyword) {
    typedef Ewoms::GridProperties<int>::SupportedKeywordInfo SupportedKeywordInfo;
    std::vector<SupportedKeywordInfo> supportedKeywords = {
        SupportedKeywordInfo("SATNUM" , 0, "1", true),
        SupportedKeywordInfo("FIPNUM" , 0, "1", true)
    };
    const Ewoms::EclipseGrid grid(10, 7, 9);
    const Ewoms::GridProperties<int> gridProperties( grid, std::move( supportedKeywords ) );

    // calling getKeyword() should not change the semantics of hasKeyword()!
    BOOST_CHECK(!gridProperties.hasKeyword("SATNUM"));
    BOOST_CHECK(!gridProperties.hasKeyword("FIPNUM"));

    gridProperties.assertKeyword("FIPNUM");
    gridProperties.getKeyword("SATNUM");
    BOOST_CHECK(gridProperties.hasKeyword("SATNUM"));
    BOOST_CHECK(gridProperties.hasKeyword("FIPNUM"));

    BOOST_CHECK_THROW( gridProperties.getKeyword( "NOT-SUPPORTED" ), std::invalid_argument );
}

// =====================================================================

namespace {

    struct Setup
    {
        Setup(const ::Ewoms::Deck& deck)
            : tabMgr{ deck }
            , eGrid { deck }
            , props { deck, tabMgr, eGrid }
        {}

        Setup(const std::string& input)
            : Setup(::Ewoms::Parser{}.parseString(input))
        {}

        ::Ewoms::TableManager        tabMgr;
        ::Ewoms::EclipseGrid         eGrid;
        ::Ewoms::Eclipse3DProperties props;
    };

} // Anonymous

BOOST_AUTO_TEST_CASE(EndScale_Horizontal)
{
    const auto input = std::string { R"(
RUNSPEC

TITLE
Defaulted SOWCR

DIMENS
 5 5 1 /

OIL
WATER
METRIC

ENDSCALE
/

TABDIMS
/

GRID

DXV
  5*100
/

DYV
 5*100
/

DZV
  10
/

TOPS
  25*2000 /

PROPS

SWOF
  0.0 0.0 1.0 0.0
  1.0 1.0 0.0 0.0
/

SOWCR
  1*  1*    1*    1*   1*
  1*  0.2   0.3   0.4  1*
  1*  0.3   1*    0.5  1*
  1*  0.4   0.5   0.6  1*
  1*  1*    1*    1*   1* /

SWL
  0.1   0.1   0.1   0.1   0.1
  0.1   0.2   0.3   0.4   0.1
  0.1   0.3   0.1   0.5   0.1
  0.1   0.4   0.5   0.6   0.1
  0.1   0.1   0.1   0.1   0.1 /

BOX
  1 5 2 2 1 1 /

SWU
  5*0.23 /

EQUALS
  SWU  0.8  2 2 3 4 1 1 / Two elements
  SWU  0.7  4 4 3 3 1 1 / Single element
/

-- Adds value to a defaulted value, should still be treated as defaulted
ADD
  SWU 0.05 3 3 5 5 1 1 /
/

-- Assigns new value (no longer defaulted)
MINVALUE
  SWU 0.3 5 5 5 5 1 1 /
/

END)" };

    const auto cse = Setup{ input };

    {
        BOOST_CHECK(cse.props.hasDeckDoubleGridProperty("SOWCR"));

        const auto& sowcr = cse.props.getDoubleGridProperty("SOWCR");
        const auto& dflt  = sowcr.wasDefaulted();

        const auto T = true;
        const auto F = false;
        const auto expect_dflt = std::vector<bool> {
            T,   T,   T,   T,   T,
            T,   F,   F,   F,   T,
            T,   F,   T,   F,   T,
            T,   F,   F,   F,   T,
            T,   T,   T,   T,   T,
        };

        BOOST_CHECK_EQUAL_COLLECTIONS(dflt       .begin(), dflt       .end(),
                                      expect_dflt.begin(), expect_dflt.end());
    }

    {
        BOOST_CHECK(cse.props.hasDeckDoubleGridProperty("SWL"));

        const auto& swl  = cse.props.getDoubleGridProperty("SWL");
        const auto& dflt = swl.wasDefaulted();

        const auto expect_dflt =
            std::vector<bool>(cse.eGrid.getNumActive(), false);

        BOOST_CHECK_EQUAL_COLLECTIONS(dflt       .begin(), dflt       .end(),
                                      expect_dflt.begin(), expect_dflt.end());
    }

    {
        BOOST_CHECK(cse.props.hasDeckDoubleGridProperty("SWU"));

        const auto& swu  = cse.props.getDoubleGridProperty("SWU");
        const auto& dflt = swu.wasDefaulted();

        const auto T = true;
        const auto F = false;
        const auto expect_dflt = std::vector<bool> {
            T,   T,   T,   T,   T,
            F,   F,   F,   F,   F,
            T,   F,   T,   F,   T,
            T,   F,   T,   T,   T,
            T,   T,   T,   T,   F,
        };

        BOOST_CHECK_EQUAL_COLLECTIONS(dflt       .begin(), dflt       .end(),
                                      expect_dflt.begin(), expect_dflt.end());
    }
}

