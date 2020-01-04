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

#define BOOST_TEST_MODULE PORVTESTS
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/parser.hh>

#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

static Ewoms::Deck createCARTDeck() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckWithPORO() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "PORO\n"
        "100*0.10 100*0.20 100*0.30 100*0.40 100*0.50 100*0.60 100*0.70 100*0.80 100*0.90 100*1.0 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckWithPORVPORO() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "BOX \n"
        "1 10 1 10 1 1 /\n"
        "PORV\n"
        "100*77 /\n"
        "ENDBOX \n"
        "PORO\n"
        "100*0.10 100*0.20 100*0.30 100*0.40 100*0.50 100*0.60 100*0.70 100*0.80 100*0.90 100*1.0 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckWithMULTPV() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "BOX \n"
        "1 10 1 10 1 1 /\n"
        "PORV\n"
        "100*77 /\n"
        "ENDBOX \n"
        "PORO\n"
        "100*0.10 100*0.20 100*0.30 100*0.40 100*0.50 100*0.60 100*0.70 100*0.80 100*0.90 100*1.0 /\n"
        "EDIT\n"
        "BOX \n"
        "1 5 1 5 1 1 /\n"
        "MULTPV\n"
        "25*10 / \n"
        "ENDBOX \n"
        "BOX \n"
        "1 10 1 10 10 10 /\n"
        "MULTPV\n"
        "100*10 / \n"
        "ENDBOX \n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckWithBOXPORV() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "PORV\n"
        "1000*123.456 /\n"
        "BOX \n"
        "2 3 2 3 2 3 /\n"
        "PORV\n"
        "8*789.012 /\n"
        "ENDBOX\n"
        "MULTPV\n"
        "1000*10 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckWithNTG() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "PORO\n"
        "1000*0.20 /\n"
        "BOX \n"
        "1 1 1 1 1 1 /\n"
        "PORV \n"
        "1*10 /\n"
        "ENDBOX\n"
        "MULTPV\n"
        "1000*10 /\n"
        "NTG\n"
        "1000*2 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

static Ewoms::Deck createDeckWithMULTREGP() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "PORV\n"
        "1000*77 /\n"
        "MULTNUM\n"
        "100*1 700*2 200*3 / \n"
        "FLUXNUM\n"
        "200*1 700*2 100*3  / \n"
        "MULTREGP\n"
        "1 10.0 / \n"
        "1 2.0 F/ \n"
        "0 123.0 F/ \n" // ignored
        "2 20.0 M / \n"
        "/\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(PORV_cartesianDeck) {
    /* Check that an exception is raised if we try to create a PORV field without PORO. */
    Ewoms::Deck deck = createCARTDeck();
    Ewoms::TableManager tm( deck );
    Ewoms::EclipseGrid grid( deck );
    Ewoms::Eclipse3DProperties props( deck, tm, grid );
    const auto& poro = props.getDoubleGridProperty("PORO");
    BOOST_CHECK(poro.containsNaN());
    BOOST_CHECK_THROW(props.getDoubleGridProperty("PORV"), std::logic_error);
}

