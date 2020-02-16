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

#define BOOST_TEST_MODULE ParserIntegrationTests
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyvisctable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plymaxtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyrocktable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyadstable.hh>

using namespace Ewoms;

inline std::string pathprefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

BOOST_AUTO_TEST_CASE( parse_polymer_tables ) {
    Parser parser;
    auto deck = parser.parseFile(pathprefix() + "POLYMER/POLY.inc");
    Ewoms::TableManager tables( deck );
    const TableContainer& plymax = tables.getPlymaxTables();
    const TableContainer& plyrock = tables.getPlyrockTables();
    const TableContainer& plyads = tables.getPlyadsTables();
    const TableContainer& plyvis = tables.getPlyviscTables();

    BOOST_CHECK_EQUAL( plymax.size() , 1U );
    BOOST_CHECK_EQUAL( plyrock.size() , 1U );
    BOOST_CHECK_EQUAL( plyvis.size() , 1U );
    BOOST_CHECK_EQUAL( plyads.size() , 1U );

    {
        const Ewoms::PlymaxTable& table0 = plymax.getTable<Ewoms::PlymaxTable>(0);
        BOOST_CHECK_EQUAL( table0.numColumns() , 2U );
        BOOST_CHECK_EQUAL( table0.getPolymerConcentrationColumn()[0] , 3.0 );
        BOOST_CHECK_EQUAL( table0.getMaxPolymerConcentrationColumn()[0] , 0.0 );
    }

    {
        const Ewoms::PlyviscTable& table0 = plyvis.getTable<Ewoms::PlyviscTable>(0);
        BOOST_CHECK_EQUAL( table0.numColumns() , 2U );
        BOOST_CHECK_EQUAL( table0.getPolymerConcentrationColumn()[5] , 3.0 );
        BOOST_CHECK_EQUAL( table0.getViscosityMultiplierColumn()[5] , 48.0 );
    }

    {
        const Ewoms::PlyrockTable& table0 = plyrock.getTable<Ewoms::PlyrockTable>(0);
        BOOST_CHECK_EQUAL( table0.numColumns() , 5U );
        BOOST_CHECK_EQUAL( table0.getDeadPoreVolumeColumn()[0] , 0.05 );
        BOOST_CHECK_EQUAL( table0.getMaxAdsorbtionColumn()[0] , 0.000025 );
    }

    {
        const Ewoms::PlyadsTable& table0 = plyads.getTable<Ewoms::PlyadsTable>(0);
        BOOST_CHECK_EQUAL( table0.numColumns() , 2U );
        BOOST_CHECK_EQUAL( table0.getPolymerConcentrationColumn()[8] , 3.0 );
        BOOST_CHECK_EQUAL( table0.getAdsorbedPolymerColumn()[8] , 0.000025 );
    }
}
