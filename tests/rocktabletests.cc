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

#define BOOST_TEST_MODULE RockTableTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/parserkeywords/p.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

// generic table classes
#include <ewoms/eclio/parser/eclipsestate/tables/simpletable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

// keyword specific table classes
//#include <ewoms/eclio/parser/eclipsestate/tables/pvtotable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/swoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/rockwnodtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/overburdtable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/sgoftable.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/plyadstable.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/vfpprodtable.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/vfpinjtable.hh>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <stdexcept>
#include <iostream>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE( Rock2d ) {
    const char *input =
            "TABDIMS\n"
            "1 1 /\n"
            "\n"
            "FIELD\n"
            "\n"
            "ROCKCOMP\n"
            " REVERS 2 /\n"
            "\n"
            "OVERBURD\n"
            "1 1.0\n"
            "10 2.0\n"
            " / \n"
            "2 1.1\n"
            "20 2.1\n"
            " / \n"
            "ROCK2D\n"
            " 0.0     0.01\n"
            "         0.02\n"
            "         0.03 / \n"
            "10.0     0.11 \n"
            "         0.12 \n"
            "         0.13/ \n"
            "20.0     0.21\n"
            "         0.22\n"
            "         0.23/\n"
            " / \n"
            "0.7      0.04 \n"
            "         0.05 /\n"
            "10.0     0.14\n"
            "         0.15/\n"
            "20.0     0.24\n"
            "         0.25/\n"
            " / \n"
            "ROCK2DTR\n"
            " 0.0     0.01\n"
            "         0.02\n"
            "         0.03 / \n"
            "10.0     0.11 \n"
            "         0.12 \n"
            "         0.13/ \n"
            "20.0     0.21\n"
            "         0.22\n"
            "         0.23/\n"
            " / \n"
            "0.7      0.04 \n"
            "         0.05 /\n"
            "10.0     0.14\n"
            "         0.15/\n"
            "20.0     0.24\n"
            "         0.25/\n"
            " / \n"
            "ROCKWNOD\n"
            "0.0\n"
            "0.5\n"
            "1.0 /\n"
            "0.9\n"
            "1.0 /\n"
            "\n";

    auto deck = Parser().parseString( input );
    TableManager tables( deck );

    const auto& rock2d = tables.getRock2dTables();
    const auto& rock2dtr = tables.getRock2dtrTables();
    const auto& rockwnod = tables.getRockwnodTables();
    const auto& overburd = tables.getOverburdTables();

    const auto& rec1 = rock2d[0];
    const auto& rec2 = rock2d.at(1);
    const auto& rec1tr = rock2dtr[0];

    const RockwnodTable& rockwnodTable1 = rockwnod.getTable<RockwnodTable>(0);
    const RockwnodTable& rockwnodTable2 = rockwnod.getTable<RockwnodTable>(1);

    const OverburdTable& overburdTable = overburd.getTable<OverburdTable>(0);
    BOOST_CHECK_THROW( rock2d.at(2), std::out_of_range );
    BOOST_REQUIRE_EQUAL(3U, rec1.size());
    BOOST_REQUIRE_EQUAL(3U, rec2.size());
    BOOST_REQUIRE_EQUAL(0.0, rec1.getPressureValue(0));
    BOOST_REQUIRE_EQUAL(0.13, rec1.getPvmultValue(1,2));
    BOOST_REQUIRE_EQUAL(rec1.sizeMultValues(), rockwnodTable1.getSaturationColumn().size());
    BOOST_REQUIRE_EQUAL(rec2.sizeMultValues(), rockwnodTable2.getSaturationColumn().size());
    BOOST_REQUIRE_EQUAL(0.0, rockwnodTable1.getSaturationColumn()[0]);
    // convert from FIELD units to SI units
    BOOST_CHECK_CLOSE(1.0 / 3.28084, overburdTable.getDepthColumn()[0], 1e-4);
    BOOST_CHECK_CLOSE(1.0 * 6894.76, overburdTable.getOverburdenPressureColumn()[0], 1e-4);
    BOOST_REQUIRE_EQUAL(0.0, rec1tr.getPressureValue(0));
    BOOST_REQUIRE_EQUAL(0.13, rec1tr.getTransMultValue(1,2));
}
