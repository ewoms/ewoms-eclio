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

#define BOOST_TEST_MODULE SaltTableTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/parserkeywords/p.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/units/units.hh>

// generic table classes
#include <ewoms/eclio/parser/eclipsestate/tables/simpletable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/pvtwsalttable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rwgsalttable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/saltvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/saltpvdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/permfacttable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

// keyword specific table classes

#include <boost/date_time/posix_time/posix_time.hpp>

#include <stdexcept>
#include <iostream>

using namespace Ewoms;

inline std::string prefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

BOOST_AUTO_TEST_CASE( Brine ) {
    const char *deckData =
        "TABDIMS\n"
        "1 1/\n"
        "\n"
        "EQLDIMS\n"
        "1 /\n"
        "\n"
        "PVTWSALT\n"
        " 1000 0.0/\n"
        " 0 1 2 3 4 \n"
        " 10 11 12 13 14/\n"
        "\n"
        "RWGSALT\n"
        " 300 0.0 0.00013 \n"
        " 600 0.5 0.000132 \n"
        "/ \n"
        "PERMFACT\n"
        "0 0 \n"
        "0.5 0.5 \n"
        "1 1 \n"
        "1.5 1.5\n"
        "/ \n"
        "BDENSITY\n"
        " 1000 1050 /\n"
        "\n"
        "SALTVD\n"
        "500 0\n"
        "550 50/\n"
        "\n"
        "SALTPVD\n"
        "500 0\n"
        "550 0.5/\n";

    Ewoms::Parser parser;
    auto deck = parser.parseString(deckData);

    Ewoms::TableManager tables(deck);
    const auto& PvtwsaltTables = tables.getPvtwSaltTables( );
    BOOST_CHECK_EQUAL(1U, PvtwsaltTables.size() );
    BOOST_CHECK_EQUAL(2U, PvtwsaltTables[0].size());

    const auto& PvtwsaltTable1 = PvtwsaltTables[0];
    BOOST_CHECK_EQUAL (PvtwsaltTable1.getSaltConcentrationColumn().size(), 2U);
    BOOST_CHECK_CLOSE (PvtwsaltTable1.getSaltConcentrationColumn()[1], 10, 1e-5);

    BOOST_CHECK_EQUAL (PvtwsaltTable1.getFormationVolumeFactorColumn().size(), 2U);
    BOOST_CHECK_CLOSE (PvtwsaltTable1.getFormationVolumeFactorColumn()[0], 1, 1e-5);

    BOOST_CHECK_EQUAL (PvtwsaltTable1.getCompressibilityColumn().size(), 2U);
    BOOST_CHECK_CLOSE (PvtwsaltTable1.getCompressibilityColumn()[1], 12/1e5, 1e-5);

    BOOST_CHECK_EQUAL (PvtwsaltTable1.getViscosityColumn().size(), 2U);
    BOOST_CHECK_CLOSE (PvtwsaltTable1.getViscosityColumn()[1], 13*0.001, 1e-5);

    BOOST_CHECK_CLOSE (PvtwsaltTable1.getReferencePressureValue(), 1000*1e5, 1e-5);

    const auto& RwgsaltTables = tables.getRwgSaltTables( );
    BOOST_CHECK_EQUAL(1U, RwgsaltTables.size() );
    BOOST_CHECK_EQUAL(2U, RwgsaltTables[0].size());

    const auto& RwgsaltTable1 = RwgsaltTables[0];

    BOOST_CHECK_EQUAL (RwgsaltTable1.getPressureColumn().size(), 2U);
    BOOST_CHECK_CLOSE (RwgsaltTable1.getPressureColumn()[1], Metric::Pressure * 600, 1e-5);

    BOOST_CHECK_EQUAL (RwgsaltTable1.getSaltConcentrationColumn().size(), 2U);
    BOOST_CHECK_CLOSE (RwgsaltTable1.getSaltConcentrationColumn()[1], 0.5, 1e-5);

    BOOST_CHECK_EQUAL (RwgsaltTable1.getVaporizedWaterGasRatioColumn().size(), 2U);
    BOOST_CHECK_CLOSE (RwgsaltTable1.getVaporizedWaterGasRatioColumn()[0], 0.00013, 1e-5);

    const auto& BdensityTables = tables.getBrineDensityTables( );
    const auto& BdensityTable1 = BdensityTables[0];

    BOOST_CHECK_EQUAL(1U, BdensityTables.size() );
    BOOST_CHECK_EQUAL (BdensityTable1.getBrineDensityColumn().size(), 2U);
    BOOST_CHECK_CLOSE (BdensityTable1.getBrineDensityColumn()[1], 1050, 1e-5);

    const Ewoms::TableContainer& saltvdTables = tables.getSaltvdTables();
    const auto& saltvdTable = saltvdTables.getTable<Ewoms::SaltvdTable>(0);

    BOOST_CHECK_EQUAL(saltvdTable.getDepthColumn().size(), 2U);
    BOOST_CHECK_CLOSE (saltvdTable.getSaltColumn() [1],50, 1e-5);

    const Ewoms::TableContainer& saltpvdTables = tables.getSaltpvdTables();
    const auto& saltpvdTable = saltpvdTables.getTable<Ewoms::SaltpvdTable>(0);

    BOOST_CHECK_EQUAL(saltpvdTable.getDepthColumn().size(), 2U);
    BOOST_CHECK_CLOSE(saltpvdTable.getSaltpColumn() [1],0.5, 1e-5);

    const Ewoms::TableContainer& permfactTables = tables.getPermfactTables();
    const auto& permfactTable = permfactTables.getTable<Ewoms::PermfactTable>(0);

    BOOST_CHECK_EQUAL(permfactTable.getPorosityChangeColumn().size(), 4U);
    BOOST_CHECK_CLOSE(permfactTable.getPermeabilityMultiplierColumn() [3],1.5, 1e-5);

}