BOOST_AUTO_TEST_CASE(PORV_initFromPoro) {
    /* Check that the PORV field is correctly calculated from PORO. */
    Ewoms::Deck deck = createDeckWithPORO();
    Ewoms::TableManager tm( deck );
    Ewoms::EclipseGrid grid( deck );
    Ewoms::Eclipse3DProperties props( deck, tm, grid );
    const auto& poro = props.getDoubleGridProperty("PORO");
    BOOST_CHECK( !poro.containsNaN() );

    const auto& porv_data = props.getDoubleGridProperty("PORV").getData();
    double cell_volume = 0.25 * 0.25 * 0.25;

    BOOST_CHECK_CLOSE( cell_volume * 0.10 , porv_data[grid.getGlobalIndex(0,0,0)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 0.10 , porv_data[grid.getGlobalIndex(9,9,0)] , 0.001);

    BOOST_CHECK_CLOSE( cell_volume * 0.50 , porv_data[grid.getGlobalIndex(0,0,4)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 0.50 , porv_data[grid.getGlobalIndex(9,9,4)] , 0.001);

    BOOST_CHECK_CLOSE( cell_volume * 1.00 , porv_data[grid.getGlobalIndex(0,0,9)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 1.00 , porv_data[grid.getGlobalIndex(9,9,9)] , 0.001);
}

BOOST_AUTO_TEST_CASE(PORV_initFromPoroWithCellVolume) {
    /* Check that explicit PORV and CellVOlume * PORO can be combined. */
    Ewoms::Deck deck = createDeckWithPORVPORO();
    Ewoms::TableManager tm( deck );
    Ewoms::EclipseGrid grid( deck );
    Ewoms::Eclipse3DProperties props( deck, tm, grid );
    const auto& porv_data = props.getDoubleGridProperty("PORV").getData();
    double cell_volume = 0.25 * 0.25 * 0.25;

    BOOST_CHECK_CLOSE( 77.0 , porv_data[grid.getGlobalIndex(0,0,0)] , 0.001);
    BOOST_CHECK_CLOSE( 77.0 , porv_data[grid.getGlobalIndex(9,9,0)] , 0.001);

    BOOST_CHECK_CLOSE( cell_volume * 0.50 , porv_data[grid.getGlobalIndex(0,0,4)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 0.50 , porv_data[grid.getGlobalIndex(9,9,4)] , 0.001);

    BOOST_CHECK_CLOSE( cell_volume * 1.00 , porv_data[grid.getGlobalIndex(0,0,9)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 1.00 , porv_data[grid.getGlobalIndex(9,9,9)] , 0.001);
}

BOOST_AUTO_TEST_CASE(PORV_multpv) {
    /* Check that MULTPV is correctly accounted for. */
    Ewoms::Deck deck = createDeckWithMULTPV();
    Ewoms::TableManager tm( deck );
    Ewoms::EclipseGrid grid( deck );
    Ewoms::Eclipse3DProperties props( deck, tm, grid );
    const auto& porv_data = props.getDoubleGridProperty("PORV").getData();
    double cell_volume = 0.25 * 0.25 * 0.25;

    BOOST_CHECK_CLOSE( 770.0 , porv_data[grid.getGlobalIndex(0,0,0)] , 0.001);
    BOOST_CHECK_CLOSE( 770.0 , porv_data[grid.getGlobalIndex(4,4,0)] , 0.001);
    BOOST_CHECK_CLOSE( 77.0 , porv_data[grid.getGlobalIndex(9,9,0)] , 0.001);

    BOOST_CHECK_CLOSE( cell_volume * 0.50 , porv_data[grid.getGlobalIndex(0,0,4)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 0.50 , porv_data[grid.getGlobalIndex(9,9,4)] , 0.001);

    BOOST_CHECK_CLOSE( cell_volume * 0.90 , porv_data[grid.getGlobalIndex(0,0,8)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 0.90 , porv_data[grid.getGlobalIndex(9,9,8)] , 0.001);

    BOOST_CHECK_CLOSE( cell_volume * 10.00 , porv_data[grid.getGlobalIndex(0,0,9)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * 10.00 , porv_data[grid.getGlobalIndex(9,9,9)] , 0.001);
}

BOOST_AUTO_TEST_CASE(PORV_mutipleBoxAndMultpv) {
    /* Check that MULTIPLE Boxed PORV and MULTPV statements work */
    Ewoms::Deck deck = createDeckWithBOXPORV();
    Ewoms::TableManager tm( deck );
    Ewoms::EclipseGrid grid( deck );
    Ewoms::Eclipse3DProperties props( deck, tm, grid );
    const auto& porv_data = props.getDoubleGridProperty("PORV").getData();

    BOOST_CHECK_CLOSE( 1234.56 , porv_data[grid.getGlobalIndex(0,0,0)] , 0.001);
    BOOST_CHECK_CLOSE( 1234.56 , porv_data[grid.getGlobalIndex(9,9,9)] , 0.001);

    BOOST_CHECK_CLOSE( 7890.12 , porv_data[grid.getGlobalIndex(1,1,1)] , 0.001);
    BOOST_CHECK_CLOSE( 7890.12 , porv_data[grid.getGlobalIndex(2,2,2)] , 0.001);

}

BOOST_AUTO_TEST_CASE(PORV_multpvAndNtg) {
    /* Check that MULTIPLE Boxed PORV and MULTPV statements work and NTG */
    Ewoms::Deck deck = createDeckWithNTG();
    Ewoms::TableManager tm( deck );
    Ewoms::EclipseGrid grid( deck );
    Ewoms::Eclipse3DProperties props( deck, tm, grid );
    const auto& porv_data = props.getDoubleGridProperty("PORV").getData();
    double cell_volume = 0.25 * 0.25 * 0.25;
    double poro = 0.20;
    double multpv = 10;
    double NTG = 2;
    double PORV = 10;

    BOOST_CHECK_CLOSE( PORV * multpv                 , porv_data[grid.getGlobalIndex(0,0,0)] , 0.001);
    BOOST_CHECK_CLOSE( cell_volume * poro*multpv*NTG , porv_data[grid.getGlobalIndex(9,9,9)] , 0.001);
}

BOOST_AUTO_TEST_CASE(PORV_multregp) {
    Ewoms::Deck deck = createDeckWithMULTREGP();
    Ewoms::TableManager tm( deck );
    Ewoms::EclipseGrid grid( deck );
    Ewoms::Eclipse3DProperties props( deck, tm, grid );
    const auto& porv = props.getDoubleGridProperty("PORV");
    const auto& porvData = porv.getData();
    double basePorv = 77.0;

    // the cumlative effect of the base pore volume is multiplied 2 for the cells in
    // region 1 of FLUXNUM and by 20 for region 2 of MULTNUM. This means that the first
    // 100 cels are multiplied by 2, then follow 100 cells multiplied by 2*20, then
    // 600 cells multiplied by 20 while the last 200 cells are not modified at all.
    for (int i=0; i < 100; ++i)
        BOOST_CHECK_CLOSE(porvData[i], basePorv*2, 1e-8);

    for (int i=100; i < 200; ++i)
        BOOST_CHECK_CLOSE(porvData[i], basePorv*2*20, 1e-8);

    for (int i=200; i < 800; ++i)
        BOOST_CHECK_CLOSE(porvData[i], basePorv*20, 1e-8);

    for (int i=800; i < 1000; ++i)
        BOOST_CHECK_CLOSE(porvData[i], basePorv, 1e-8);
}

static Ewoms::Deck createDeckNakedGRID() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "PORO\n"
        "100*0.10 100*0.20 100*0.30 100*0.40 100*0.50 100*0.60 100*0.70 100*0.80 100*0.90 100*1.0 /\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(NAKED_GRID_THROWS) {
    /* Check that MULTIPLE Boxed PORV and MULTPV statements work and NTG */
    Ewoms::Deck deck = createDeckNakedGRID();

    Ewoms::TableManager tm( deck );
    BOOST_CHECK_THROW( Ewoms::EclipseGrid grid( deck ), std::invalid_argument );
}

static Ewoms::Deck createDeckWithPOROZero() {
    const char* deckData =
        "RUNSPEC\n"
        "\n"
        "DIMENS\n"
        " 10 10 10 /\n"
        "GRID\n"
        "DX\n"
        "1000*0.25 /\n"
        "DYV\n"
        "10*0.25 /\n"
        "DZ\n"
        "1000*0.25 /\n"
        "TOPS\n"
        "100*0.25 /\n"
        "PORO\n"
        "100*0.10 100*0.20 100*0.30 100*0.40 100*0.50 100*0.60 100*0.70 100*0.80 100*0.90 100*1.0 /\n"
        "EQUALS\n"
        "  ACTNUM 0 1 10 1 10 2 2 /\n"
        "/\n"
        "EQUALS\n"
        "  PORO 0 1 10 1 10 3 3 /\n"
        "/\n"
        "EQUALS\n"
        "  NTG 0 1 10 1 10 4 4 /\n"
        "/\n"
        "EDIT\n"
        "\n";

    Ewoms::Parser parser;
    return parser.parseString(deckData) ;
}

BOOST_AUTO_TEST_CASE(PORO_ZERO_ACTNUM_CORRECT) {
    /* Check that MULTIPLE Boxed PORV and MULTPV statements work and NTG */
    Ewoms::Deck deck = createDeckWithPOROZero();
    Ewoms::EclipseState state( deck);
    const auto& grid = state.getInputGrid( );

    /* Top layer is active */
    BOOST_CHECK( grid.cellActive( 0,0,0 ));

    /* Layer k=1 is inactive due to EQUAL ACTNUM */
    BOOST_CHECK( !grid.cellActive(0,0,1));

    /* Layer k = 2 is inactive due t PORO = 0 */
    BOOST_CHECK( !grid.cellActive(0,0,2));

    /* Layer k = 3 is inactive due t NTG = 0 */
    BOOST_CHECK( !grid.cellActive(0,0,2));

    BOOST_CHECK_EQUAL( grid.getNumActive() , 700U );
}
