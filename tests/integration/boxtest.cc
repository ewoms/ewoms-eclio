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

#define BOOST_TEST_MODULE BoxTest

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>

#include <boost/filesystem/path.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>

using namespace Ewoms;

inline std::string prefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

inline Deck makeDeck(const std::string& fileName) {
    Parser parser;
    boost::filesystem::path boxFile(fileName);
    return parser.parseFile(boxFile.string());
}

inline EclipseState makeState(const std::string& fileName) {
    return EclipseState( makeDeck(fileName) );
}

BOOST_AUTO_TEST_CASE( PERMX ) {
    EclipseState state = makeState( prefix() + "BOX/BOXTEST1" );
    const auto& permx = state.fieldProps().get_global<double>( "PERMX" );
    const auto& permy = state.fieldProps().get_global<double>( "PERMY" );
    const auto& permz = state.fieldProps().get_global<double>( "PERMZ" );
    size_t i, j, k;
    const EclipseGrid& grid = state.getInputGrid();

    for (k = 0; k < grid.getNZ(); k++) {
        for (j = 0; j < grid.getNY(); j++) {
            for (i = 0; i < grid.getNX(); i++) {
                std::size_t g = grid.getGlobalIndex(i,j,k);

                BOOST_CHECK_CLOSE( permx[g] * 0.25, permz[g], 0.001 );
                BOOST_CHECK_EQUAL( permx[g] * 2   , permy[g] );

            }
        }
    }
}

BOOST_AUTO_TEST_CASE( PARSE_BOX_OK ) {
    EclipseState state = makeState( prefix() + "BOX/BOXTEST1" );
    const auto& satnum = state.fieldProps().get_global<int>( "SATNUM" );
    {
        size_t i, j, k;
        const EclipseGrid& grid = state.getInputGrid();
        for (k = 0; k < grid.getNZ(); k++) {
            for (j = 0; j < grid.getNY(); j++) {
                for (i = 0; i < grid.getNX(); i++) {

                    size_t g = i + j * grid.getNX() + k * grid.getNX() * grid.getNY();
                    if (i <= 1 && j <= 1 && k <= 1)
                        BOOST_CHECK_EQUAL( satnum[ g ], 10 );
                    else
                        BOOST_CHECK_EQUAL( satnum[g], 2 );

                }
            }
        }
    }
}

BOOST_AUTO_TEST_CASE( PARSE_MULTIPLY_COPY ) {
    EclipseState state = makeState( prefix() + "BOX/BOXTEST1" );
    const auto& satnum = state.fieldProps().get_global<int>( "SATNUM" );
    const auto& fipnum = state.fieldProps().get_global<int>( "FIPNUM" );
    size_t i, j, k;
    const EclipseGrid& grid = state.getInputGrid();

    for (k = 0; k < grid.getNZ(); k++) {
        for (j = 0; j < grid.getNY(); j++) {
            for (i = 0; i < grid.getNX(); i++) {

                size_t g = grid.getGlobalIndex(i,j,k);
                if (i <= 1 && j <= 1 && k <= 1)
                    BOOST_CHECK_EQUAL( 4 * satnum[g], fipnum[g] );
                else
                    BOOST_CHECK_EQUAL( 2 * satnum[g], fipnum[g] );

            }
        }
    }
}

BOOST_AUTO_TEST_CASE( EQUALS ) {
    EclipseState state = makeState( prefix() + "BOX/BOXTEST1" );
    const auto& pvtnum = state.fieldProps().get_global<int>( "PVTNUM" );
    const auto& eqlnum = state.fieldProps().get_global<int>( "EQLNUM" );
    const auto& poro   = state.fieldProps().get_global<double>( "PORO" );
    size_t i, j, k;
    const EclipseGrid& grid = state.getInputGrid();

    for (k = 0; k < grid.getNZ(); k++) {
        for (j = 0; j < grid.getNY(); j++) {
            for (i = 0; i < grid.getNX(); i++) {
                size_t g = grid.getGlobalIndex(i,j,k);

                BOOST_CHECK_EQUAL( pvtnum[g], k );
                BOOST_CHECK_EQUAL( eqlnum[g], 77 + 2 * k );
                BOOST_CHECK_EQUAL( poro[g], 0.25 );
            }
        }
    }
}

BOOST_AUTO_TEST_CASE( OPERATE ) {
    EclipseState state = makeState( prefix() + "BOX/BOXTEST1" );
    const EclipseGrid& grid = state.getInputGrid();
    const auto& ntg = state.fieldProps().get_global<double>("NTG");
    BOOST_CHECK_EQUAL( ntg[grid.getGlobalIndex(0,0,0)], 8.50 );  // MULTA
    BOOST_CHECK_EQUAL( ntg[grid.getGlobalIndex(0,5,0)], 5.00 );  // POLY
    BOOST_CHECK_EQUAL( ntg[grid.getGlobalIndex(0,0,1)], 4.0 );   // COPY
    BOOST_CHECK_EQUAL( ntg[grid.getGlobalIndex(0,5,1)], 4.0 );   // MINLIM
    BOOST_CHECK_EQUAL( ntg[grid.getGlobalIndex(0,0,2)], 2.0 );   // MAXLIM
    BOOST_CHECK_EQUAL( ntg[grid.getGlobalIndex(0,0,3)], 0.5 );   // MAXVALUE
    BOOST_CHECK_EQUAL( ntg[grid.getGlobalIndex(0,0,4)], 1.5 );   // MINVALUE
}

BOOST_AUTO_TEST_CASE( CONSTRUCTOR_AND_UPDATE ) {
    auto deck = makeDeck( prefix() + "BOX/BOXTEST1" );
    EclipseGrid grid(deck);
    const auto& box_keyword = deck.getKeyword("BOX", 0);
    const auto& operate_keyword = deck.getKeyword("OPERATE");
    Box box(grid);
    box.update(box_keyword.getRecord(0));
    BOOST_CHECK_EQUAL(box.size(), 8);

    box.update( operate_keyword.getRecord(0) );
    BOOST_CHECK_EQUAL(box.size(), 50);

    box.reset();
    BOOST_CHECK_EQUAL(box.size(), 1000);
}
