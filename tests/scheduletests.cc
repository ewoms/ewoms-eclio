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

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>

#define BOOST_TEST_MODULE ScheduleTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/utility/timeservice.hh>
#include <ewoms/eclio/utility/opminputerror.hh>

#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/rftconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/oilvaporizationproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellconnections.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/gasliftopt.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellmatcher.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/pavg.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/errorguard.hh>
#include <ewoms/eclio/parser/units/dimension.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellproductionproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/wellinjectionproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/guiderateconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/group/guiderate.hh>

using namespace Ewoms;

namespace {
    double liquid_PI_unit()
    {
        return UnitSystem::newMETRIC().to_si(UnitSystem::measure::liquid_productivity_index, 1.0);
    }

    double cp_rm3_per_db()
    {
        return UnitSystem::newMETRIC().to_si(UnitSystem::measure::transmissibility, 1.0);
    }
}

static Schedule make_schedule(const std::string& deck_string) {
    const auto& deck = Parser{}.parseString(deck_string);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    return Schedule(deck, grid , fp, runspec);
}

static std::string createDeck() {
    std::string input =
        "START\n"
        "8 MAR 1998 /\n"
        "\n"
        "SCHEDULE\n"
        "\n";

    return input;
}

static std::string createDeckWithWells() {
    std::string input =
            "START             -- 0 \n"
            "10 MAI 2007 / \n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     \'W_1\'        \'OP\'   30   37  3.33       \'OIL\'  7* /   \n"
            "/ \n"
            "DATES             -- 1\n"
            " 10  \'JUN\'  2007 / \n"
            "/\n"
            "DATES             -- 2,3\n"
            "  10  JLY 2007 / \n"
            "   10  AUG 2007 / \n"
            "/\n"
            "WELSPECS\n"
            "     \'WX2\'        \'OP\'   30   37  3.33       \'OIL\'  7* /   \n"
            "     \'W_3\'        \'OP\'   20   51  3.92       \'OIL\'  7* /  \n"
            "/\n";

    return input;
}

static std::string createDeckWTEST() {
    std::string input =
            "START             -- 0 \n"
            "10 MAI 2007 / \n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     \'DEFAULT\'    \'OP\'   30   37  3.33       \'OIL\'  7*/   \n"
            "     \'ALLOW\'      \'OP\'   30   37  3.33       \'OIL\'  3*  YES / \n"
            "     \'BAN\'        \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "     \'W1\'         \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "     \'W2\'         \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "     \'W3\'         \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "/\n"

            "COMPDAT\n"
            " \'BAN\'  1  1   1   1 \'OPEN\' 1*    1.168   0.311   107.872 1*  1*  \'Z\'  21.925 / \n"
            "/\n"

            "WCONHIST\n"
            "     'BAN'      'OPEN'      'RESV'      0.000      0.000      0.000  5* / \n"
            "/\n"

            "WTEST\n"
            "   'ALLOW'   1   'PE' / \n"
            "/\n"

            "DATES             -- 1\n"
            " 10  JUN 2007 / \n"
            "/\n"

            "WTEST\n"
            "   'ALLOW'  1  '' / \n"
            "   'BAN'    1  'DGC' / \n"
            "/\n"

            "WCONHIST\n"
            "     'BAN'      'OPEN'      'RESV'      1.000      0.000      0.000  5* / \n"
            "/\n"

            "DATES             -- 2\n"
            " 10  JUL 2007 / \n"
            "/\n"

            "WELSPECS\n"
            "     \'I1\'         \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "     \'I2\'         \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "     \'I3\'         \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "/\n"

            "WLIST\n"
            "  \'*ILIST\'  \'NEW\'  I1 /\n"
            "  \'*ILIST\'  \'ADD\'  I2 /\n"
            "/\n"

            "WCONPROD\n"
            "     'BAN'      'OPEN'      'ORAT'      0.000      0.000      0.000  5* / \n"
            "/\n"

            "DATES             -- 3\n"
            " 10  AUG 2007 / \n"
            "/\n"

            "WCONINJH\n"
            "     'BAN'      'WATER'      1*      0 / \n"
            "/\n"

            "DATES             -- 4\n"
            " 10  SEP 2007 / \n"
            "/\n"

            "WELOPEN\n"
            " 'BAN' OPEN / \n"
            "/\n"

            "DATES             -- 4\n"
            " 10  NOV 2007 / \n"
            "/\n"

            "WCONINJH\n"
            "     'BAN'      'WATER'      1*      1.0 / \n"
            "/\n";

    return input;
}

static std::string createDeckForTestingCrossFlow() {
    std::string input =
            "START             -- 0 \n"
            "10 MAI 2007 / \n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     \'DEFAULT\'    \'OP\'   30   37  3.33       \'OIL\'  7*/   \n"
            "     \'ALLOW\'      \'OP\'   30   37  3.33       \'OIL\'  3*  YES / \n"
            "     \'BAN\'        \'OP\'   20   51  3.92       \'OIL\'  3*  NO /  \n"
            "/\n"

            "COMPDAT\n"
            " \'BAN\'  1  1   1   1 \'OPEN\' 1*    1.168   0.311   107.872 1*  1*  \'Z\'  21.925 / \n"
            "/\n"

            "WCONHIST\n"
            "     'BAN'      'OPEN'      'RESV'      0.000      0.000      0.000  5* / \n"
            "/\n"

            "DATES             -- 1\n"
            " 10  JUN 2007 / \n"
            "/\n"

            "WCONHIST\n"
            "     'BAN'      'OPEN'      'RESV'      1.000      0.000      0.000  5* / \n"
            "/\n"

            "DATES             -- 2\n"
            " 10  JUL 2007 / \n"
            "/\n"

            "WCONPROD\n"
            "     'BAN'      'OPEN'      'ORAT'      0.000      0.000      0.000  5* / \n"
            "/\n"

            "DATES             -- 3\n"
            " 10  AUG 2007 / \n"
            "/\n"

            "WCONINJH\n"
            "     'BAN'      'WATER'      1*      0 / \n"
            "/\n"

            "DATES             -- 4\n"
            " 10  SEP 2007 / \n"
            "/\n"

            "WELOPEN\n"
            " 'BAN' OPEN / \n"
            "/\n"

            "DATES             -- 4\n"
            " 10  NOV 2007 / \n"
            "/\n"

            "WCONINJH\n"
            "     'BAN'      'WATER'      1*      1.0 / \n"
            "/\n";

    return input;
}

static std::string createDeckWithWellsOrdered() {
    std::string input =
            "START             -- 0 \n"
            "10 MAI 2007 / \n"
            "WELLDIMS\n"
            "   *  *   3 /\n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     \'CW_1\'        \'CG\'   3   3  3.33       \'OIL\'  7* /   \n"
            "     \'BW_2\'        \'BG\'   3   3  3.33       \'OIL\'  7* /   \n"
            "     \'AW_3\'        \'AG\'   2   5  3.92       \'OIL\'  7* /  \n"
            "/\n";

    return input;
}

static std::string createDeckWithWellsOrderedGRUPTREE() {
    std::string input =
            "START             -- 0 \n"
            "10 MAI 2007 / \n"
            "SCHEDULE\n"
            "GRUPTREE\n"
            "  PG1 PLATFORM /\n"
            "  PG2 PLATFORM /\n"
            "  CG1  PG1 /\n"
            "  CG2  PG2 /\n"
            "/\n"
            "WELSPECS\n"
            "     \'DW_0\'        \'CG1\'   3   3  3.33       \'OIL\'  7* /   \n"
            "     \'CW_1\'        \'CG1\'   3   3  3.33       \'OIL\'  7* /   \n"
            "     \'BW_2\'        \'CG2\'   3   3  3.33       \'OIL\'  7* /   \n"
            "     \'AW_3\'        \'CG2\'   2   5  3.92       \'OIL\'  7* /   \n"
            "/\n";

    return input;
}

static std::string createDeckWithWellsAndCompletionData() {
    std::string input =
      "START             -- 0 \n"
      "1 NOV 1979 / \n"
      "SCHEDULE\n"
      "DATES             -- 1\n"
      " 1 DES 1979/ \n"
      "/\n"
      "WELSPECS\n"
      "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
      "    'OP_2'       'OP'   8   8 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
      "    'OP_3'       'OP'   7   7 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
      "/\n"
      "COMPDAT\n"
      " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
      " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
      " 'OP_2'  8  8   1   3 'OPEN' 1*    1.168   0.311   107.872 1*  1*  'Y'  21.925 / \n"
      " 'OP_2'  8  7   3   3 'OPEN' 1*   15.071   0.311  1391.859 1*  1*  'Y'  21.920 / \n"
      " 'OP_2'  8  7   3   6 'OPEN' 1*    6.242   0.311   576.458 1*  1*  'Y'  21.915 / \n"
      " 'OP_3'  7  7   1   1 'OPEN' 1*   27.412   0.311  2445.337 1*  1*  'Y'  18.521 / \n"
      " 'OP_3'  7  7   2   2 'OPEN' 1*   55.195   0.311  4923.842 1*  1*  'Y'  18.524 / \n"
      "/\n"
      "DATES             -- 2,3\n"
      " 10  JUL 2007 / \n"
      " 10  AUG 2007 / \n"
      "/\n"
      "COMPDAT\n" // with defaulted I and J
      " 'OP_1'  0  *   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
      "/\n";

    return input;
}

static std::string createDeckRFTConfig() {
    return R"(RUNSPEC
START             -- 0
  1 NOV 1979 /

SCHEDULE
DATES             -- 1  (sim step = 0)
 1 DES 1979/
/
WELSPECS
    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
    'OP_2'       'OP'   8   8 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
    'OP_3'       'OP'   7   7 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
/
COMPDAT
 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 /
 'OP_2'  8  8   1   3 'OPEN' 1*    1.168   0.311   107.872 1*  1*  'Y'  21.925 /
 'OP_2'  8  7   3   3 'OPEN' 1*   15.071   0.311  1391.859 1*  1*  'Y'  21.920 /
 'OP_2'  8  7   3   6 'OPEN' 1*    6.242   0.311   576.458 1*  1*  'Y'  21.915 /
 'OP_3'  7  7   1   1 'OPEN' 1*   27.412   0.311  2445.337 1*  1*  'Y'  18.521 /
 'OP_3'  7  7   2   2 'OPEN' 1*   55.195   0.311  4923.842 1*  1*  'Y'  18.524 /
/
WELOPEN
 'OP_2' 'OPEN' /
/
DATES             -- 2  (sim step = 1)
 10  JUL 2007 /
/
WRFTPLT
  'OP_2'       'YES'        'NO'        'NO' /
/
DATES             -- 3  (sim step = 2)
 10  AUG 2007 /
/
COMPDAT
-- with defaulted I and J
 'OP_1'  0  *   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/
END
)";
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckMissingReturnsDefaults) {
    Deck deck;
    Parser parser;
    deck.addKeyword( DeckKeyword( parser.getKeyword("SCHEDULE" )));
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule(deck, grid , fp, runspec);
    BOOST_CHECK_EQUAL( schedule.getStartTime() , TimeMap::mkdate(1983, 1 , 1));
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWellsOrdered) {
    const auto& schedule = make_schedule( createDeckWithWellsOrdered() );
    auto well_names = schedule.wellNames();

    BOOST_CHECK_EQUAL( "CW_1" , well_names[0]);
    BOOST_CHECK_EQUAL( "BW_2" , well_names[1]);
    BOOST_CHECK_EQUAL( "AW_3" , well_names[2]);

    auto group_names = schedule.groupNames();
    BOOST_CHECK_EQUAL( "FIELD", group_names[0]);
    BOOST_CHECK_EQUAL( "CG", group_names[1]);
    BOOST_CHECK_EQUAL( "BG", group_names[2]);
    BOOST_CHECK_EQUAL( "AG", group_names[3]);

    auto restart_groups = schedule.restart_groups(0);
    BOOST_REQUIRE_EQUAL(restart_groups.size(), 4U);
    for (std::size_t group_index = 0; group_index < restart_groups.size() - 1; group_index++) {
        const auto& group_ptr = restart_groups[group_index];
        BOOST_CHECK_EQUAL(group_ptr->insert_index(), group_index + 1);
    }
    const auto& field_ptr = restart_groups.back();
    BOOST_CHECK_EQUAL(field_ptr->insert_index(), 0U);
    BOOST_CHECK_EQUAL(field_ptr->name(), "FIELD");
}

static bool has_well( const std::vector<Well>& wells, const std::string& well_name) {
    for (const auto& well : wells )
        if (well.name( ) == well_name)
            return true;
    return false;
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWellsOrderedGRUPTREE) {
    const auto& schedule = make_schedule( createDeckWithWellsOrderedGRUPTREE() );

    BOOST_CHECK_THROW( schedule.getChildWells2( "NO_SUCH_GROUP" , 1 ), std::invalid_argument);
    {
        auto field_wells = schedule.getChildWells2("FIELD" , 0);
        BOOST_CHECK_EQUAL( field_wells.size() , 4U);

        BOOST_CHECK( has_well( field_wells, "DW_0" ));
        BOOST_CHECK( has_well( field_wells, "CW_1" ));
        BOOST_CHECK( has_well( field_wells, "BW_2" ));
        BOOST_CHECK( has_well( field_wells, "AW_3" ));
    }

    {
        auto platform_wells = schedule.getChildWells2("PLATFORM" , 0);
        BOOST_CHECK_EQUAL( platform_wells.size() , 4U);

        BOOST_CHECK( has_well( platform_wells, "DW_0" ));
        BOOST_CHECK( has_well( platform_wells, "CW_1" ));
        BOOST_CHECK( has_well( platform_wells, "BW_2" ));
        BOOST_CHECK( has_well( platform_wells, "AW_3" ));
    }

    {
        auto child_wells1 = schedule.getChildWells2("CG1" , 0);
        BOOST_CHECK_EQUAL( child_wells1.size() , 2U);

        BOOST_CHECK( has_well( child_wells1, "DW_0" ));
        BOOST_CHECK( has_well( child_wells1, "CW_1" ));
    }

    {
        auto parent_wells2 = schedule.getChildWells2("PG2" , 0);
        BOOST_CHECK_EQUAL( parent_wells2.size() , 2U);

        BOOST_CHECK( has_well( parent_wells2, "BW_2" ));
        BOOST_CHECK( has_well( parent_wells2, "AW_3" ));
    }
    auto group_names = schedule.groupNames("P*", 0);
    BOOST_CHECK( std::find(group_names.begin(), group_names.end(), "PG1") != group_names.end() );
    BOOST_CHECK( std::find(group_names.begin(), group_names.end(), "PG2") != group_names.end() );
    BOOST_CHECK( std::find(group_names.begin(), group_names.end(), "PLATFORM") != group_names.end() );
}

BOOST_AUTO_TEST_CASE(GroupTree2TEST) {
    const auto& schedule = make_schedule( createDeckWithWellsOrderedGRUPTREE() );

    BOOST_CHECK_THROW( schedule.groupTree("NO_SUCH_GROUP", 0), std::invalid_argument);
    auto cg1 = schedule.getGroup("CG1", 0);
    BOOST_CHECK( cg1.hasWell("DW_0"));
    BOOST_CHECK( cg1.hasWell("CW_1"));

    auto cg1_tree = schedule.groupTree("CG1", 0);
    BOOST_CHECK_EQUAL(cg1_tree.wells().size(), 2U);

    auto gt = schedule.groupTree(0);
    BOOST_CHECK_EQUAL(gt.wells().size(), 0U);
    BOOST_CHECK_EQUAL(gt.group().name(), "FIELD");
    BOOST_CHECK_THROW(gt.parent_name(), std::invalid_argument);

    auto cg = gt.groups();
    auto pg = cg[0];
    BOOST_CHECK_EQUAL(cg.size(), 1U);
    BOOST_CHECK_EQUAL(pg.group().name(), "PLATFORM");
    BOOST_CHECK_EQUAL(pg.parent_name(), "FIELD");
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWithStart) {
    const auto& schedule = make_schedule( createDeck() );
    BOOST_CHECK_EQUAL( schedule.getStartTime() , TimeMap::mkdate(1998, 3  , 8 ));
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWithSCHEDULENoThrow) {
    BOOST_CHECK_NO_THROW( make_schedule( "SCHEDULE" ));
}

BOOST_AUTO_TEST_CASE(EmptyScheduleHasNoWells) {
    const auto& schedule = make_schedule( createDeck() );
    BOOST_CHECK_EQUAL( 0U , schedule.numWells() );
    BOOST_CHECK_EQUAL( false , schedule.hasWell("WELL1") );
    BOOST_CHECK_THROW( schedule.getWell("WELL2", 0) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(EmptyScheduleHasFIELDGroup) {
    const auto& schedule = make_schedule( createDeck() );

    BOOST_CHECK_EQUAL( 1U , schedule.numGroups() );
    BOOST_CHECK_EQUAL( true , schedule.hasGroup("FIELD") );
    BOOST_CHECK_EQUAL( false , schedule.hasGroup("GROUP") );
    BOOST_CHECK_THROW( schedule.getGroup("GROUP", 0) , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(HasGroup_At_Time) {
    const auto input = std::string { R"(
SCHEDULE
WELSPECS
-- Group 'P' exists from the first report step
  'P1' 'P' 1 1  2502.5  'OIL' /
/
WCONPROD
  'P1' 'OPEN' 'ORAT'  123.4  4*  50.0 /
/
TSTEP
  10 20 30 40 /
WELSPECS
-- Group 'I' does not exist before now (report step 4, zero-based = 3)
  'I1' 'I' 5 5 2522.5 'WATER' /
/
WCONINJE
  'I1' 'WATER'  'OPEN'  'RATE'  200  1*  450.0 /
/
TSTEP
  50 50 /
END
)"
    };

    const auto sched = make_schedule(input);

    BOOST_CHECK_MESSAGE(sched.hasGroup("P"), R"(Group "P" Must Exist)");
    BOOST_CHECK_MESSAGE(sched.hasGroup("I"), R"(Group "I" Must Exist)");

    BOOST_CHECK_MESSAGE(  sched.hasGroup("P", 3), R"(Group "P" Must Exist at Report Step 3)");
    BOOST_CHECK_MESSAGE(! sched.hasGroup("I", 3), R"(Group "I" Must NOT Exist at Report Step 3)");
    BOOST_CHECK_MESSAGE(  sched.hasGroup("I", 4), R"(Group "I" Must Exist at Report Step 4)");

    BOOST_CHECK_MESSAGE(sched.hasGroup("P", 6), R"(Group "P" Must Exist At Last Report Step)");
    BOOST_CHECK_MESSAGE(sched.hasGroup("I", 6), R"(Group "I" Must Exist At Last Report Step)");

    BOOST_CHECK_MESSAGE(! sched.hasGroup("P", 7), R"(Group "P" Must NOT Exist Immediately After Last Report Step)");
    BOOST_CHECK_MESSAGE(! sched.hasGroup("I", 7), R"(Group "I" Must NOT Exist Immediately After Last Report Step)");

    BOOST_CHECK_MESSAGE(! sched.hasGroup("P", 1729), R"(Group "P" Must NOT Exist Long After Last Report Step)");
    BOOST_CHECK_MESSAGE(! sched.hasGroup("I", 1729), R"(Group "I" Must NOT Exist Long After Last Report Step)");

    BOOST_CHECK_THROW(sched.getGroup("I", 3), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(WellsIterator_Empty_EmptyVectorReturned) {
    const auto& schedule = make_schedule( createDeck() );

    const auto wells_alltimesteps = schedule.getWellsatEnd();
    BOOST_CHECK_EQUAL(0U, wells_alltimesteps.size());

    const auto wells_t0 = schedule.getWells(0);
    BOOST_CHECK_EQUAL(0U, wells_t0.size());

    // The time argument is beyond the length of the vector
    BOOST_CHECK_THROW(schedule.getWells(1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(WellsIterator_HasWells_WellsReturned) {
    const auto& schedule = make_schedule( createDeckWithWells() );
    size_t timeStep = 0;

    const auto wells_alltimesteps = schedule.getWellsatEnd();
    BOOST_CHECK_EQUAL(3U, wells_alltimesteps.size());
    const auto wells_t0 = schedule.getWells(timeStep);
    BOOST_CHECK_EQUAL(1U, wells_t0.size());
    const auto wells_t3 = schedule.getWells(3);
    BOOST_CHECK_EQUAL(3U, wells_t3.size());
}

BOOST_AUTO_TEST_CASE(ReturnNumWellsTimestep) {
    const auto& schedule = make_schedule( createDeckWithWells() );

    BOOST_CHECK_EQUAL(schedule.numWells(0), 1U);
    BOOST_CHECK_EQUAL(schedule.numWells(1), 1U);
    BOOST_CHECK_EQUAL(schedule.numWells(2), 1U);
    BOOST_CHECK_EQUAL(schedule.numWells(3), 3U);
}

BOOST_AUTO_TEST_CASE(TestCrossFlowHandling) {
    const auto& schedule = make_schedule( createDeckForTestingCrossFlow() );

    BOOST_CHECK_EQUAL(schedule.getWell("BAN", 0).getAllowCrossFlow(), false);
    BOOST_CHECK_EQUAL(schedule.getWell("ALLOW", 0).getAllowCrossFlow(), true);
    BOOST_CHECK_EQUAL(schedule.getWell("DEFAULT", 0).getAllowCrossFlow(), true);
    BOOST_CHECK(Well::Status::SHUT == schedule.getWell("BAN", 0).getStatus());
    BOOST_CHECK(Well::Status::OPEN == schedule.getWell("BAN", 1).getStatus());
    BOOST_CHECK(Well::Status::OPEN == schedule.getWell("BAN", 2).getStatus());
    BOOST_CHECK(Well::Status::SHUT == schedule.getWell("BAN", 3).getStatus());
    BOOST_CHECK(Well::Status::SHUT == schedule.getWell("BAN", 4).getStatus()); // not allow to open
    BOOST_CHECK(Well::Status::OPEN == schedule.getWell("BAN", 5).getStatus());
}

static std::string createDeckWithWellsAndConnectionDataWithWELOPEN() {
    std::string input =
            "START             -- 0 \n"
                    "1 NOV 1979 / \n"
                    "SCHEDULE\n"
                    "DATES             -- 1\n"
                    " 1 DES 1979/ \n"
                    "/\n"
                    "WELSPECS\n"
                    "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                    "    'OP_2'       'OP'   8   8 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                    "    'OP_3'       'OP'   7   7 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                    "/\n"
                    "COMPDAT\n"
                    " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
                    " 'OP_2'  8  8   1   3 'OPEN' 1*    1.168   0.311   107.872 1*  1*  'Y'  21.925 / \n"
                    " 'OP_2'  8  7   3   3 'OPEN' 1*   15.071   0.311  1391.859 1*  1*  'Y'  21.920 / \n"
                    " 'OP_2'  8  7   3   6 'OPEN' 1*    6.242   0.311   576.458 1*  1*  'Y'  21.915 / \n"
                    " 'OP_3'  7  7   1   1 'OPEN' 1*   27.412   0.311  2445.337 1*  1*  'Y'  18.521 / \n"
                    " 'OP_3'  7  7   2   2 'OPEN' 1*   55.195   0.311  4923.842 1*  1*  'Y'  18.524 / \n"
                    "/\n"
                    "DATES             -- 2,3\n"
                    " 10  JUL 2007 / \n"
                    " 10  AUG 2007 / \n"
                    "/\n"
                    "COMPDAT\n"
                    " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_1' SHUT / \n"
                    " '*'    OPEN 0 0 3 / \n"
                    " 'OP_2' SHUT 0 0 0 4 6 / \n "
                    " 'OP_3' SHUT 0 0 0 / \n"
                    "/\n"
                    "DATES             -- 4\n"
                    " 10  JUL 2008 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_1' OPEN / \n"
                    " 'OP_2' OPEN 0 0 0 4 6 / \n "
                    " 'OP_3' OPEN 0 0 0 / \n"
                    "/\n"
                    "DATES             -- 5\n"
                    " 10  OKT 2008 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_1' SHUT 0 0 0 0 0 / \n "
                    "/\n";

    return input;
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWellsAndConnectionDataWithWELOPEN) {
    const auto& schedule = make_schedule(createDeckWithWellsAndConnectionDataWithWELOPEN());
    {
        constexpr auto well_shut = Well::Status::SHUT;
        constexpr auto well_open = Well::Status::OPEN;

        BOOST_CHECK(well_shut == schedule.getWell("OP_1", 3).getStatus(  ));
        BOOST_CHECK(well_open == schedule.getWell("OP_1", 4).getStatus(  ));
        BOOST_CHECK(well_shut == schedule.getWell("OP_1", 5).getStatus(  ));
    }
    {
        constexpr auto comp_shut = Connection::State::SHUT;
        constexpr auto comp_open = Connection::State::OPEN;
        {
            const auto& well = schedule.getWell("OP_2", 3);
            const auto& cs = well.getConnections( );

            BOOST_CHECK_EQUAL( 7U, cs.size() );
            BOOST_CHECK(comp_shut == cs.getFromIJK( 7, 6, 2 ).state());
            BOOST_CHECK(comp_shut == cs.getFromIJK( 7, 6, 3 ).state());
            BOOST_CHECK(comp_shut == cs.getFromIJK( 7, 6, 4 ).state());
            BOOST_CHECK(comp_open == cs.getFromIJK( 7, 7, 2 ).state());
        }
        {
            const auto& well = schedule.getWell("OP_2", 4);
            const auto& cs2 = well.getConnections( );
            BOOST_CHECK(comp_open == cs2.getFromIJK( 7, 6, 2 ).state());
            BOOST_CHECK(comp_open == cs2.getFromIJK( 7, 6, 3 ).state());
            BOOST_CHECK(comp_open == cs2.getFromIJK( 7, 6, 4 ).state());
            BOOST_CHECK(comp_open == cs2.getFromIJK( 7, 7, 2 ).state());
        }
        {
            const auto& well = schedule.getWell("OP_3", 3);
            const auto& cs3 = well.getConnections(  );
            BOOST_CHECK(comp_shut == cs3.get( 0 ).state());
        }
        {
            const auto& well = schedule.getWell("OP_3", 4);
            const auto& cs4 = well.getConnections(  );
            BOOST_CHECK(comp_open == cs4.get( 0 ).state());
        }
    }
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWithWELOPEN_TryToOpenWellWithShutCompletionsDoNotOpenWell) {
    Ewoms::Parser parser;
    std::string input =
        "START             -- 0 \n"
        "1 NOV 1979 / \n"
        "SCHEDULE\n"
        "DATES             -- 1\n"
        " 1 DES 1979/ \n"
        "/\n"
        "WELSPECS\n"
        "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
        "/\n"
        "COMPDAT\n"
        " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
        " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
        " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
        "/\n"
        "DATES             -- 2\n"
        " 10  JUL 2008 / \n"
        "/\n"
        "WELOPEN\n"
        " 'OP_1' OPEN / \n"
        "/\n"
        "DATES             -- 3\n"
        " 10  OKT 2008 / \n"
        "/\n"
        "WELOPEN\n"
        " 'OP_1' SHUT 0 0 0 0 0 / \n "
        "/\n"
        "DATES             -- 4\n"
        " 10  NOV 2008 / \n"
        "/\n"
        "WELOPEN\n"
        " 'OP_1' OPEN / \n "
        "/\n";

    const auto& schedule = make_schedule(input);
    const auto& well2_3 = schedule.getWell("OP_1",3);
    const auto& well2_4 = schedule.getWell("OP_1",4);
    BOOST_CHECK(Well::Status::SHUT == well2_3.getStatus());
    BOOST_CHECK(Well::Status::SHUT == well2_4.getStatus());
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWithWELOPEN_CombineShutCompletionsAndAddNewCompletionsDoNotShutWell) {
  Ewoms::Parser parser;
  std::string input =
          "START             -- 0 \n"
                  "1 NOV 1979 / \n"
                  "SCHEDULE\n"
                  "DATES             -- 1\n"
                  " 1 DES 1979/ \n"
                  "/\n"
                  "WELSPECS\n"
                  "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                  "/\n"
                  "COMPDAT\n"
                  " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                  " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
                  " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                  "/\n"
                  "DATES             -- 2\n"
                  " 10  JUL 2008 / \n"
                  "/\n"
                  "WELOPEN\n"
                  " 'OP_1' OPEN / \n"
                  "/\n"
                  "DATES             -- 3\n"
                  " 10  OKT 2008 / \n"
                  "/\n"
                  "WELOPEN\n"
                  " 'OP_1' SHUT 0 0 0 0 0 / \n "
                  "/\n"
                  "COMPDAT\n"
                  " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                  "/\n"
                  "DATES             -- 4\n"
                  " 10  NOV 2008 / \n"
                  "/\n"
                  "WELOPEN\n"
                  " 'OP_1' SHUT 0 0 0 0 0 / \n "
                  "/\n"
                  "DATES             -- 5\n"
                  " 11  NOV 2008 / \n"
                  "/\n"
                  "COMPDAT\n"
                  " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                  "/\n"
                  "DATES             -- 6\n"
                  " 12  NOV 2008 / \n"
                  "/\n";

  const auto& schedule = make_schedule(input);
  const auto& well_3 = schedule.getWell("OP_1", 3);
  const auto& well_4 = schedule.getWell("OP_1", 4);
  const auto& well_5 = schedule.getWell("OP_1", 5);
  // timestep 3. Close all completions with WELOPEN and immediately open new completions with COMPDAT.
  BOOST_CHECK(Well::Status::OPEN == well_3.getStatus());
  BOOST_CHECK( !schedule.hasWellGroupEvent( "OP_1", ScheduleEvents::WELL_STATUS_CHANGE , 3 ));
  // timestep 4. Close all completions with WELOPEN. The well will be shut since no completions
  // are open.
  BOOST_CHECK(Well::Status::SHUT == well_4.getStatus());
  BOOST_CHECK( schedule.hasWellGroupEvent( "OP_1", ScheduleEvents::WELL_STATUS_CHANGE , 4 ));
  // timestep 5. Open new completions. But keep the well shut,
  BOOST_CHECK(Well::Status::SHUT == well_5.getStatus());
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWithWRFT) {
    Ewoms::Parser parser;
    std::string input =
            "START             -- 0 \n"
                    "1 NOV 1979 / \n"
                    "SCHEDULE\n"
                    "DATES             -- 1\n"
                    " 1 DES 1979/ \n"
                    "/\n"
                    "WELSPECS\n"
                    "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                    "    'OP_2'       'OP'   4   4 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                    "/\n"
                    "COMPDAT\n"
                    " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
                    " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    " 'OP_2'  4  4   4  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    "/\n"
                    "DATES             -- 2\n"
                    " 10  OKT 2008 / \n"
                    "/\n"
                    "WRFT \n"
                    "/ \n"
                    "WELOPEN\n"
                    " 'OP_1' OPEN / \n"
                    "/\n"
                    "DATES             -- 3\n"
                    " 10  NOV 2008 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_2' OPEN / \n"
                    "/\n"
                    "DATES             -- 4\n"
                    " 30  NOV 2008 / \n"
                    "/\n";

    const auto& schedule = make_schedule(input);
    const auto& rft_config = schedule.rftConfig();

    BOOST_CHECK_EQUAL(2U, rft_config.firstRFTOutput());
    BOOST_CHECK_EQUAL(true, rft_config.rft("OP_1", 2));
    BOOST_CHECK_EQUAL(true, rft_config.rft("OP_2", 3));
}

BOOST_AUTO_TEST_CASE(CreateScheduleDeckWithWRFTPLT) {
    Ewoms::Parser parser;
    std::string input =
            "START             -- 0 \n"
                    "1 NOV 1979 / \n"
                    "SCHEDULE\n"
                    "DATES             -- 1\n"
                    " 1 DES 1979/ \n"
                    "/\n"
                    "WELSPECS\n"
                    "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                    "/\n"
                    "COMPDAT\n"
                    " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
                    " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_1' SHUT / \n"
                    "/\n"
                    "DATES             -- 2\n"
                    " 10  OKT 2006 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_1' SHUT / \n"
                    "/\n"
                    "WRFTPLT \n"
                    " 'OP_1' FOPN / \n"
                    "/ \n"
                    "DATES             -- 3\n"
                    " 10  OKT 2007 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_1' OPEN 0 0 0 0 0 / \n"
                    "/\n"
                    "DATES             -- 4\n"
                    " 10  OKT 2008 / \n"
                    "/\n"
                    "WELOPEN\n"
                    " 'OP_1' OPEN / \n"
                    "/\n"
                    "DATES             -- 5\n"
                    " 10  NOV 2008 / \n"
                    "/\n";

    const auto& schedule = make_schedule(input);
    const auto& well = schedule.getWell("OP_1", 4);
    BOOST_CHECK(Well::Status::OPEN == well.getStatus());

    const auto& rft_config = schedule.rftConfig();
    BOOST_CHECK_EQUAL(rft_config.rft("OP_1", 3),false);
    BOOST_CHECK_EQUAL(rft_config.rft("OP_1", 4),true);
    BOOST_CHECK_EQUAL(rft_config.rft("OP_1", 5),false);
}

BOOST_AUTO_TEST_CASE(createDeckWithWeltArg) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONPROD\n"
            " 'OP_1'      'OPEN'      'ORAT'      0.000      0.000      0.000  5* / \n"
            "/\n"
            "DATES             -- 2\n"
            " 20  JAN 2010 / \n"
            "/\n"
            "WELTARG\n"
            " OP_1     ORAT        1300 /\n"
            " OP_1     WRAT        1400 /\n"
            " OP_1     GRAT        1500.52 /\n"
            " OP_1     LRAT        1600.58 /\n"
            " OP_1     RESV        1801.05 /\n"
            " OP_1     BHP         1900 /\n"
            " OP_1     THP         2000 /\n"
            " OP_1     GUID        2300.14 /\n"
            "/\n";

    const auto& schedule = make_schedule(input);
    Ewoms::UnitSystem unitSystem = UnitSystem( UnitSystem::UnitType::UNIT_TYPE_METRIC );
    double siFactorL = unitSystem.parse("LiquidSurfaceVolume/Time").getSIScaling();
    double siFactorG = unitSystem.parse("GasSurfaceVolume/Time").getSIScaling();
    double siFactorP = unitSystem.parse("Pressure").getSIScaling();
    SummaryState st(std::chrono::system_clock::now());

    const auto& well_1 = schedule.getWell("OP_1", 1);
    const auto wpp_1 = well_1.getProductionProperties();
    BOOST_CHECK_EQUAL(wpp_1.WaterRate.get<double>(), 0);
    BOOST_CHECK (wpp_1.hasProductionControl( Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK (!wpp_1.hasProductionControl( Ewoms::Well::ProducerCMode::RESV) );

    const auto& well_2 = schedule.getWell("OP_1", 2);
    const auto wpp_2 = well_2.getProductionProperties();
    const auto prod_controls = wpp_2.controls(st, 0);

    BOOST_CHECK_EQUAL(prod_controls.oil_rate, 1300 * siFactorL);
    BOOST_CHECK_EQUAL(prod_controls.water_rate, 1400 * siFactorL);
    BOOST_CHECK_EQUAL(prod_controls.gas_rate, 1500.52 * siFactorG);
    BOOST_CHECK_EQUAL(prod_controls.liquid_rate, 1600.58 * siFactorL);
    BOOST_CHECK_EQUAL(prod_controls.resv_rate, 1801.05 * siFactorL);
    BOOST_CHECK_EQUAL(prod_controls.bhp_limit, 1900 * siFactorP);
    BOOST_CHECK_EQUAL(prod_controls.thp_limit, 2000 * siFactorP);

    BOOST_CHECK (wpp_2.hasProductionControl( Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK (wpp_2.hasProductionControl( Ewoms::Well::ProducerCMode::RESV) );

}

BOOST_AUTO_TEST_CASE(createDeckWithWeltArg_UDA) {
    std::string input = R"(
START             -- 0
19 JUN 2007 /
SCHEDULE
DATES             -- 1
 10  OKT 2008 /
/

UDQ
   ASSIGN WUORAT 10 /
   ASSIGN WUWRAT 20 /
/

WELSPECS
    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  /
/
COMPDAT
 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 /
 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/
WCONPROD
 'OP_1'      'OPEN'      'ORAT'      0.000      0.000      0.000  5* /
/
DATES             -- 2
 20  JAN 2010 /
/
WELTARG
 OP_1     ORAT        WUORAT /
 OP_1     WRAT        WUWRAT /
/
)";

    const auto& schedule = make_schedule(input);
    SummaryState st(std::chrono::system_clock::now());
    Ewoms::UnitSystem unitSystem = UnitSystem( UnitSystem::UnitType::UNIT_TYPE_METRIC );
    double siFactorL = unitSystem.parse("LiquidSurfaceVolume/Time").getSIScaling();

    st.update_well_var("OP_1", "WUORAT", 10);
    st.update_well_var("OP_1", "WUWRAT", 20);

    const auto& well_1 = schedule.getWell("OP_1", 1);
    const auto wpp_1 = well_1.getProductionProperties();
    BOOST_CHECK_EQUAL(wpp_1.OilRate.get<double>(), 0);
    BOOST_CHECK_EQUAL(wpp_1.WaterRate.get<double>(), 0);
    BOOST_CHECK (wpp_1.hasProductionControl( Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK (!wpp_1.hasProductionControl( Ewoms::Well::ProducerCMode::RESV) );

    const auto& well_2 = schedule.getWell("OP_1", 2);
    const auto wpp_2 = well_2.getProductionProperties();
    BOOST_CHECK( wpp_2.OilRate.is<std::string>() );
    BOOST_CHECK_EQUAL( wpp_2.OilRate.get<std::string>(), "WUORAT" );
    BOOST_CHECK_EQUAL( wpp_2.WaterRate.get<std::string>(), "WUWRAT" );
    const auto prod_controls = wpp_2.controls(st, 0);

    BOOST_CHECK_EQUAL(prod_controls.oil_rate, 10 * siFactorL);
    BOOST_CHECK_EQUAL(prod_controls.water_rate, 20 * siFactorL);

    BOOST_CHECK (wpp_2.hasProductionControl( Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK (wpp_2.hasProductionControl( Ewoms::Well::ProducerCMode::WRAT) );
}

BOOST_AUTO_TEST_CASE(createDeckWithWeltArgException) {
    std::string input =
            "SCHEDULE\n"
            "WELTARG\n"
            " OP_1     GRAT        1500.52 /\n"
            " OP_1     LRAT        /\n"
            " OP_1     RESV        1801.05 /\n"
            "/\n";

    BOOST_CHECK_THROW(make_schedule(input), Ewoms::OpmInputError);
}

BOOST_AUTO_TEST_CASE(createDeckWithWeltArgException2) {
    std::string input =
            "SCHEDULE\n"
            "WELTARG\n"
            " OP_1     LRAT        /\n"
            " OP_1     RESV        1801.05 /\n"
            "/\n";
    BOOST_CHECK_THROW(make_schedule(input), Ewoms::OpmInputError);
}

BOOST_AUTO_TEST_CASE(createDeckWithWPIMULT) {
    std::string input =
            "START             -- 0 \n"
                    "19 JUN 2007 / \n"
                    "SCHEDULE\n"
                    "DATES             -- 1\n"
                    " 10  OKT 2008 / \n"
                    "/\n"
                    "WELSPECS\n"
                    "    'OP_1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
                    "/\n"
                    "COMPDAT\n"
                    " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
                    " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    "/\n"
                    "DATES             -- 2\n"
                    " 20  JAN 2010 / \n"
                    "/\n"
                    "WPIMULT\n"
                    "OP_1  1.30 /\n"
                    "/\n"
                    "DATES             -- 3\n"
                    " 20  JAN 2011 / \n"
                    "/\n"
                    "WPIMULT\n"
                    "OP_1  1.30 /\n"
                    "/\n"
                    "DATES             -- 4\n"
                    " 20  JAN 2012 / \n"
                    "/\n"
                    "COMPDAT\n"
                    " 'OP_1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    " 'OP_1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
                    " 'OP_1'  9  9   3  9 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
                    "/\n";

    const auto& schedule = make_schedule(input);
    const auto& cs1 = schedule.getWell("OP_1", 1).getConnections();
    const auto& cs2 = schedule.getWell("OP_1", 2).getConnections();
    const auto& cs3 = schedule.getWell("OP_1", 3).getConnections();
    const auto& cs4 = schedule.getWell("OP_1", 4).getConnections();
    for(size_t i = 0; i < cs2.size(); i++)
        BOOST_CHECK_EQUAL(cs2.get( i ).CF() / cs1.get(i).CF(), 1.3);

    for(size_t i = 0; i < cs3.size(); i++ )
        BOOST_CHECK_EQUAL(cs3.get( i ).CF() / cs1.get(i).CF(), (1.3*1.3));

    for(size_t i = 0; i < cs4.size(); i++ )
        BOOST_CHECK_EQUAL(cs4.get( i ).CF(), cs1.get(i).CF());

    BOOST_CHECK_THROW(schedule.simTime(10000), std::invalid_argument);
    auto sim_time1 = TimeStampUTC{ schedule.simTime(1) };
    BOOST_CHECK_EQUAL(sim_time1.day(), 10);
    BOOST_CHECK_EQUAL(sim_time1.month(), 10);
    BOOST_CHECK_EQUAL(sim_time1.year(), 2008);

    sim_time1 = schedule.simTime(3);
    BOOST_CHECK_EQUAL(sim_time1.day(), 20);
    BOOST_CHECK_EQUAL(sim_time1.month(), 1);
    BOOST_CHECK_EQUAL(sim_time1.year(), 2011);
}

BOOST_AUTO_TEST_CASE(WELSPECS_WGNAME_SPACE) {
        Ewoms::Parser parser;
        const std::string input = R"(
        START  -- 0
         10 'JAN' 2000 /
        RUNSPEC
        DIMENS
          10 10 10 /
        GRID
        DX
        1000*0.25 /
        DY
        1000*0.25 /
        DZ
        1000*0.25 /
        TOPS
        100*0.25 /
        SCHEDULE
        DATES             -- 1
         10  OKT 2008 /
        /
        WELSPECS
            ' PROD1' 'G1'  1 1 10 'OIL' /
            'PROD2' 'G2'  2 2 10 'OIL' /
            'PROD3' 'H1'  3 3 10 'OIL' /
        /
        GCONPROD
        'G1' 'ORAT' 1000 /
        /
        DATES             -- 2
         10  NOV 2008 /
        /
        GCONPROD
        'G*' 'ORAT' 2000 /
        /
        )";

        auto deck = parser.parseString(input);
        EclipseGrid grid( deck );
        TableManager table ( deck );
        FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
        Runspec runspec (deck);
        ParseContext parseContext;
        ErrorGuard errors;

        parseContext.update(ParseContext::PARSE_WGNAME_SPACE, InputError::THROW_EXCEPTION);
        BOOST_CHECK_THROW( Ewoms::Schedule(deck,  grid, fp, runspec, parseContext, errors), Ewoms::OpmInputError);

        parseContext.update(ParseContext::PARSE_WGNAME_SPACE, InputError::IGNORE);
        BOOST_CHECK_NO_THROW( Ewoms::Schedule(deck,  grid, fp, runspec, parseContext, errors));
}

BOOST_AUTO_TEST_CASE(createDeckModifyMultipleGCONPROD) {
        const std::string input = R"(
        START  -- 0
         10 'JAN' 2000 /
        RUNSPEC
        DIMENS
          10 10 10 /
        GRID
        DX
        1000*0.25 /
        DY
        1000*0.25 /
        DZ
        1000*0.25 /
        TOPS
        100*0.25 /
        SCHEDULE
        DATES             -- 1
         10  OKT 2008 /
        /
        WELSPECS
            'PROD1' 'G1'  1 1 10 'OIL' /
            'PROD2' 'G2'  2 2 10 'OIL' /
            'PROD3' 'H1'  3 3 10 'OIL' /
        /
        GCONPROD
        'G1' 'ORAT' 1000 /
        /
        DATES             -- 2
         10  NOV 2008 /
        /
        GCONPROD
        'G*' 'ORAT' 2000 0 0 0 'NONE' 'YES' 148 'OIL'/
        /
        )";

        const auto& schedule = make_schedule(input);
        Ewoms::SummaryState st(std::chrono::system_clock::now());

        Ewoms::UnitSystem unitSystem = UnitSystem(UnitSystem::UnitType::UNIT_TYPE_METRIC);
        double siFactorL = unitSystem.parse("LiquidSurfaceVolume/Time").getSIScaling();

        {
            auto g = schedule.getGroup("G1", 1);
            BOOST_CHECK_EQUAL(g.productionControls(st).oil_target, 1000 * siFactorL);
            BOOST_CHECK(g.has_control(Group::ProductionCMode::ORAT));
            BOOST_CHECK(!g.has_control(Group::ProductionCMode::WRAT));
            BOOST_CHECK_EQUAL(g.productionControls(st).guide_rate, 0);
        }
        {
            auto g = schedule.getGroup("G1", 2);
            BOOST_CHECK_EQUAL(g.productionControls(st).oil_target, 2000 * siFactorL);
            BOOST_CHECK_EQUAL(g.productionControls(st).guide_rate, 148);
            BOOST_CHECK_EQUAL(true, g.productionControls(st).guide_rate_def == Group::GuideRateTarget::OIL);
        }

        auto g2 = schedule.getGroup("G2", 2);
        BOOST_CHECK_EQUAL(g2.productionControls(st).oil_target, 2000 * siFactorL);

        auto gh = schedule.getGroup("H1", 1);

        BOOST_CHECK(  !schedule.hasWellGroupEvent( "G2", ScheduleEvents::GROUP_PRODUCTION_UPDATE , 1 ));
        BOOST_CHECK(  schedule.hasWellGroupEvent( "G2", ScheduleEvents::GROUP_PRODUCTION_UPDATE , 2 ));

}

BOOST_AUTO_TEST_CASE(createDeckWithDRSDT) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "DRSDT\n"
            "0.0003\n"
            "/\n";

    const auto& schedule = make_schedule(input);
    size_t currentStep = 1;
    BOOST_CHECK_EQUAL(schedule.hasOilVaporizationProperties(), true);
    const auto& ovap = schedule.getOilVaporizationProperties(currentStep);

    BOOST_CHECK_EQUAL(true,   ovap.getOption(0));
    BOOST_CHECK(ovap.getType() == OilVaporizationProperties::OilVaporization::DRDT);

    BOOST_CHECK_EQUAL(true,   ovap.drsdtActive());
    BOOST_CHECK_EQUAL(false,   ovap.drvdtActive());
}

BOOST_AUTO_TEST_CASE(createDeckWithDRSDTR) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "TABDIMS\n"
            " 1* 3 \n "
            "/\n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "DRSDTR\n"
            "0 /\n"
            "1 /\n"
            "2 /\n";

    const auto& schedule = make_schedule(input);
    size_t currentStep = 1;
    BOOST_CHECK_EQUAL(schedule.hasOilVaporizationProperties(), true);
    const auto& ovap = schedule.getOilVaporizationProperties(currentStep);
    auto unitSystem =  UnitSystem::newMETRIC();
    for (int i = 0; i < 3; ++i) {
        double value = unitSystem.to_si( UnitSystem::measure::gas_surface_rate, i );
        BOOST_CHECK_EQUAL(value, ovap.getMaxDRSDT(i));
        BOOST_CHECK_EQUAL(true,   ovap.getOption(i));
    }

    BOOST_CHECK(ovap.getType() == OilVaporizationProperties::OilVaporization::DRDT);
    BOOST_CHECK_EQUAL(true,   ovap.drsdtActive());
    BOOST_CHECK_EQUAL(false,   ovap.drvdtActive());
}

BOOST_AUTO_TEST_CASE(createDeckWithDRSDTthenDRVDT) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "DRSDT\n"
            "0.0003\n"
            "/\n"
            "DATES             -- 2\n"
            " 10  OKT 2009 / \n"
            "/\n"
            "DRVDT\n"
            "0.100\n"
            "/\n"
            "DATES             -- 3\n"
            " 10  OKT 2010 / \n"
            "/\n"
            "VAPPARS\n"
            "2 0.100\n"
            "/\n";

    const auto& schedule = make_schedule(input);
    BOOST_CHECK_EQUAL(schedule.hasOilVaporizationProperties(), true);

    const OilVaporizationProperties& ovap1 = schedule.getOilVaporizationProperties(1);
    BOOST_CHECK(ovap1.getType() == OilVaporizationProperties::OilVaporization::DRDT);
    BOOST_CHECK_EQUAL(true,   ovap1.drsdtActive());
    BOOST_CHECK_EQUAL(false,   ovap1.drvdtActive());

    const OilVaporizationProperties& ovap2 = schedule.getOilVaporizationProperties(2);
    //double value =  ovap2.getMaxDRVDT(0);
    //BOOST_CHECK_EQUAL(1.1574074074074074e-06, value);
    BOOST_CHECK(ovap2.getType() == OilVaporizationProperties::OilVaporization::DRDT);
    BOOST_CHECK_EQUAL(true,   ovap2.drvdtActive());
    BOOST_CHECK_EQUAL(true,   ovap2.drsdtActive());

    const OilVaporizationProperties& ovap3 = schedule.getOilVaporizationProperties(3);
    BOOST_CHECK(ovap3.getType() == OilVaporizationProperties::OilVaporization::VAPPARS);
    BOOST_CHECK_EQUAL(false,   ovap3.drvdtActive());
    BOOST_CHECK_EQUAL(false,   ovap3.drsdtActive());

}

BOOST_AUTO_TEST_CASE(createDeckWithVAPPARS) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "VAPPARS\n"
            "2 0.100\n"
            "/\n";

    const auto& schedule = make_schedule(input);
    size_t currentStep = 1;
    BOOST_CHECK_EQUAL(schedule.hasOilVaporizationProperties(), true);
    const OilVaporizationProperties& ovap = schedule.getOilVaporizationProperties(currentStep);
    BOOST_CHECK(ovap.getType() == OilVaporizationProperties::OilVaporization::VAPPARS);
    double vap1 =  ovap.vap1();
    BOOST_CHECK_EQUAL(2, vap1);
    double vap2 =  ovap.vap2();
    BOOST_CHECK_EQUAL(0.100, vap2);
    BOOST_CHECK_EQUAL(false,   ovap.drsdtActive());
    BOOST_CHECK_EQUAL(false,   ovap.drvdtActive());

}

BOOST_AUTO_TEST_CASE(createDeckWithOutOilVaporizationProperties) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n";

    const auto& schedule = make_schedule(input);
    BOOST_CHECK_EQUAL(schedule.hasOilVaporizationProperties(), false);
}

BOOST_AUTO_TEST_CASE(changeBhpLimitInHistoryModeWithWeltarg) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'P'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'I'       'OP'   1   1 1*     'WATER' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'P'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'I'  1  1   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P' 'OPEN' 'RESV' 6*  500 / \n"
            "/\n"
            "WCONINJH\n"
            " 'I' 'WATER' 1* 100 250 / \n"
            "/\n"
            "WELTARG\n"
            "   'P' 'BHP' 50 / \n"
            "   'I' 'BHP' 600 / \n"
            "/\n"
            "DATES             -- 2\n"
            " 15  OKT 2008 / \n"
            "/\n"
            "WCONHIST\n"
            "   'P' 'OPEN' 'RESV' 6*  500/\n/\n"
            "WCONINJH\n"
            " 'I' 'WATER' 1* 100 250 / \n"
            "/\n"
            "DATES             -- 3\n"
            " 18  OKT 2008 / \n"
            "/\n"
            "WCONHIST\n"
            "   'I' 'OPEN' 'RESV' 6*  /\n/\n"
            "DATES             -- 4\n"
            " 20  OKT 2008 / \n"
            "/\n"
            "WCONINJH\n"
            " 'I' 'WATER' 1* 100 250 / \n"
            "/\n"
            ;

    const auto& sched = make_schedule(input);
    const auto st = ::Ewoms::SummaryState{ std::chrono::system_clock::now() };
    UnitSystem unit_system(UnitSystem::UnitType::UNIT_TYPE_METRIC);

    // The BHP limit should not be effected by WCONHIST
    {
        const auto& c1 = sched.getWell("P",1).getProductionProperties().controls(st, 0);
        const auto& c2 = sched.getWell("P",2).getProductionProperties().controls(st, 0);
        BOOST_CHECK_EQUAL(c1.bhp_limit, 50 * 1e5); // 1
        BOOST_CHECK_EQUAL(c2.bhp_limit, 50 * 1e5); // 2
    }
    {
        const auto& c1 = sched.getWell("I",1).getInjectionProperties().controls(unit_system, st, 0);
        const auto& c2 = sched.getWell("I",2).getInjectionProperties().controls(unit_system, st, 0);
        BOOST_CHECK_EQUAL(c1.bhp_limit, 600 * 1e5); // 1
        BOOST_CHECK_EQUAL(c2.bhp_limit, 600 * 1e5); // 2
    }
    BOOST_CHECK_EQUAL(sched.getWell("I", 2).getInjectionProperties().hasInjectionControl(Ewoms::Well::InjectorCMode::BHP), true);

    // The well is producer for timestep 3 and the injection properties BHPTarget should be set to zero.
    BOOST_CHECK(sched.getWell("I", 3).isProducer());
    BOOST_CHECK_EQUAL(sched.getWell("I", 3).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::BHP), true );
    BOOST_CHECK_EQUAL(sched.getWell("I", 4).getInjectionProperties().hasInjectionControl(Ewoms::Well::InjectorCMode::BHP), true );
    {
        const auto& c3 = sched.getWell("I",3).getInjectionProperties().controls(unit_system, st, 0);
        const auto& c4 = sched.getWell("I",4).getInjectionProperties().controls(unit_system, st, 0);
        BOOST_CHECK_EQUAL(c3.bhp_limit, 0); // 1
        BOOST_CHECK_EQUAL(c4.bhp_limit, 6891.2 * 1e5); // 2
    }
}

BOOST_AUTO_TEST_CASE(changeModeWithWHISTCTL) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'P1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'P2'       'OP'   5   5 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'I'       'OP'   1   1 1*     'WATER' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'P1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'P2'  5  5   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P2'  5  5   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'I'  1  1   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 2\n"
            " 15  OKT 2008 / \n"
            "/\n"
            "WHISTCTL\n"
            " RESV / \n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 3\n"
            " 18  OKT 2008 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 4\n"
            " 20  OKT 2008 / \n"
            "/\n"
            "WHISTCTL\n"
            " LRAT / \n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 5\n"
            " 25  OKT 2008 / \n"
            "/\n"
            "WHISTCTL\n"
            " NONE / \n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            ;

    const auto& schedule = make_schedule(input);

    //Start
    BOOST_CHECK_THROW(schedule.getWell("P1", 0), std::invalid_argument);
    BOOST_CHECK_THROW(schedule.getWell("P2", 0), std::invalid_argument);

    //10  OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 1).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::ORAT);
    BOOST_CHECK(schedule.getWell("P2", 1).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::ORAT);

    //15  OKT 2008
    {
        const auto& props1 = schedule.getWell("P1", 2).getProductionProperties();
        const auto& props2 = schedule.getWell("P2", 2).getProductionProperties();

        BOOST_CHECK(props1.controlMode == Ewoms::Well::ProducerCMode::RESV);
        BOOST_CHECK(props2.controlMode == Ewoms::Well::ProducerCMode::RESV);
        // under history mode, a producing well should have only one rate target/limit or have no rate target/limit.
        // the rate target/limit from previous report step should not be kept.
        BOOST_CHECK( !props1.hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
        BOOST_CHECK( !props2.hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
    }

    //18  OKT 2008
    {
        const auto& props1 = schedule.getWell("P1", 3).getProductionProperties();
        const auto& props2 = schedule.getWell("P2", 3).getProductionProperties();

        BOOST_CHECK(props1.controlMode == Ewoms::Well::ProducerCMode::RESV);
        BOOST_CHECK(props2.controlMode == Ewoms::Well::ProducerCMode::RESV);

        BOOST_CHECK( !props1.hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
        BOOST_CHECK( !props2.hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
    }

    // 20 OKT 2008
    {
        const auto& props1 = schedule.getWell("P1", 4).getProductionProperties();
        const auto& props2 = schedule.getWell("P2", 4).getProductionProperties();

        BOOST_CHECK(props1.controlMode == Ewoms::Well::ProducerCMode::LRAT);
        BOOST_CHECK(props2.controlMode == Ewoms::Well::ProducerCMode::LRAT);

        BOOST_CHECK( !props1.hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
        BOOST_CHECK( !props2.hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
        BOOST_CHECK( !props1.hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );
        BOOST_CHECK( !props2.hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );
    }

    // 25 OKT 2008
    {
        const auto& props1 = schedule.getWell("P1", 5).getProductionProperties();
        const auto& props2 = schedule.getWell("P2", 5).getProductionProperties();

        BOOST_CHECK(props1.controlMode == Ewoms::Well::ProducerCMode::ORAT);
        BOOST_CHECK(props2.controlMode == Ewoms::Well::ProducerCMode::ORAT);

        BOOST_CHECK( !props1.hasProductionControl(Ewoms::Well::ProducerCMode::LRAT) );
        BOOST_CHECK( !props2.hasProductionControl(Ewoms::Well::ProducerCMode::LRAT) );
        BOOST_CHECK( !props1.hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );
        BOOST_CHECK( !props2.hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );
    }
}

BOOST_AUTO_TEST_CASE(fromWCONHISTtoWCONPROD) {
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'P1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'P2'       'OP'   5   5 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'I'       'OP'   1   1 1*     'WATER' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'P1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'P2'  5  5   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P2'  5  5   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'I'  1  1   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 2\n"
            " 15  OKT 2008 / \n"
            "/\n"
            "WCONPROD\n"
            " 'P1' 'OPEN' 'GRAT' 1*    200.0 300.0 / \n"
            " 'P2' 'OPEN' 'WRAT' 1*    100.0 300.0 / \n"
            "/\n"
            "DATES             -- 3\n"
            " 18  OKT 2008 / \n"
            "/\n"
            ;

    const auto& schedule = make_schedule(input);
    //Start
    BOOST_CHECK_THROW(schedule.getWell("P1", 0), std::invalid_argument);
    BOOST_CHECK_THROW(schedule.getWell("P2", 0), std::invalid_argument);

    //10  OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 1).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::ORAT);
    BOOST_CHECK(schedule.getWell("P2", 1).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::ORAT);

    //15  OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 2).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::GRAT);
    BOOST_CHECK(schedule.getWell("P1", 2).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::WRAT) );
    BOOST_CHECK(schedule.getWell("P2", 2).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::WRAT);
    BOOST_CHECK(schedule.getWell("P2", 2).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::GRAT) );
    // the previous control limits/targets should not stay
    BOOST_CHECK( !schedule.getWell("P1", 2).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK( !schedule.getWell("P2", 2).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
}

BOOST_AUTO_TEST_CASE(WHISTCTL_NEW_WELL) {
    Ewoms::Parser parser;
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "WHISTCTL\n"
            " GRAT/ \n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'P1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'P2'       'OP'   5   5 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'I'       'OP'   1   1 1*     'WATER' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'P1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'P2'  5  5   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P2'  5  5   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'I'  1  1   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 2\n"
            " 15  OKT 2008 / \n"
            "/\n"
            "WHISTCTL\n"
            " RESV / \n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 3\n"
            " 18  OKT 2008 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 4\n"
            " 20  OKT 2008 / \n"
            "/\n"
            "WHISTCTL\n"
            " LRAT / \n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 5\n"
            " 25  OKT 2008 / \n"
            "/\n"
            "WHISTCTL\n"
            " NONE / \n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            ;

    auto deck = parser.parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule(deck, grid , fp, runspec);

    //10  OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 1).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::GRAT);
    BOOST_CHECK(schedule.getWell("P2", 1).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::GRAT);

    //15  OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 2).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::RESV);
    BOOST_CHECK(schedule.getWell("P2", 2).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::RESV);
    // under history mode, a producing well should have only one rate target/limit or have no rate target/limit.
    // the rate target/limit from previous report step should not be kept.
    BOOST_CHECK( !schedule.getWell("P1", 2).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK( !schedule.getWell("P2", 2).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );

    //18  OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 3).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::RESV);
    BOOST_CHECK(schedule.getWell("P2", 3).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::RESV);
    BOOST_CHECK( !schedule.getWell("P1", 3).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK( !schedule.getWell("P2", 3).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );

    // 20 OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 4).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::LRAT);
    BOOST_CHECK(schedule.getWell("P2", 4).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::LRAT);
    BOOST_CHECK( !schedule.getWell("P1", 4).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK( !schedule.getWell("P2", 4).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::ORAT) );
    BOOST_CHECK( !schedule.getWell("P1", 4).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );
    BOOST_CHECK( !schedule.getWell("P2", 4).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );

    // 25 OKT 2008
    BOOST_CHECK(schedule.getWell("P1", 5).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::ORAT);
    BOOST_CHECK(schedule.getWell("P2", 5).getProductionProperties().controlMode == Ewoms::Well::ProducerCMode::ORAT);
    BOOST_CHECK( !schedule.getWell("P1", 5).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );
    BOOST_CHECK( !schedule.getWell("P2", 5).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::RESV) );
    BOOST_CHECK( !schedule.getWell("P1", 5).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::LRAT) );
    BOOST_CHECK( !schedule.getWell("P2", 5).getProductionProperties().hasProductionControl(Ewoms::Well::ProducerCMode::LRAT) );
}

BOOST_AUTO_TEST_CASE(unsupportedOptionWHISTCTL) {
    Ewoms::Parser parser;
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'P1'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'P2'       'OP'   5   5 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'I'       'OP'   1   1 1*     'WATER' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'P1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P1'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'P2'  5  5   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P2'  5  5   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'I'  1  1   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P1' 'OPEN' 'ORAT' 5*/ \n"
            " 'P2' 'OPEN' 'ORAT' 5*/ \n"
            "/\n"
            "DATES             -- 2\n"
            " 15  OKT 2008 / \n"
            "/\n"
            "WHISTCTL\n"
            " * YES / \n"
            ;

    auto deck = parser.parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    BOOST_CHECK_THROW(Schedule(deck, grid, fp, runspec), Ewoms::OpmInputError);
}

BOOST_AUTO_TEST_CASE(move_HEAD_I_location) {
    std::string input = R"(
            START             -- 0
            19 JUN 2007 /
            SCHEDULE
            DATES             -- 1
             10  OKT 2008 /
            /
            WELSPECS
                'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
                'W2' 'G2'  5 5 1       'OIL'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /
            DATES             -- 2
                15  OKT 2008 /
            /

            WELSPECS
                'W1' 'G1'  4 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /
    )";

    auto deck = Parser().parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);
    BOOST_CHECK_EQUAL(2, schedule.getWell("W1", 1).getHeadI());
    BOOST_CHECK_EQUAL(3, schedule.getWell("W1", 2).getHeadI());
}

BOOST_AUTO_TEST_CASE(change_ref_depth) {
    std::string input = R"(
            START             -- 0
            19 JUN 2007 /
            SCHEDULE
            DATES             -- 1
             10  OKT 2008 /
            /
            WELSPECS
                'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
                'W2' 'G2'  5 5 1       'OIL'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /
            DATES             -- 2
                15  OKT 2008 /
            /

            WELSPECS
                'W1' 'G1'  3 3 12.0 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /
    )";

    auto deck = Parser().parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);
    BOOST_CHECK_CLOSE(2873.94, schedule.getWell("W1", 1).getRefDepth(), 1e-5);
    BOOST_CHECK_EQUAL(12.0, schedule.getWell("W1", 2).getRefDepth());
}

BOOST_AUTO_TEST_CASE(WTEMP_well_template) {
    std::string input = R"(
            START             -- 0
            19 JUN 2007 /
            SCHEDULE
            DATES             -- 1
             10  OKT 2008 /
            /
            WELSPECS
                'W1' 'G1'  3 3 2873.94 'OIL' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
                'W2' 'G2'  5 5 1       'WATER'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
                'W3' 'G2'  6 6 1       'WATER'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /

            WCONINJE
            'W2' 'WATER' 'OPEN' 'RATE' 20000 4*  /
            'W3' 'WATER' 'OPEN' 'RATE' 20000 4*  /
            /

            DATES             -- 2
                15  OKT 2008 /
            /

            WTEMP
                'W*' 40.0 /
            /
    )";

    auto deck = Parser().parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);

    BOOST_CHECK_CLOSE(288.71, schedule.getWell("W1", 1).getInjectionProperties().temperature, 1e-5);
    BOOST_CHECK_CLOSE(288.71, schedule.getWell("W1", 2).getInjectionProperties().temperature, 1e-5);

    BOOST_CHECK_CLOSE(288.71, schedule.getWell("W2", 1).getInjectionProperties().temperature, 1e-5);
    BOOST_CHECK_CLOSE(313.15, schedule.getWell("W2", 2).getInjectionProperties().temperature, 1e-5);

    BOOST_CHECK_CLOSE(288.71, schedule.getWell("W3", 1).getInjectionProperties().temperature, 1e-5);
    BOOST_CHECK_CLOSE(313.15, schedule.getWell("W3", 2).getInjectionProperties().temperature, 1e-5);
}

BOOST_AUTO_TEST_CASE(WTEMPINJ_well_template) {
        std::string input = R"(
            START             -- 0
            19 JUN 2007 /
            SCHEDULE
            DATES             -- 1
             10  OKT 2008 /
            /
            WELSPECS
                'W1' 'G1'  3 3 2873.94 'OIL' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
                'W2' 'G2'  5 5 1       'WATER'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
                'W3' 'G2'  6 6 1       'WATER'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /

            WCONINJE
            'W2' 'WATER' 'OPEN' 'RATE' 20000 4*  /
            'W3' 'WATER' 'OPEN' 'RATE' 20000 4*  /
            /

            DATES             -- 2
                15  OKT 2008 /
            /

            WINJTEMP
                'W*' 1* 40.0 1* /
            /
    )";

        auto deck = Parser().parseString(input);
        EclipseGrid grid(10,10,10);
        TableManager table ( deck );
        FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
        Runspec runspec (deck);
        Schedule schedule( deck, grid, fp, runspec);

        // Producerwell - currently setting temperature only acts on injectors.
        BOOST_CHECK_CLOSE(288.71, schedule.getWell("W1", 1).getInjectionProperties().temperature, 1e-5);
        BOOST_CHECK_CLOSE(288.71, schedule.getWell("W1", 2).getInjectionProperties().temperature, 1e-5);

        BOOST_CHECK_CLOSE(288.71, schedule.getWell("W2", 1).getInjectionProperties().temperature, 1e-5);
        BOOST_CHECK_CLOSE(313.15, schedule.getWell("W2", 2).getInjectionProperties().temperature, 1e-5);

        BOOST_CHECK_CLOSE(288.71, schedule.getWell("W3", 1).getInjectionProperties().temperature, 1e-5);
        BOOST_CHECK_CLOSE(313.15, schedule.getWell("W3", 2).getInjectionProperties().temperature, 1e-5);
}

BOOST_AUTO_TEST_CASE( COMPDAT_sets_automatic_complnum ) {
    std::string input = R"(
        START             -- 0
        19 JUN 2007 /
        GRID
        PERMX
          1000*0.10/
        COPY
          PERMX PERMY /
          PERMX PERMZ /
        /
        SCHEDULE
        DATES             -- 1
            10  OKT 2008 /
        /
        WELSPECS
            'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
        /

        COMPDAT
            'W1' 0 0 1 1 'SHUT' 1*    / -- regular completion (1)
            'W1' 0 0 2 2 'SHUT' 1*    / -- regular completion (2)
            'W1' 0 0 3 4 'SHUT' 1*    / -- two completions in one record (3, 4)
        /

        DATES             -- 2
            11  OKT 2008 /
        /

        COMPDAT
            'W1' 0 0 1 1 'SHUT' 1*    / -- respecify, essentially ignore (1)
        /
    )";

    auto deck = Parser().parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);
    const auto& cs1 = schedule.getWell( "W1", 1 ).getConnections(  );
    BOOST_CHECK_EQUAL( 1, cs1.get( 0 ).complnum() );
    BOOST_CHECK_EQUAL( 2, cs1.get( 1 ).complnum() );
    BOOST_CHECK_EQUAL( 3, cs1.get( 2 ).complnum() );
    BOOST_CHECK_EQUAL( 4, cs1.get( 3 ).complnum() );

    const auto& cs2 = schedule.getWell( "W1", 2 ).getConnections(  );
    BOOST_CHECK_EQUAL( 1, cs2.get( 0 ).complnum() );
    BOOST_CHECK_EQUAL( 2, cs2.get( 1 ).complnum() );
    BOOST_CHECK_EQUAL( 3, cs2.get( 2 ).complnum() );
    BOOST_CHECK_EQUAL( 4, cs2.get( 3 ).complnum() );
}

BOOST_AUTO_TEST_CASE( COMPDAT_multiple_wells ) {
    std::string input = R"(
        START             -- 0
        19 JUN 2007 /
        GRID
        PERMX
          1000*0.10/
        COPY
          PERMX PERMY /
          PERMX PERMZ /
        /
        SCHEDULE
        DATES             -- 1
            10  OKT 2008 /
        /
        WELSPECS
            'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            'W2' 'G2'  5 5 1       'OIL'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
        /

        COMPDAT
            'W1' 0 0 1 1 'SHUT' 1*    / -- regular completion (1)
            'W1' 0 0 2 2 'SHUT' 1*    / -- regular completion (2)
            'W1' 0 0 3 4 'SHUT' 1*    / -- two completions in one record (3, 4)
            'W2' 0 0 3 3 'SHUT' 1*    / -- regular completion (1)
            'W2' 0 0 1 3 'SHUT' 1*    / -- two completions (one exist already) (2, 3)
            'W*' 0 0 3 5 'SHUT' 1*    / -- two completions, two wells (includes existing
                                        -- and adding for both wells)
        /
    )";

    auto deck = Parser().parseString( input);
    EclipseGrid grid( 10, 10, 10 );
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);

    {
        const auto& w1cs = schedule.getWell( "W1", 1 ).getConnections();
        BOOST_CHECK_EQUAL( 1, w1cs.get( 0 ).complnum() );
        BOOST_CHECK_EQUAL( 2, w1cs.get( 1 ).complnum() );
        BOOST_CHECK_EQUAL( 3, w1cs.get( 2 ).complnum() );
        BOOST_CHECK_EQUAL( 4, w1cs.get( 3 ).complnum() );
        BOOST_CHECK_EQUAL( 5, w1cs.get( 4 ).complnum() );

        const auto& w2cs = schedule.getWell( "W2", 1 ).getConnections();
        BOOST_CHECK_EQUAL( 1, w2cs.getFromIJK( 4, 4, 2 ).complnum() );
        BOOST_CHECK_EQUAL( 2, w2cs.getFromIJK( 4, 4, 0 ).complnum() );
        BOOST_CHECK_EQUAL( 3, w2cs.getFromIJK( 4, 4, 1 ).complnum() );
        BOOST_CHECK_EQUAL( 4, w2cs.getFromIJK( 4, 4, 3 ).complnum() );
        BOOST_CHECK_EQUAL( 5, w2cs.getFromIJK( 4, 4, 4 ).complnum() );
    }

    {
        const auto& w1cs = schedule.getWell( "W1", 1 ).getConnections();
        BOOST_CHECK_EQUAL( 1, w1cs.get( 0 ).complnum() );
        BOOST_CHECK_EQUAL( 2, w1cs.get( 1 ).complnum() );
        BOOST_CHECK_EQUAL( 3, w1cs.get( 2 ).complnum() );
        BOOST_CHECK_EQUAL( 4, w1cs.get( 3 ).complnum() );
        BOOST_CHECK_EQUAL( 5, w1cs.get( 4 ).complnum() );

        const auto& w2cs = schedule.getWell( "W2", 1 ).getConnections();
        BOOST_CHECK_EQUAL( 1, w2cs.getFromIJK( 4, 4, 2 ).complnum() );
        BOOST_CHECK_EQUAL( 2, w2cs.getFromIJK( 4, 4, 0 ).complnum() );
        BOOST_CHECK_EQUAL( 3, w2cs.getFromIJK( 4, 4, 1 ).complnum() );
        BOOST_CHECK_EQUAL( 4, w2cs.getFromIJK( 4, 4, 3 ).complnum() );
        BOOST_CHECK_EQUAL( 5, w2cs.getFromIJK( 4, 4, 4 ).complnum() );

        BOOST_CHECK_THROW( w2cs.get( 5 ).complnum(), std::out_of_range );
    }
}

BOOST_AUTO_TEST_CASE( COMPDAT_multiple_records_same_completion ) {
    std::string input = R"(
        START             -- 0
        19 JUN 2007 /
        GRID
        PERMX
          1000*0.10/
        COPY
          PERMX PERMY /
          PERMX PERMZ /
        /
        SCHEDULE
        DATES             -- 1
            10  OKT 2008 /
        /
        WELSPECS
            'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            'W2' 'G2'  5 5 1       'OIL'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
        /

        COMPDAT
            'W1' 0 0 1 2 'SHUT' 1*    / -- multiple completion (1, 2)
            'W1' 0 0 2 2 'SHUT' 1*    / -- updated completion (2)
            'W1' 0 0 3 3 'SHUT' 1*    / -- regular completion (3)
        /
    )";

    auto deck = Parser().parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);
    const auto& cs = schedule.getWell( "W1", 1 ).getConnections();
    BOOST_CHECK_EQUAL( 3U, cs.size() );
    BOOST_CHECK_EQUAL( 1, cs.get( 0 ).complnum() );
    BOOST_CHECK_EQUAL( 2, cs.get( 1 ).complnum() );
    BOOST_CHECK_EQUAL( 3, cs.get( 2 ).complnum() );
}

BOOST_AUTO_TEST_CASE( complump_less_than_1 ) {
    std::string input = R"(
            START             -- 0
            19 JUN 2007 /
            GRID
            PERMX
              1000*0.10/
            COPY
              PERMX PERMY /
              PERMX PERMZ /
            /
            SCHEDULE

            WELSPECS
                'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /

            COMPDAT
                'W1' 0 0 1 2 'SHUT' 1*    /
            /

            COMPLUMP
                'W1' 0 0 0 0 0 /
            /
    )";

    auto deck = Parser().parseString( input);
    EclipseGrid grid( 10, 10, 10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    BOOST_CHECK_THROW( Schedule( deck , grid, fp, runspec), std::exception );
}

BOOST_AUTO_TEST_CASE( complump ) {
    std::string input = R"(
            START             -- 0
            19 JUN 2007 /
            GRID
            PERMX
              1000*0.10/
            COPY
              PERMX PERMY /
              PERMX PERMZ /
            /
            SCHEDULE

            WELSPECS
                'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
                'W2' 'G2'  5 5 1       'OIL'   0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
            /

            COMPDAT
                'W1' 0 0 1 2 'SHUT' 1*    /    Global Index = 23, 123, 223, 323, 423, 523
                'W1' 0 0 2 3 'SHUT' 1*    /
                'W1' 0 0 4 6 'SHUT' 1*    /
                'W2' 0 0 3 4 'SHUT' 1*    /
                'W2' 0 0 1 4 'SHUT' 1*    /
            /

            COMPLUMP
                -- name I J K1 K2 C
                -- where C is the completion number of this lump
                'W1' 0 0 1 3 1 /
            /

            DATES             -- 1
             10  OKT 2008 /
            /

            WELOPEN
                'W1' 'OPEN' 0 0 0 1 1 /
            /
    )";

    constexpr auto open = Connection::State::OPEN;
    constexpr auto shut = Connection::State::SHUT;

    auto deck = Parser().parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);

    const auto& sc0 = schedule.getWell("W1", 0).getConnections();
    /* complnum should be modified by COMPLNUM */
    BOOST_CHECK_EQUAL( 1, sc0.getFromIJK( 2, 2, 0 ).complnum() );
    BOOST_CHECK_EQUAL( 1, sc0.getFromIJK( 2, 2, 1 ).complnum() );
    BOOST_CHECK_EQUAL( 1, sc0.getFromIJK( 2, 2, 2 ).complnum() );

    BOOST_CHECK( shut == sc0.getFromIJK( 2, 2, 0 ).state() );
    BOOST_CHECK( shut == sc0.getFromIJK( 2, 2, 1 ).state() );
    BOOST_CHECK( shut == sc0.getFromIJK( 2, 2, 2 ).state() );

    const auto& sc1  = schedule.getWell("W1", 1).getConnections();
    BOOST_CHECK( open == sc1.getFromIJK( 2, 2, 0 ).state() );
    BOOST_CHECK( open == sc1.getFromIJK( 2, 2, 1 ).state() );
    BOOST_CHECK( open == sc1.getFromIJK( 2, 2, 2 ).state() );
    BOOST_CHECK( shut == sc1.getFromIJK( 2, 2, 3 ).state() );

    const auto& completions = schedule.getWell("W1", 1).getCompletions();
    BOOST_CHECK_EQUAL(completions.size(), 4U);

    const auto& c1 = completions.at(1);
    BOOST_CHECK_EQUAL(c1.size(), 3U);

    for (const auto& pair : completions) {
        if (pair.first == 1)
            BOOST_CHECK(pair.second.size() > 1);
        else
            BOOST_CHECK_EQUAL(pair.second.size(), 1U);
    }

    const auto& w0 = schedule.getWell("W1", 0);
    BOOST_CHECK(w0.hasCompletion(1));
    BOOST_CHECK(!w0.hasCompletion(2));

    const auto& conn0 = w0.getConnections(100);
    BOOST_CHECK(conn0.empty());

    const auto& conn_all = w0.getConnections();
    const auto& conn1 = w0.getConnections(1);
    BOOST_CHECK_EQUAL( conn1.size(), 3);
    for (const auto& conn : conn_all) {
        if (conn.complnum() == 1) {
            auto conn_iter = std::find_if(conn1.begin(), conn1.end(), [&conn](const Connection * cptr)
                                                                      {
                                                                          return *cptr == conn;
                                                                      });
            BOOST_CHECK( conn_iter != conn1.end() );
        }
    }

    const auto& all_connections = w0.getConnections();
    auto global_index = grid.getGlobalIndex(2,2,0);
    BOOST_CHECK( all_connections.hasGlobalIndex(global_index));
    const auto& conn_g = all_connections.getFromGlobalIndex(global_index);
    const auto& conn_ijk = all_connections.getFromIJK(2,2,0);
    BOOST_CHECK(conn_g == conn_ijk);

    BOOST_CHECK_THROW( all_connections.getFromGlobalIndex(100000), std::exception );
}

BOOST_AUTO_TEST_CASE( COMPLUMP_specific_coordinates ) {
    std::string input = R"(
        START             -- 0
        19 JUN 2007 /
        GRID
        PERMX
          1000*0.10/
        COPY
          PERMX PERMY /
          PERMX PERMZ /
        /
        SCHEDULE

        WELSPECS
            'W1' 'G1'  3 3 2873.94 'WATER' 0.00 'STD' 'SHUT' 'NO' 0 'SEG' /
        /

        COMPDAT                         -- completion number
            'W1' 1 1 1 1 'SHUT' 1*    / -- 1
            'W1' 1 1 2 2 'SHUT' 1*    / -- 2
            'W1' 0 0 1 2 'SHUT' 1*    / -- 3, 4
            'W1' 0 0 2 3 'SHUT' 1*    / -- 5
            'W1' 2 2 1 1 'SHUT' 1*    / -- 6
            'W1' 2 2 4 6 'SHUT' 1*    / -- 7, 8, 9
        /

        DATES             -- 1
            10  OKT 2008 /
        /

        DATES             -- 2
            15  OKT 2008 /
        /

        COMPLUMP
            -- name I J K1 K2 C
            -- where C is the completion number of this lump
            'W1' 0 0 2 3 2 / -- all with k = [2 <= k <= 3] -> {2, 4, 5}
            'W1' 2 2 1 5 7 / -- fix'd i,j, k = [1 <= k <= 5] -> {6, 7, 8}
        /

        WELOPEN
            'W1' OPEN 0 0 0 2 2 / -- open the new 2 {2, 4, 5}
            'W1' OPEN 0 0 0 5 7 / -- open 5..7 {5, 6, 7, 8}
        /
    )";

    constexpr auto open = Connection::State::OPEN;
    constexpr auto shut = Connection::State::SHUT;

    auto deck = Parser().parseString( input);
    EclipseGrid grid( 10, 10, 10 );
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);

    const auto& cs1 = schedule.getWell("W1", 1).getConnections();
    const auto& cs2 = schedule.getWell("W1", 2).getConnections();
    BOOST_CHECK_EQUAL( 9U, cs1.size() );
    BOOST_CHECK( shut == cs1.getFromIJK( 0, 0, 1 ).state() );
    BOOST_CHECK( shut == cs1.getFromIJK( 2, 2, 0 ).state() );
    BOOST_CHECK( shut == cs1.getFromIJK( 2, 2, 1 ).state() );
    BOOST_CHECK( shut == cs1.getFromIJK( 2, 2, 2 ).state() );
    BOOST_CHECK( shut == cs1.getFromIJK( 1, 1, 0 ).state() );
    BOOST_CHECK( shut == cs1.getFromIJK( 1, 1, 3 ).state() );
    BOOST_CHECK( shut == cs1.getFromIJK( 1, 1, 4 ).state() );
    BOOST_CHECK( shut == cs1.getFromIJK( 1, 1, 5 ).state() );

    BOOST_CHECK( open == cs2.getFromIJK( 0, 0, 1 ).state() );
    BOOST_CHECK( shut == cs2.getFromIJK( 2, 2, 0 ).state() );
    BOOST_CHECK( open == cs2.getFromIJK( 2, 2, 1 ).state() );
    BOOST_CHECK( open == cs2.getFromIJK( 2, 2, 2 ).state() );
    BOOST_CHECK( open == cs2.getFromIJK( 1, 1, 0 ).state() );
    BOOST_CHECK( open == cs2.getFromIJK( 1, 1, 3 ).state() );
    BOOST_CHECK( open == cs2.getFromIJK( 1, 1, 4 ).state() );
    BOOST_CHECK( shut == cs2.getFromIJK( 1, 1, 5 ).state() );
}

BOOST_AUTO_TEST_CASE(TestCompletionStateEnum2String) {
    BOOST_CHECK( "AUTO" == Connection::State2String(Connection::State::AUTO));
    BOOST_CHECK( "OPEN" == Connection::State2String(Connection::State::OPEN));
    BOOST_CHECK( "SHUT" == Connection::State2String(Connection::State::SHUT));
}

BOOST_AUTO_TEST_CASE(TestCompletionStateEnumFromString) {
    BOOST_CHECK_THROW( Connection::StateFromString("XXX") , std::invalid_argument );
    BOOST_CHECK( Connection::State::AUTO == Connection::StateFromString("AUTO"));
    BOOST_CHECK( Connection::State::SHUT == Connection::StateFromString("SHUT"));
    BOOST_CHECK( Connection::State::SHUT == Connection::StateFromString("STOP"));
    BOOST_CHECK( Connection::State::OPEN == Connection::StateFromString("OPEN"));
}

BOOST_AUTO_TEST_CASE(TestCompletionStateEnumLoop) {
    BOOST_CHECK( Connection::State::AUTO == Connection::StateFromString( Connection::State2String( Connection::State::AUTO ) ));
    BOOST_CHECK( Connection::State::SHUT == Connection::StateFromString( Connection::State2String( Connection::State::SHUT ) ));
    BOOST_CHECK( Connection::State::OPEN == Connection::StateFromString( Connection::State2String( Connection::State::OPEN ) ));

    BOOST_CHECK( "AUTO" == Connection::State2String(Connection::StateFromString(  "AUTO" ) ));
    BOOST_CHECK( "OPEN" == Connection::State2String(Connection::StateFromString(  "OPEN" ) ));
    BOOST_CHECK( "SHUT" == Connection::State2String(Connection::StateFromString(  "SHUT" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestCompletionDirectionEnum2String)
{
    BOOST_CHECK("X" == Connection::Direction2String(Connection::Direction::X));
    BOOST_CHECK("Y" == Connection::Direction2String(Connection::Direction::Y));
    BOOST_CHECK("Z" == Connection::Direction2String(Connection::Direction::Z));
}

BOOST_AUTO_TEST_CASE(TestCompletionDirectionEnumFromString)
{
    BOOST_CHECK_THROW(Connection::DirectionFromString("XXX"), std::invalid_argument);

    BOOST_CHECK(Connection::Direction::X == Connection::DirectionFromString("X"));
    BOOST_CHECK(Connection::Direction::Y == Connection::DirectionFromString("Y"));
    BOOST_CHECK(Connection::Direction::Z == Connection::DirectionFromString("Z"));
}

BOOST_AUTO_TEST_CASE(TestCompletionConnectionDirectionLoop)
{
    BOOST_CHECK(Connection::Direction::X == Connection::DirectionFromString(Connection::Direction2String(Connection::Direction::X)));
    BOOST_CHECK(Connection::Direction::Y == Connection::DirectionFromString(Connection::Direction2String(Connection::Direction::Y)));
    BOOST_CHECK(Connection::Direction::Z == Connection::DirectionFromString(Connection::Direction2String(Connection::Direction::Z)));

    BOOST_CHECK("X" == Connection::Direction2String(Connection::DirectionFromString("X")));
    BOOST_CHECK("Y" == Connection::Direction2String(Connection::DirectionFromString("Y")));
    BOOST_CHECK("Z" == Connection::Direction2String(Connection::DirectionFromString("Z")));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestGroupInjectionControlEnum2String) {
    BOOST_CHECK_EQUAL( "NONE" , Group::InjectionCMode2String(Group::InjectionCMode::NONE));
    BOOST_CHECK_EQUAL( "RATE" , Group::InjectionCMode2String(Group::InjectionCMode::RATE));
    BOOST_CHECK_EQUAL( "RESV" , Group::InjectionCMode2String(Group::InjectionCMode::RESV));
    BOOST_CHECK_EQUAL( "REIN" , Group::InjectionCMode2String(Group::InjectionCMode::REIN));
    BOOST_CHECK_EQUAL( "VREP" , Group::InjectionCMode2String(Group::InjectionCMode::VREP));
    BOOST_CHECK_EQUAL( "FLD"  , Group::InjectionCMode2String(Group::InjectionCMode::FLD));
}

BOOST_AUTO_TEST_CASE(TestGroupInjectionControlEnumFromString) {
    BOOST_CHECK_THROW( Group::InjectionCModeFromString("XXX") , std::invalid_argument );
    BOOST_CHECK( Group::InjectionCMode::NONE == Group::InjectionCModeFromString("NONE"));
    BOOST_CHECK( Group::InjectionCMode::RATE == Group::InjectionCModeFromString("RATE"));
    BOOST_CHECK( Group::InjectionCMode::RESV == Group::InjectionCModeFromString("RESV"));
    BOOST_CHECK( Group::InjectionCMode::REIN == Group::InjectionCModeFromString("REIN"));
    BOOST_CHECK( Group::InjectionCMode::VREP == Group::InjectionCModeFromString("VREP"));
    BOOST_CHECK( Group::InjectionCMode::FLD  == Group::InjectionCModeFromString("FLD"));
}

BOOST_AUTO_TEST_CASE(TestGroupInjectionControlEnumLoop) {
    BOOST_CHECK( Group::InjectionCMode::NONE == Group::InjectionCModeFromString( Group::InjectionCMode2String( Group::InjectionCMode::NONE ) ));
    BOOST_CHECK( Group::InjectionCMode::RATE == Group::InjectionCModeFromString( Group::InjectionCMode2String( Group::InjectionCMode::RATE ) ));
    BOOST_CHECK( Group::InjectionCMode::RESV == Group::InjectionCModeFromString( Group::InjectionCMode2String( Group::InjectionCMode::RESV ) ));
    BOOST_CHECK( Group::InjectionCMode::REIN == Group::InjectionCModeFromString( Group::InjectionCMode2String( Group::InjectionCMode::REIN ) ));
    BOOST_CHECK( Group::InjectionCMode::VREP == Group::InjectionCModeFromString( Group::InjectionCMode2String( Group::InjectionCMode::VREP ) ));
    BOOST_CHECK( Group::InjectionCMode::FLD  == Group::InjectionCModeFromString( Group::InjectionCMode2String( Group::InjectionCMode::FLD ) ));

    BOOST_CHECK_EQUAL( "NONE" , Group::InjectionCMode2String(Group::InjectionCModeFromString( "NONE" ) ));
    BOOST_CHECK_EQUAL( "RATE" , Group::InjectionCMode2String(Group::InjectionCModeFromString( "RATE" ) ));
    BOOST_CHECK_EQUAL( "RESV" , Group::InjectionCMode2String(Group::InjectionCModeFromString( "RESV" ) ));
    BOOST_CHECK_EQUAL( "REIN" , Group::InjectionCMode2String(Group::InjectionCModeFromString( "REIN" ) ));
    BOOST_CHECK_EQUAL( "VREP" , Group::InjectionCMode2String(Group::InjectionCModeFromString( "VREP" ) ));
    BOOST_CHECK_EQUAL( "FLD"  , Group::InjectionCMode2String(Group::InjectionCModeFromString( "FLD"  ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestGroupProductionControlEnum2String) {
    BOOST_CHECK_EQUAL( "NONE" , Group::ProductionCMode2String(Group::ProductionCMode::NONE));
    BOOST_CHECK_EQUAL( "ORAT" , Group::ProductionCMode2String(Group::ProductionCMode::ORAT));
    BOOST_CHECK_EQUAL( "WRAT" , Group::ProductionCMode2String(Group::ProductionCMode::WRAT));
    BOOST_CHECK_EQUAL( "GRAT" , Group::ProductionCMode2String(Group::ProductionCMode::GRAT));
    BOOST_CHECK_EQUAL( "LRAT" , Group::ProductionCMode2String(Group::ProductionCMode::LRAT));
    BOOST_CHECK_EQUAL( "CRAT" , Group::ProductionCMode2String(Group::ProductionCMode::CRAT));
    BOOST_CHECK_EQUAL( "RESV" , Group::ProductionCMode2String(Group::ProductionCMode::RESV));
    BOOST_CHECK_EQUAL( "PRBL" , Group::ProductionCMode2String(Group::ProductionCMode::PRBL));
}

BOOST_AUTO_TEST_CASE(TestGroupProductionControlEnumFromString) {
    BOOST_CHECK_THROW(Group::ProductionCModeFromString("XXX") , std::invalid_argument );
    BOOST_CHECK(Group::ProductionCMode::NONE  == Group::ProductionCModeFromString("NONE"));
    BOOST_CHECK(Group::ProductionCMode::ORAT  == Group::ProductionCModeFromString("ORAT"));
    BOOST_CHECK(Group::ProductionCMode::WRAT  == Group::ProductionCModeFromString("WRAT"));
    BOOST_CHECK(Group::ProductionCMode::GRAT  == Group::ProductionCModeFromString("GRAT"));
    BOOST_CHECK(Group::ProductionCMode::LRAT  == Group::ProductionCModeFromString("LRAT"));
    BOOST_CHECK(Group::ProductionCMode::CRAT  == Group::ProductionCModeFromString("CRAT"));
    BOOST_CHECK(Group::ProductionCMode::RESV  == Group::ProductionCModeFromString("RESV"));
    BOOST_CHECK(Group::ProductionCMode::PRBL  == Group::ProductionCModeFromString("PRBL"));
}

BOOST_AUTO_TEST_CASE(TestGroupProductionControlEnumLoop) {
    BOOST_CHECK( Group::ProductionCMode::NONE == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::NONE ) ));
    BOOST_CHECK( Group::ProductionCMode::ORAT == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::ORAT ) ));
    BOOST_CHECK( Group::ProductionCMode::WRAT == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::WRAT ) ));
    BOOST_CHECK( Group::ProductionCMode::GRAT == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::GRAT ) ));
    BOOST_CHECK( Group::ProductionCMode::LRAT == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::LRAT ) ));
    BOOST_CHECK( Group::ProductionCMode::CRAT == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::CRAT ) ));
    BOOST_CHECK( Group::ProductionCMode::RESV == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::RESV ) ));
    BOOST_CHECK( Group::ProductionCMode::PRBL == Group::ProductionCModeFromString( Group::ProductionCMode2String( Group::ProductionCMode::PRBL ) ));

    BOOST_CHECK_EQUAL( "NONE" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "NONE" ) ));
    BOOST_CHECK_EQUAL( "ORAT" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "ORAT" ) ));
    BOOST_CHECK_EQUAL( "WRAT" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "WRAT" ) ));
    BOOST_CHECK_EQUAL( "GRAT" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "GRAT" ) ));
    BOOST_CHECK_EQUAL( "LRAT" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "LRAT" ) ));
    BOOST_CHECK_EQUAL( "CRAT" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "CRAT" ) ));
    BOOST_CHECK_EQUAL( "RESV" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "RESV" ) ));
    BOOST_CHECK_EQUAL( "PRBL" , Group::ProductionCMode2String(Group::ProductionCModeFromString( "PRBL" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestGroupProductionExceedLimitControlEnum2String) {
    BOOST_CHECK_EQUAL( "NONE"     , Group::ExceedAction2String(Group::ExceedAction::NONE));
    BOOST_CHECK_EQUAL( "CON"      , Group::ExceedAction2String(Group::ExceedAction::CON));
    BOOST_CHECK_EQUAL( "+CON"     , Group::ExceedAction2String(Group::ExceedAction::CON_PLUS));
    BOOST_CHECK_EQUAL( "WELL"     , Group::ExceedAction2String(Group::ExceedAction::WELL));
    BOOST_CHECK_EQUAL( "PLUG"     , Group::ExceedAction2String(Group::ExceedAction::PLUG));
    BOOST_CHECK_EQUAL( "RATE"     , Group::ExceedAction2String(Group::ExceedAction::RATE));
}

BOOST_AUTO_TEST_CASE(TestGroupProductionExceedLimitActionEnumFromString) {
    BOOST_CHECK_THROW( Group::ExceedActionFromString("XXX") , std::invalid_argument );

    BOOST_CHECK(Group::ExceedAction::NONE     == Group::ExceedActionFromString("NONE"));
    BOOST_CHECK(Group::ExceedAction::CON      == Group::ExceedActionFromString("CON" ));
    BOOST_CHECK(Group::ExceedAction::CON_PLUS == Group::ExceedActionFromString("+CON"));
    BOOST_CHECK(Group::ExceedAction::WELL     == Group::ExceedActionFromString("WELL"));
    BOOST_CHECK(Group::ExceedAction::PLUG     == Group::ExceedActionFromString("PLUG"));
    BOOST_CHECK(Group::ExceedAction::RATE     == Group::ExceedActionFromString("RATE"));
}

BOOST_AUTO_TEST_CASE(TestGroupProductionExceedLimitActionEnumLoop) {
    BOOST_CHECK( Group::ExceedAction::NONE     == Group::ExceedActionFromString( Group::ExceedAction2String( Group::ExceedAction::NONE     ) ));
    BOOST_CHECK( Group::ExceedAction::CON      == Group::ExceedActionFromString( Group::ExceedAction2String( Group::ExceedAction::CON      ) ));
    BOOST_CHECK( Group::ExceedAction::CON_PLUS == Group::ExceedActionFromString( Group::ExceedAction2String( Group::ExceedAction::CON_PLUS ) ));
    BOOST_CHECK( Group::ExceedAction::WELL     == Group::ExceedActionFromString( Group::ExceedAction2String( Group::ExceedAction::WELL     ) ));
    BOOST_CHECK( Group::ExceedAction::PLUG     == Group::ExceedActionFromString( Group::ExceedAction2String( Group::ExceedAction::PLUG     ) ));
    BOOST_CHECK( Group::ExceedAction::RATE     == Group::ExceedActionFromString( Group::ExceedAction2String( Group::ExceedAction::RATE     ) ));

    BOOST_CHECK_EQUAL("NONE" , Group::ExceedAction2String(Group::ExceedActionFromString( "NONE" ) ));
    BOOST_CHECK_EQUAL("CON"  , Group::ExceedAction2String(Group::ExceedActionFromString( "CON"  ) ));
    BOOST_CHECK_EQUAL("+CON" , Group::ExceedAction2String(Group::ExceedActionFromString( "+CON" ) ));
    BOOST_CHECK_EQUAL("WELL" , Group::ExceedAction2String(Group::ExceedActionFromString( "WELL" ) ));
    BOOST_CHECK_EQUAL("PLUG" , Group::ExceedAction2String(Group::ExceedActionFromString( "PLUG" ) ));
    BOOST_CHECK_EQUAL("RATE" , Group::ExceedAction2String(Group::ExceedActionFromString( "RATE" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestInjectorEnum2String) {
    BOOST_CHECK_EQUAL( "OIL"  ,  InjectorType2String(InjectorType::OIL));
    BOOST_CHECK_EQUAL( "GAS"  ,  InjectorType2String(InjectorType::GAS));
    BOOST_CHECK_EQUAL( "WATER" , InjectorType2String(InjectorType::WATER));
    BOOST_CHECK_EQUAL( "MULTI" , InjectorType2String(InjectorType::MULTI));
}

BOOST_AUTO_TEST_CASE(TestInjectorEnumFromString) {
    BOOST_CHECK_THROW( InjectorTypeFromString("XXX") , std::invalid_argument );
    BOOST_CHECK( InjectorType::OIL   == InjectorTypeFromString("OIL"));
    BOOST_CHECK( InjectorType::WATER == InjectorTypeFromString("WATER"));
    BOOST_CHECK( InjectorType::WATER == InjectorTypeFromString("WAT"));
    BOOST_CHECK( InjectorType::GAS   == InjectorTypeFromString("GAS"));
    BOOST_CHECK( InjectorType::MULTI == InjectorTypeFromString("MULTI"));
}

BOOST_AUTO_TEST_CASE(TestInjectorEnumLoop) {
    BOOST_CHECK( InjectorType::OIL   == InjectorTypeFromString( InjectorType2String( InjectorType::OIL ) ));
    BOOST_CHECK( InjectorType::WATER == InjectorTypeFromString( InjectorType2String( InjectorType::WATER ) ));
    BOOST_CHECK( InjectorType::GAS   == InjectorTypeFromString( InjectorType2String( InjectorType::GAS ) ));
    BOOST_CHECK( InjectorType::MULTI == InjectorTypeFromString( InjectorType2String( InjectorType::MULTI ) ));

    BOOST_CHECK_EQUAL( "MULTI"    , InjectorType2String(InjectorTypeFromString(  "MULTI" ) ));
    BOOST_CHECK_EQUAL( "OIL"      , InjectorType2String(InjectorTypeFromString(  "OIL" ) ));
    BOOST_CHECK_EQUAL( "GAS"      , InjectorType2String(InjectorTypeFromString(  "GAS" ) ));
    BOOST_CHECK_EQUAL( "WATER"    , InjectorType2String(InjectorTypeFromString(  "WATER" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(InjectorCOntrolMopdeEnum2String) {
    BOOST_CHECK_EQUAL( "RATE"  , Well::InjectorCMode2String(Well::InjectorCMode::RATE));
    BOOST_CHECK_EQUAL( "RESV"  , Well::InjectorCMode2String(Well::InjectorCMode::RESV));
    BOOST_CHECK_EQUAL( "BHP"   , Well::InjectorCMode2String(Well::InjectorCMode::BHP));
    BOOST_CHECK_EQUAL( "THP"   , Well::InjectorCMode2String(Well::InjectorCMode::THP));
    BOOST_CHECK_EQUAL( "GRUP"  , Well::InjectorCMode2String(Well::InjectorCMode::GRUP));
}

BOOST_AUTO_TEST_CASE(InjectorControlModeEnumFromString) {
    BOOST_CHECK_THROW( Well::InjectorCModeFromString("XXX") , std::invalid_argument );
    BOOST_CHECK( Well::InjectorCMode::RATE == Well::InjectorCModeFromString("RATE"));
    BOOST_CHECK( Well::InjectorCMode::BHP  == Well::InjectorCModeFromString("BHP"));
    BOOST_CHECK( Well::InjectorCMode::RESV == Well::InjectorCModeFromString("RESV"));
    BOOST_CHECK( Well::InjectorCMode::THP  == Well::InjectorCModeFromString("THP"));
    BOOST_CHECK( Well::InjectorCMode::GRUP == Well::InjectorCModeFromString("GRUP"));
}

BOOST_AUTO_TEST_CASE(InjectorControlModeEnumLoop) {
    BOOST_CHECK( Well::InjectorCMode::RATE == Well::InjectorCModeFromString( Well::InjectorCMode2String( Well::InjectorCMode::RATE ) ));
    BOOST_CHECK( Well::InjectorCMode::BHP  == Well::InjectorCModeFromString( Well::InjectorCMode2String( Well::InjectorCMode::BHP ) ));
    BOOST_CHECK( Well::InjectorCMode::RESV == Well::InjectorCModeFromString( Well::InjectorCMode2String( Well::InjectorCMode::RESV ) ));
    BOOST_CHECK( Well::InjectorCMode::THP  == Well::InjectorCModeFromString( Well::InjectorCMode2String( Well::InjectorCMode::THP ) ));
    BOOST_CHECK( Well::InjectorCMode::GRUP == Well::InjectorCModeFromString( Well::InjectorCMode2String( Well::InjectorCMode::GRUP ) ));

    BOOST_CHECK_EQUAL( "THP"  , Well::InjectorCMode2String(Well::InjectorCModeFromString(  "THP" ) ));
    BOOST_CHECK_EQUAL( "RATE" , Well::InjectorCMode2String(Well::InjectorCModeFromString(  "RATE" ) ));
    BOOST_CHECK_EQUAL( "RESV" , Well::InjectorCMode2String(Well::InjectorCModeFromString(  "RESV" ) ));
    BOOST_CHECK_EQUAL( "BHP"  , Well::InjectorCMode2String(Well::InjectorCModeFromString(  "BHP" ) ));
    BOOST_CHECK_EQUAL( "GRUP" , Well::InjectorCMode2String(Well::InjectorCModeFromString(  "GRUP" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(InjectorStatusEnum2String) {
    BOOST_CHECK_EQUAL( "OPEN",  Well::Status2String(Well::Status::OPEN));
    BOOST_CHECK_EQUAL( "SHUT",  Well::Status2String(Well::Status::SHUT));
    BOOST_CHECK_EQUAL( "AUTO",  Well::Status2String(Well::Status::AUTO));
    BOOST_CHECK_EQUAL( "STOP",  Well::Status2String(Well::Status::STOP));
}

BOOST_AUTO_TEST_CASE(InjectorStatusEnumFromString) {
    BOOST_CHECK_THROW( Well::StatusFromString("XXX") , std::invalid_argument );
    BOOST_CHECK( Well::Status::OPEN == Well::StatusFromString("OPEN"));
    BOOST_CHECK( Well::Status::AUTO == Well::StatusFromString("AUTO"));
    BOOST_CHECK( Well::Status::SHUT == Well::StatusFromString("SHUT"));
    BOOST_CHECK( Well::Status::STOP == Well::StatusFromString("STOP"));
}

BOOST_AUTO_TEST_CASE(InjectorStatusEnumLoop) {
    BOOST_CHECK( Well::Status::OPEN == Well::StatusFromString( Well::Status2String( Well::Status::OPEN ) ));
    BOOST_CHECK( Well::Status::AUTO == Well::StatusFromString( Well::Status2String( Well::Status::AUTO ) ));
    BOOST_CHECK( Well::Status::SHUT == Well::StatusFromString( Well::Status2String( Well::Status::SHUT ) ));
    BOOST_CHECK( Well::Status::STOP == Well::StatusFromString( Well::Status2String( Well::Status::STOP ) ));

    BOOST_CHECK_EQUAL( "STOP", Well::Status2String(Well::StatusFromString(  "STOP" ) ));
    BOOST_CHECK_EQUAL( "OPEN", Well::Status2String(Well::StatusFromString(  "OPEN" ) ));
    BOOST_CHECK_EQUAL( "SHUT", Well::Status2String(Well::StatusFromString(  "SHUT" ) ));
    BOOST_CHECK_EQUAL( "AUTO", Well::Status2String(Well::StatusFromString(  "AUTO" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(ProducerCOntrolMopdeEnum2String) {
    BOOST_CHECK_EQUAL( "ORAT"  ,  Well::ProducerCMode2String(Well::ProducerCMode::ORAT));
    BOOST_CHECK_EQUAL( "WRAT"  ,  Well::ProducerCMode2String(Well::ProducerCMode::WRAT));
    BOOST_CHECK_EQUAL( "GRAT"  ,  Well::ProducerCMode2String(Well::ProducerCMode::GRAT));
    BOOST_CHECK_EQUAL( "LRAT"  ,  Well::ProducerCMode2String(Well::ProducerCMode::LRAT));
    BOOST_CHECK_EQUAL( "CRAT"  ,  Well::ProducerCMode2String(Well::ProducerCMode::CRAT));
    BOOST_CHECK_EQUAL( "RESV"  ,  Well::ProducerCMode2String(Well::ProducerCMode::RESV));
    BOOST_CHECK_EQUAL( "BHP"   ,  Well::ProducerCMode2String(Well::ProducerCMode::BHP));
    BOOST_CHECK_EQUAL( "THP"   ,  Well::ProducerCMode2String(Well::ProducerCMode::THP));
    BOOST_CHECK_EQUAL( "GRUP"  ,  Well::ProducerCMode2String(Well::ProducerCMode::GRUP));
}

BOOST_AUTO_TEST_CASE(ProducerControlModeEnumFromString) {
    BOOST_CHECK_THROW( Well::ProducerCModeFromString("XRAT") , std::invalid_argument );
    BOOST_CHECK( Well::ProducerCMode::ORAT   == Well::ProducerCModeFromString("ORAT"));
    BOOST_CHECK( Well::ProducerCMode::WRAT   == Well::ProducerCModeFromString("WRAT"));
    BOOST_CHECK( Well::ProducerCMode::GRAT   == Well::ProducerCModeFromString("GRAT"));
    BOOST_CHECK( Well::ProducerCMode::LRAT   == Well::ProducerCModeFromString("LRAT"));
    BOOST_CHECK( Well::ProducerCMode::CRAT   == Well::ProducerCModeFromString("CRAT"));
    BOOST_CHECK( Well::ProducerCMode::RESV   == Well::ProducerCModeFromString("RESV"));
    BOOST_CHECK( Well::ProducerCMode::BHP    == Well::ProducerCModeFromString("BHP" ));
    BOOST_CHECK( Well::ProducerCMode::THP    == Well::ProducerCModeFromString("THP" ));
    BOOST_CHECK( Well::ProducerCMode::GRUP   == Well::ProducerCModeFromString("GRUP"));
}

BOOST_AUTO_TEST_CASE(ProducerControlModeEnumLoop) {
    BOOST_CHECK( Well::ProducerCMode::ORAT == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::ORAT ) ));
    BOOST_CHECK( Well::ProducerCMode::WRAT == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::WRAT ) ));
    BOOST_CHECK( Well::ProducerCMode::GRAT == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::GRAT ) ));
    BOOST_CHECK( Well::ProducerCMode::LRAT == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::LRAT ) ));
    BOOST_CHECK( Well::ProducerCMode::CRAT == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::CRAT ) ));
    BOOST_CHECK( Well::ProducerCMode::RESV == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::RESV ) ));
    BOOST_CHECK( Well::ProducerCMode::BHP  == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::BHP  ) ));
    BOOST_CHECK( Well::ProducerCMode::THP  == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::THP  ) ));
    BOOST_CHECK( Well::ProducerCMode::GRUP == Well::ProducerCModeFromString( Well::ProducerCMode2String( Well::ProducerCMode::GRUP ) ));

    BOOST_CHECK_EQUAL( "ORAT"      , Well::ProducerCMode2String(Well::ProducerCModeFromString( "ORAT"  ) ));
    BOOST_CHECK_EQUAL( "WRAT"      , Well::ProducerCMode2String(Well::ProducerCModeFromString( "WRAT"  ) ));
    BOOST_CHECK_EQUAL( "GRAT"      , Well::ProducerCMode2String(Well::ProducerCModeFromString( "GRAT"  ) ));
    BOOST_CHECK_EQUAL( "LRAT"      , Well::ProducerCMode2String(Well::ProducerCModeFromString( "LRAT"  ) ));
    BOOST_CHECK_EQUAL( "CRAT"      , Well::ProducerCMode2String(Well::ProducerCModeFromString( "CRAT"  ) ));
    BOOST_CHECK_EQUAL( "RESV"      , Well::ProducerCMode2String(Well::ProducerCModeFromString( "RESV"  ) ));
    BOOST_CHECK_EQUAL( "BHP"       , Well::ProducerCMode2String(Well::ProducerCModeFromString( "BHP"   ) ));
    BOOST_CHECK_EQUAL( "THP"       , Well::ProducerCMode2String(Well::ProducerCModeFromString( "THP"   ) ));
    BOOST_CHECK_EQUAL( "GRUP"      , Well::ProducerCMode2String(Well::ProducerCModeFromString( "GRUP"  ) ));
}

/*******************************************************************/
/*****************************************************************/

BOOST_AUTO_TEST_CASE(GuideRatePhaseEnum2String) {
    BOOST_CHECK_EQUAL( "OIL"  ,        Well::GuideRateTarget2String(Well::GuideRateTarget::OIL));
    BOOST_CHECK_EQUAL( "WAT"  ,        Well::GuideRateTarget2String(Well::GuideRateTarget::WAT));
    BOOST_CHECK_EQUAL( "GAS"  ,        Well::GuideRateTarget2String(Well::GuideRateTarget::GAS));
    BOOST_CHECK_EQUAL( "LIQ"  ,        Well::GuideRateTarget2String(Well::GuideRateTarget::LIQ));
    BOOST_CHECK_EQUAL( "COMB" ,        Well::GuideRateTarget2String(Well::GuideRateTarget::COMB));
    BOOST_CHECK_EQUAL( "WGA"  ,        Well::GuideRateTarget2String(Well::GuideRateTarget::WGA));
    BOOST_CHECK_EQUAL( "CVAL" ,        Well::GuideRateTarget2String(Well::GuideRateTarget::CVAL));
    BOOST_CHECK_EQUAL( "RAT"  ,        Well::GuideRateTarget2String(Well::GuideRateTarget::RAT));
    BOOST_CHECK_EQUAL( "RES"  ,        Well::GuideRateTarget2String(Well::GuideRateTarget::RES));
    BOOST_CHECK_EQUAL( "UNDEFINED"  ,  Well::GuideRateTarget2String(Well::GuideRateTarget::UNDEFINED));
}

BOOST_AUTO_TEST_CASE(GuideRatePhaseEnumFromString) {
    BOOST_CHECK_THROW( Well::GuideRateTargetFromString("XRAT") , std::invalid_argument );
    BOOST_CHECK( Well::GuideRateTarget::OIL       == Well::GuideRateTargetFromString("OIL"));
    BOOST_CHECK( Well::GuideRateTarget::WAT       == Well::GuideRateTargetFromString("WAT"));
    BOOST_CHECK( Well::GuideRateTarget::GAS       == Well::GuideRateTargetFromString("GAS"));
    BOOST_CHECK( Well::GuideRateTarget::LIQ       == Well::GuideRateTargetFromString("LIQ"));
    BOOST_CHECK( Well::GuideRateTarget::COMB      == Well::GuideRateTargetFromString("COMB"));
    BOOST_CHECK( Well::GuideRateTarget::WGA       == Well::GuideRateTargetFromString("WGA"));
    BOOST_CHECK( Well::GuideRateTarget::CVAL      == Well::GuideRateTargetFromString("CVAL"));
    BOOST_CHECK( Well::GuideRateTarget::RAT       == Well::GuideRateTargetFromString("RAT"));
    BOOST_CHECK( Well::GuideRateTarget::RES       == Well::GuideRateTargetFromString("RES"));
    BOOST_CHECK( Well::GuideRateTarget::UNDEFINED == Well::GuideRateTargetFromString("UNDEFINED"));
}

BOOST_AUTO_TEST_CASE(GuideRatePhaseEnum2Loop) {
    BOOST_CHECK( Well::GuideRateTarget::OIL        == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::OIL ) ));
    BOOST_CHECK( Well::GuideRateTarget::WAT        == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::WAT ) ));
    BOOST_CHECK( Well::GuideRateTarget::GAS        == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::GAS ) ));
    BOOST_CHECK( Well::GuideRateTarget::LIQ        == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::LIQ ) ));
    BOOST_CHECK( Well::GuideRateTarget::COMB       == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::COMB ) ));
    BOOST_CHECK( Well::GuideRateTarget::WGA        == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::WGA ) ));
    BOOST_CHECK( Well::GuideRateTarget::CVAL       == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::CVAL ) ));
    BOOST_CHECK( Well::GuideRateTarget::RAT        == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::RAT ) ));
    BOOST_CHECK( Well::GuideRateTarget::RES        == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::RES ) ));
    BOOST_CHECK( Well::GuideRateTarget::UNDEFINED  == Well::GuideRateTargetFromString( Well::GuideRateTarget2String( Well::GuideRateTarget::UNDEFINED ) ));

    BOOST_CHECK_EQUAL( "OIL"        , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "OIL"  ) ));
    BOOST_CHECK_EQUAL( "WAT"        , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "WAT"  ) ));
    BOOST_CHECK_EQUAL( "GAS"        , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "GAS"  ) ));
    BOOST_CHECK_EQUAL( "LIQ"        , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "LIQ"  ) ));
    BOOST_CHECK_EQUAL( "COMB"       , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "COMB"  ) ));
    BOOST_CHECK_EQUAL( "WGA"        , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "WGA"  ) ));
    BOOST_CHECK_EQUAL( "CVAL"       , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "CVAL"  ) ));
    BOOST_CHECK_EQUAL( "RAT"        , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "RAT"  ) ));
    BOOST_CHECK_EQUAL( "RES"        , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "RES"  ) ));
    BOOST_CHECK_EQUAL( "UNDEFINED"  , Well::GuideRateTarget2String(Well::GuideRateTargetFromString( "UNDEFINED"  ) ));

}

BOOST_AUTO_TEST_CASE(handleWEFAC) {
    Ewoms::Parser parser;
    std::string input =
            "START             -- 0 \n"
            "19 JUN 2007 / \n"
            "SCHEDULE\n"
            "DATES             -- 1\n"
            " 10  OKT 2008 / \n"
            "/\n"
            "WELSPECS\n"
            "    'P'       'OP'   9   9 1*     'OIL' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "    'I'       'OP'   1   1 1*     'WATER' 1*      1*  1*   1*  1*   1*  1*  / \n"
            "/\n"
            "COMPDAT\n"
            " 'P'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            " 'P'  9  9   2   2 'OPEN' 1*   46.825   0.311  4332.346 1*  1*  'X'  22.123 / \n"
            " 'I'  1  1   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 / \n"
            "/\n"
            "WCONHIST\n"
            " 'P' 'OPEN' 'RESV' 6*  500 / \n"
            "/\n"
            "WCONINJH\n"
            " 'I' 'WATER' 1* 100 250 / \n"
            "/\n"
            "WEFAC\n"
            "   'P' 0.5 / \n"
            "   'I' 0.9 / \n"
            "/\n"
            "DATES             -- 2\n"
            " 15  OKT 2008 / \n"
            "/\n"

            "DATES             -- 3\n"
            " 18  OKT 2008 / \n"
            "/\n"
            "WEFAC\n"
            "   'P' 1.0 / \n"
            "/\n"
            ;

    auto deck = parser.parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule(deck, grid , fp, runspec);

    //1
    BOOST_CHECK_EQUAL(schedule.getWell("P", 1).getEfficiencyFactor(), 0.5);
    BOOST_CHECK_EQUAL(schedule.getWell("I", 1).getEfficiencyFactor(), 0.9);

    //2
    BOOST_CHECK_EQUAL(schedule.getWell("P", 2).getEfficiencyFactor(), 0.5);
    BOOST_CHECK_EQUAL(schedule.getWell("I", 2).getEfficiencyFactor(), 0.9);

    //3
    BOOST_CHECK_EQUAL(schedule.getWell("P", 3).getEfficiencyFactor(), 1.0);
    BOOST_CHECK_EQUAL(schedule.getWell("I", 3).getEfficiencyFactor(), 0.9);
}

BOOST_AUTO_TEST_CASE(historic_BHP_and_THP) {
    Ewoms::Parser parser;
    std::string input =
        "START             -- 0 \n"
        "19 JUN 2007 / \n"
        "SCHEDULE\n"
        "DATES             -- 1\n"
        " 10  OKT 2008 / \n"
        "/\n"
        "WELSPECS\n"
        " 'P' 'OP' 9 9 1 'OIL' 1* / \n"
        " 'P1' 'OP' 9 9 1 'OIL' 1* / \n"
        " 'I' 'OP' 9 9 1 'WATER' 1* / \n"
        "/\n"
        "WCONHIST\n"
        " P SHUT ORAT 6  500 0 0 0 1.2 1.1 / \n"
        "/\n"
        "WCONPROD\n"
        " P1 SHUT ORAT 6  500 0 0 0 3.2 3.1 / \n"
        "/\n"
        "WCONINJH\n"
        " I WATER STOP 100 2.1 2.2 / \n"
        "/\n"
        ;

    auto deck = parser.parseString(input);
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid, table);
    Runspec runspec (deck);
    Schedule schedule( deck, grid, fp, runspec);

    {
        const auto& prod = schedule.getWell("P", 1).getProductionProperties();
        const auto& pro1 = schedule.getWell("P1", 1).getProductionProperties();
        const auto& inje = schedule.getWell("I", 1).getInjectionProperties();

        BOOST_CHECK_CLOSE( 1.1 * 1e5,  prod.BHPH, 1e-5 );
        BOOST_CHECK_CLOSE( 1.2 * 1e5,  prod.THPH, 1e-5 );
        BOOST_CHECK_CLOSE( 2.1 * 1e5,  inje.BHPH, 1e-5 );
        BOOST_CHECK_CLOSE( 2.2 * 1e5,  inje.THPH, 1e-5 );
        BOOST_CHECK_CLOSE( 0.0 * 1e5,  pro1.BHPH, 1e-5 );
        BOOST_CHECK_CLOSE( 0.0 * 1e5,  pro1.THPH, 1e-5 );

        {
            const auto& wtest_config = schedule.wtestConfig(0);
            BOOST_CHECK_EQUAL(wtest_config.size(), 0U);
        }

        {
            const auto& wtest_config = schedule.wtestConfig(1);
            BOOST_CHECK_EQUAL(wtest_config.size(), 0U);
        }
    }
}

BOOST_AUTO_TEST_CASE(FilterCompletions2) {
    const auto& deck = Parser{}.parseString(createDeckWithWellsAndCompletionData());
    EclipseGrid grid1(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid1, table);
    Runspec runspec (deck);
    Schedule schedule(deck, grid1, fp, runspec);
    std::vector<int> actnum = grid1.getACTNUM();

    {
        const auto& c1_1 = schedule.getWell("OP_1", 1).getConnections();
        const auto& c1_3 = schedule.getWell("OP_1", 3).getConnections();
        BOOST_CHECK_EQUAL(2U, c1_1.size());
        BOOST_CHECK_EQUAL(9U, c1_3.size());
    }
    actnum[grid1.getGlobalIndex(8,8,1)] = 0;
    {
        std::vector<int> globalCell(grid1.getNumActive());
        for(std::size_t i = 0; i < grid1.getNumActive(); ++i)
            if (actnum[grid1.getGlobalIndex(i)])
                globalCell[i] = grid1.getGlobalIndex(i);
        ActiveGridCells active(grid1.getNXYZ(), globalCell.data(),
                               grid1.getNumActive());
        schedule.filterConnections(active);

        const auto& c1_1 = schedule.getWell("OP_1", 1).getConnections();
        const auto& c1_3 = schedule.getWell("OP_1", 3).getConnections();
        BOOST_CHECK_EQUAL(1U, c1_1.size());
        BOOST_CHECK_EQUAL(8U, c1_3.size());
    }
}

BOOST_AUTO_TEST_CASE(VFPINJ_TEST) {
    const char *input = "\
START\n \
8 MAR 1998 /\n \
\n \
GRID \n\
PORO\n\
  1000*0.25 /\n\
PERMX \n\
  1000*0.10/ \n\
COPY \n\
  PERMX PERMY / \n\
  PERMX PERMZ / \n\
/ \n \
SCHEDULE \n\
VFPINJ \n                                       \
-- Table Depth  Rate   TAB  UNITS  BODY    \n\
-- ----- ----- ----- ----- ------ -----    \n\
       5  32.9   WAT   THP METRIC   BHP /  \n\
-- Rate axis \n\
1 3 5 /      \n\
-- THP axis  \n\
7 11 /       \n\
-- Table data with THP# <values 1-num_rates> \n\
1 1.5 2.5 3.5 /    \n\
2 4.5 5.5 6.5 /    \n\
TSTEP \n\
10 10/\n\
VFPINJ \n                                       \
-- Table Depth  Rate   TAB  UNITS  BODY    \n\
-- ----- ----- ----- ----- ------ -----    \n\
       5  100   GAS   THP METRIC   BHP /  \n\
-- Rate axis \n\
1 3 5 /      \n\
-- THP axis  \n\
7 11 /       \n\
-- Table data with THP# <values 1-num_rates> \n\
1 1.5 2.5 3.5 /    \n\
2 4.5 5.5 6.5 /    \n\
--\n\
VFPINJ \n                                       \
-- Table Depth  Rate   TAB  UNITS  BODY    \n\
-- ----- ----- ----- ----- ------ -----    \n\
       10 200  WAT   THP METRIC   BHP /  \n\
-- Rate axis \n\
1 3 5 /      \n\
-- THP axis  \n\
7 11 /       \n\
-- Table data with THP# <values 1-num_rates> \n\
1 1.5 2.5 3.5 /    \n\
2 4.5 5.5 6.5 /    \n";

    const auto& schedule = make_schedule(input);

    BOOST_CHECK( schedule.getEvents().hasEvent(ScheduleEvents::VFPINJ_UPDATE, 0));
    BOOST_CHECK( !schedule.getEvents().hasEvent(ScheduleEvents::VFPINJ_UPDATE, 1));
    BOOST_CHECK( schedule.getEvents().hasEvent(ScheduleEvents::VFPINJ_UPDATE, 2));

    // No such table id
    BOOST_CHECK_THROW(schedule.getVFPInjTable(77,0), std::invalid_argument);

    // Table not defined at step 0
    BOOST_CHECK_THROW(schedule.getVFPInjTable(10,0), std::invalid_argument);

    const Ewoms::VFPInjTable& vfpinjTable2 = schedule.getVFPInjTable(5, 2);
    BOOST_CHECK_EQUAL(vfpinjTable2.getTableNum(), 5);
    BOOST_CHECK_EQUAL(vfpinjTable2.getDatumDepth(), 100);
    BOOST_CHECK_EQUAL(vfpinjTable2.getFloType(), Ewoms::VFPInjTable::FLO_GAS);

    const Ewoms::VFPInjTable& vfpinjTable3 = schedule.getVFPInjTable(10, 2);
    BOOST_CHECK_EQUAL(vfpinjTable3.getTableNum(), 10);
    BOOST_CHECK_EQUAL(vfpinjTable3.getDatumDepth(), 200);
    BOOST_CHECK_EQUAL(vfpinjTable3.getFloType(), Ewoms::VFPInjTable::FLO_WAT);

    const Ewoms::VFPInjTable& vfpinjTable = schedule.getVFPInjTable(5, 0);
    BOOST_CHECK_EQUAL(vfpinjTable.getTableNum(), 5);
    BOOST_CHECK_EQUAL(vfpinjTable.getDatumDepth(), 32.9);
    BOOST_CHECK_EQUAL(vfpinjTable.getFloType(), Ewoms::VFPInjTable::FLO_WAT);

    const auto vfp_tables0 = schedule.getVFPInjTables(0);
    BOOST_CHECK_EQUAL( vfp_tables0.size(), 1U);

    const auto vfp_tables2 = schedule.getVFPInjTables(2);
    BOOST_CHECK_EQUAL( vfp_tables2.size(), 2U);
    //Flo axis
    {
        const std::vector<double>& flo = vfpinjTable.getFloAxis();
        BOOST_REQUIRE_EQUAL(flo.size(), 3U);

        //Unit of FLO is SM3/day, convert to SM3/second
        double conversion_factor = 1.0 / (60*60*24);
        BOOST_CHECK_EQUAL(flo[0], 1*conversion_factor);
        BOOST_CHECK_EQUAL(flo[1], 3*conversion_factor);
        BOOST_CHECK_EQUAL(flo[2], 5*conversion_factor);
    }

    //THP axis
    {
        const std::vector<double>& thp = vfpinjTable.getTHPAxis();
        BOOST_REQUIRE_EQUAL(thp.size(), 2U);

        //Unit of THP is barsa => convert to pascal
        double conversion_factor = 100000.0;
        BOOST_CHECK_EQUAL(thp[0], 7*conversion_factor);
        BOOST_CHECK_EQUAL(thp[1], 11*conversion_factor);
    }

    //The data itself
    {
        typedef Ewoms::VFPInjTable::array_type::size_type size_type;
        const auto size = vfpinjTable.shape();

        BOOST_CHECK_EQUAL(size[0], 2U);
        BOOST_CHECK_EQUAL(size[1], 3U);

        //Table given as BHP => barsa. Convert to pascal
        double conversion_factor = 100000.0;

        double index = 0.5;
        for (size_type t = 0; t < size[0]; ++t) {
            for (size_type f = 0; f < size[1]; ++f) {
                index += 1.0;
                BOOST_CHECK_EQUAL(vfpinjTable(t,f), index*conversion_factor);
            }
        }
    }
}

// tests for the polymer injectivity case
BOOST_AUTO_TEST_CASE(POLYINJ_TEST) {
    const char *deckData =
        "START\n"
        "   8 MAR 2018/\n"
        "GRID\n"
        "PORO\n"
        "  1000*0.25 /\n"
        "PERMX\n"
        "  1000*0.25 /\n"
        "COPY\n"
        "  PERMX  PERMY /\n"
        "  PERMX  PERMZ /\n"
        "/\n"
        "PROPS\n \n"
        "SCHEDULE\n"
        "WELSPECS\n"
        "'INJE01' 'I'    1  1 1 'WATER'     /\n"
        "/\n"
        "WCONINJE\n"
        "'INJE01' 'WATER' 'OPEN' 'RATE' 800.00  1* 1000 /\n"
        "/\n"
        "TSTEP\n"
        " 1/\n"
        "WPOLYMER\n"
        "    'INJE01' 1.0  0.0 /\n"
        "/\n"
        "WPMITAB\n"
        "   'INJE01' 2 /\n"
        "/\n"
        "WSKPTAB\n"
        "    'INJE01' 1  1 /\n"
        "/\n"
        "TSTEP\n"
        " 2*1/\n"
        "WPMITAB\n"
        "   'INJE01' 3 /\n"
        "/\n"
        "WSKPTAB\n"
        "    'INJE01' 2  2 /\n"
        "/\n"
        "TSTEP\n"
        " 1 /\n";

    Ewoms::Parser parser;
    auto deck = parser.parseString(deckData);
    EclipseGrid grid1(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid1, table);
    Runspec runspec (deck);
    Schedule schedule(deck, grid1 , fp, runspec);

    const auto& poly0 = schedule.getWell("INJE01", 0).getPolymerProperties();
    const auto& poly1 = schedule.getWell("INJE01", 1).getPolymerProperties();
    const auto& poly3 = schedule.getWell("INJE01", 3).getPolymerProperties();

    BOOST_CHECK_EQUAL(poly0.m_plymwinjtable, -1);
    BOOST_CHECK_EQUAL(poly0.m_skprwattable, -1);
    BOOST_CHECK_EQUAL(poly0.m_skprpolytable, -1);

    BOOST_CHECK_EQUAL(poly1.m_plymwinjtable, 2);
    BOOST_CHECK_EQUAL(poly1.m_skprwattable, 1);
    BOOST_CHECK_EQUAL(poly1.m_skprpolytable, 1);

    BOOST_CHECK_EQUAL(poly3.m_plymwinjtable, 3);
    BOOST_CHECK_EQUAL(poly3.m_skprwattable, 2);
    BOOST_CHECK_EQUAL(poly3.m_skprpolytable, 2);
}

// Test for WFOAM
BOOST_AUTO_TEST_CASE(WFOAM_TEST) {
    const char *input =
        "START\n"
        "   8 MAR 2018/\n"
        "GRID\n"
        "PERMX\n"
        "  1000*0.25 /\n"
        "PORO\n"
        "  1000*0.25 /\n"
        "COPY\n"
        "  PERMX  PERMY /\n"
        "  PERMX  PERMZ /\n"
        "/\n"
        "PROPS\n \n"
        "SCHEDULE\n"
        "WELSPECS\n"
        "'INJE01' 'I'    1  1 1 'WATER'     /\n"
        "/\n"
        "WCONINJE\n"
        "'INJE01' 'GAS' 'OPEN' 'RATE' 80000.00  1* 1000 /\n"
        "/\n"
        "TSTEP\n"
        " 1/\n"
        "WFOAM\n"
        "    'INJE01' 0.2 /\n"
        "/\n"
        "TSTEP\n"
        " 2*1/\n"
        "WFOAM\n"
        "    'INJE01' 0.3 /\n"
        "/\n"
        "TSTEP\n"
        " 1 /\n";

    const auto& schedule = make_schedule(input);

    const auto& f0 = schedule.getWell("INJE01", 0).getFoamProperties();
    const auto& f1 = schedule.getWell("INJE01", 1).getFoamProperties();
    const auto& f3 = schedule.getWell("INJE01", 3).getFoamProperties();

    BOOST_CHECK_EQUAL(f0.m_foamConcentration, 0.0);
    BOOST_CHECK_EQUAL(f1.m_foamConcentration, 0.2);
    BOOST_CHECK_EQUAL(f3.m_foamConcentration, 0.3);
}

BOOST_AUTO_TEST_CASE(WTEST_CONFIG) {
    const auto& schedule = make_schedule(createDeckWTEST());

    const auto& wtest_config1 = schedule.wtestConfig(0);
    BOOST_CHECK_EQUAL(wtest_config1.size(), 2U);
    BOOST_CHECK(wtest_config1.has("ALLOW"));
    BOOST_CHECK(!wtest_config1.has("BAN"));

    const auto& wtest_config2 = schedule.wtestConfig(1);
    BOOST_CHECK_EQUAL(wtest_config2.size(), 3U);
    BOOST_CHECK(!wtest_config2.has("ALLOW"));
    BOOST_CHECK(wtest_config2.has("BAN"));
    BOOST_CHECK(wtest_config2.has("BAN", WellTestConfig::Reason::GROUP));
    BOOST_CHECK(!wtest_config2.has("BAN", WellTestConfig::Reason::PHYSICAL));
}

static bool has(const std::vector<std::string>& l, const std::string& s) {
    auto f = std::find(l.begin(), l.end(), s);
    return (f != l.end());
}

BOOST_AUTO_TEST_CASE(WELL_STATIC) {
    const auto& deck = Parser{}.parseString(createDeckWithWells());
    EclipseGrid grid1(10,10,10);
    TableManager table ( deck );
    FieldPropsManager fp( deck, Phases{true, true, true}, grid1, table);
    Runspec runspec (deck);
    Schedule schedule(deck, grid1, fp, runspec);

    BOOST_CHECK_THROW( schedule.getWell("NO_SUCH_WELL", 0), std::invalid_argument);
    BOOST_CHECK_THROW( schedule.getWell("W_3", 0), std::invalid_argument);

    auto ws = schedule.getWell("W_3", 3);
    {
        // Make sure the copy constructor works.
        Well ws_copy(ws);
    }
    BOOST_CHECK_EQUAL(ws.name(), "W_3");

    BOOST_CHECK(!ws.updateHead(19, 50));
    BOOST_CHECK(ws.updateHead(1,50));
    BOOST_CHECK(!ws.updateHead(1,50));
    BOOST_CHECK(ws.updateHead(1,1));
    BOOST_CHECK(!ws.updateHead(1,1));

    BOOST_CHECK(ws.updateRefDepth(1.0));
    BOOST_CHECK(!ws.updateRefDepth(1.0));

    ws.updateStatus(Well::Status::OPEN, 0, false);
    ws.updateStatus(Well::Status::SHUT, 0, false);

    const auto& connections = ws.getConnections();
    BOOST_CHECK_EQUAL(connections.size(), 0U);
    auto c2 = std::make_shared<WellConnections>(Connection::Order::TRACK, 1,1);
    c2->addConnection(1,1,1,
                      grid1.getGlobalIndex(1,1,1),
                      100,
                      Connection::State::OPEN,
                      10,
                      10,
                      10,
                      10,
                      10,
                      100);

    BOOST_CHECK(  ws.updateConnections(c2, 0, false) );
    BOOST_CHECK( !ws.updateConnections(c2, 0, false) );
}

BOOST_AUTO_TEST_CASE(WellNames) {
    const auto& schedule = make_schedule(createDeckWTEST());
    auto names = schedule.wellNames("NO_SUCH_WELL", 0);
    BOOST_CHECK_EQUAL(names.size(), 0U);

    auto w1names = schedule.wellNames("W1", 0);
    BOOST_CHECK_EQUAL(w1names.size(), 1U);
    BOOST_CHECK_EQUAL(w1names[0], "W1");

    auto i1names = schedule.wellNames("11", 0);
    BOOST_CHECK_EQUAL(i1names.size(), 0U);

    auto listnamese = schedule.wellNames("*NO_LIST", 0);
    BOOST_CHECK_EQUAL( listnamese.size(), 0U);

    auto listnames0 = schedule.wellNames("*ILIST", 0);
    BOOST_CHECK_EQUAL( listnames0.size(), 0U);

    auto listnames1 = schedule.wellNames("*ILIST", 2);
    BOOST_CHECK_EQUAL( listnames1.size(), 2U);
    BOOST_CHECK( has(listnames1, "I1"));
    BOOST_CHECK( has(listnames1, "I2"));

    auto pnames1 = schedule.wellNames("I*", 0);
    BOOST_CHECK_EQUAL(pnames1.size(), 0U);

    auto pnames2 = schedule.wellNames("W*", 0);
    BOOST_CHECK_EQUAL(pnames2.size(), 3U);
    BOOST_CHECK( has(pnames2, "W1"));
    BOOST_CHECK( has(pnames2, "W2"));
    BOOST_CHECK( has(pnames2, "W3"));

    auto anames = schedule.wellNames("?", 0, {"W1", "W2"});
    BOOST_CHECK_EQUAL(anames.size(), 2U);
    BOOST_CHECK(has(anames, "W1"));
    BOOST_CHECK(has(anames, "W2"));

    auto all_names0 = schedule.wellNames("*", 0);
    BOOST_CHECK_EQUAL( all_names0.size(), 6U);
    BOOST_CHECK( has(all_names0, "W1"));
    BOOST_CHECK( has(all_names0, "W2"));
    BOOST_CHECK( has(all_names0, "W3"));
    BOOST_CHECK( has(all_names0, "DEFAULT"));
    BOOST_CHECK( has(all_names0, "ALLOW"));

    auto all_names = schedule.wellNames("*", 2);
    BOOST_CHECK_EQUAL( all_names.size(), 9U);
    BOOST_CHECK( has(all_names, "I1"));
    BOOST_CHECK( has(all_names, "I2"));
    BOOST_CHECK( has(all_names, "I3"));
    BOOST_CHECK( has(all_names, "W1"));
    BOOST_CHECK( has(all_names, "W2"));
    BOOST_CHECK( has(all_names, "W3"));
    BOOST_CHECK( has(all_names, "DEFAULT"));
    BOOST_CHECK( has(all_names, "ALLOW"));
    BOOST_CHECK( has(all_names, "BAN"));

    auto abs_all = schedule.wellNames();
    BOOST_CHECK_EQUAL(abs_all.size(), 9U);

    WellMatcher wm0( {}, WListManager{});
    const auto& wml0 = wm0.wells();
    BOOST_CHECK(wml0.empty());

    WellMatcher wm1( {"W1", "W2", "W3", "P1", "P2", "P3"}, WListManager{});
    const std::vector<std::string> pwells = {"P1", "P2", "P3"};
    BOOST_CHECK( pwells == wm1.wells("P*"));

    auto wm2 = schedule.wellMatcher(4);
    const auto& all_wells = wm2.wells();
    BOOST_CHECK_EQUAL(all_wells.size(), 9);
    for (const auto& w : std::vector<std::string>{"W1", "W2", "W3", "I1", "I2", "I3", "DEFAULT", "ALLOW", "BAN"})
        BOOST_CHECK(has(all_wells, w));

    const std::vector<std::string> wwells = {"W1", "W2", "W3"};
    BOOST_CHECK( wwells == wm2.wells("W*"));
    BOOST_CHECK( wm2.wells("XYZ*").empty() );
    BOOST_CHECK( wm2.wells("XYZ").empty() );

    auto def = wm2.wells("DEFAULT");
    BOOST_CHECK_EQUAL(def.size() , 1);
    BOOST_CHECK_EQUAL(def[0], "DEFAULT");

    auto l2 = wm2.wells("*ILIST");
    BOOST_CHECK_EQUAL( l2.size(), 2U);
    BOOST_CHECK( has(l2, "I1"));
    BOOST_CHECK( has(l2, "I2"));
}

BOOST_AUTO_TEST_CASE(RFT_CONFIG) {
    std::vector<std::time_t> tp = { asTimeT( TimeStampUTC(2010, 1, 1)),
                                    asTimeT( TimeStampUTC(2010, 1, 2)),
                                    asTimeT( TimeStampUTC(2010, 1, 3)),
                                    asTimeT( TimeStampUTC(2010, 1, 4)),
                                    asTimeT( TimeStampUTC(2010, 1, 5)),
                                    asTimeT( TimeStampUTC(2010, 1, 6))};

    TimeMap tm(tp);
    RFTConfig conf(tm);
    BOOST_CHECK_THROW( conf.rft("W1", 100), std::invalid_argument);
    BOOST_CHECK_THROW( conf.plt("W1", 100), std::invalid_argument);

    BOOST_CHECK(!conf.rft("W1", 2));
    BOOST_CHECK(!conf.plt("W1", 2));

    conf.setWellOpenRFT(2);
    BOOST_CHECK(!conf.getWellOpenRFT("W1", 0));

    conf.updateRFT("W1", 2, RFTConfig::RFT::YES);
    BOOST_CHECK(conf.rft("W1", 2));
    BOOST_CHECK(!conf.rft("W1", 1));
    BOOST_CHECK(!conf.rft("W1", 3));

    conf.updateRFT("W2", 2, RFTConfig::RFT::REPT);
    conf.updateRFT("W2", 4, RFTConfig::RFT::NO);
    BOOST_CHECK(!conf.rft("W2", 1));
    BOOST_CHECK( conf.rft("W2", 2));
    BOOST_CHECK( conf.rft("W2", 3));
    BOOST_CHECK(!conf.rft("W2", 4));

    conf.setWellOpenRFT("W3");
    BOOST_CHECK(conf.getWellOpenRFT("W3", 2));

    conf.updateRFT("W4", 2, RFTConfig::RFT::FOPN);
    BOOST_CHECK(conf.getWellOpenRFT("W4", 2));

    conf.addWellOpen("W10", 2);
    conf.addWellOpen("W100", 3);
}

BOOST_AUTO_TEST_CASE(RFT_CONFIG2) {
    const auto& schedule = make_schedule(createDeckRFTConfig());
    const auto& rft_config = schedule.rftConfig();

    BOOST_CHECK_EQUAL(2U, rft_config.firstRFTOutput());
}

BOOST_AUTO_TEST_CASE(nupcol) {
    std::string input =
        "RUNSPEC\n"
        "START             -- 0 \n"
        "19 JUN 2007 / \n"
        "NUPCOL\n"
        "  20 /\n"
        "SCHEDULE\n"
        "DATES\n             -- 1\n"
        " 10  OKT 2008 / \n"
        "/\n"
        "NUPCOL\n"
        "  1* /\n"
        "DATES\n             -- 1\n"
        " 10  OKT 2009 / \n"
        "/\n"
        "NUPCOL\n"
        "  10 /\n"
        "DATES\n             -- 1\n"
        " 10  OKT 2010 / \n"
        "/\n"

        ;

    const auto& schedule = make_schedule(input);
    {
        // EFlow uses 12 as default
        BOOST_CHECK_EQUAL(schedule.getNupcol(0),20);
        BOOST_CHECK_EQUAL(schedule.getNupcol(1),12);
        BOOST_CHECK_EQUAL(schedule.getNupcol(2),10);
    }
}

BOOST_AUTO_TEST_CASE(TESTGuideRateConfig) {
    std::string input = R"(
START             -- 0
10 MAI 2007 /
SCHEDULE
WELSPECS
     'W1'    'G1'   1 2  3.33       'OIL'  7*/
     'W2'    'G2'   1 3  3.33       'OIL'  3*  YES /
     'W3'    'G3'   1 4  3.92       'OIL'  3*  NO /
/

COMPDAT
 'W1'  1  1   1   1 'OPEN' 1*    1.168   0.311   107.872 1*  1*  'Z'  21.925 /
 'W2'  1  1   2   2 'OPEN' 1*    1.168   0.311   107.872 1*  1*  'Z'  21.925 /
 'W3'  1  1   3   3 'OPEN' 1*    1.168   0.311   107.872 1*  1*  'Z'  21.925 /
/

WCONPROD
     'W1'      'OPEN'      'ORAT'      0.000      0.000      0.000  5* /
/

WGRUPCON
    'W1' 'YES'   0.50 'OIL' /
    'W2' 'YES'   0.50 'GAS' /
/

GCONPROD
 'G1' 'ORAT' 1000 /
 'G2' 'ORAT' 1000 5* 0.25 'OIL' /
/

DATES             -- 1
 10  JUN 2007 /
/

WCONHIST
     'W1'      'OPEN'      'ORAT'      1.000      0.000      0.000  5* /
/

WGRUPCON
    'W1' 'YES'   0.75 'WAT' /
    'W2' 'NO' /
/

GCONPROD
 'G2' 'ORAT' 1000 /
 'G1' 'ORAT' 1000 6* 'FORM' /
/

DATES             -- 2
 10  JUL 2007 /
/

WCONPROD
     'W1'      'OPEN'      'ORAT'      0.000      0.000      0.000  5* /
/

DATES             -- 3
 10  AUG 2007 /
/

DATES             -- 4
 10  SEP 2007 /
/

DATES             -- 5
 10  NOV 2007 /
/

WELSPECS
     'W4'    'G1'   1 2  3.33       'OIL'  7*/
/

DATES       -- 6
    10 DEC 2007 /
/

COMPDAT
  'W4'  1  1   1   1 'OPEN' 1*    1.168   0.311   107.872 1*  1*  'Z'  21.925 /
/

     )";

    const auto& schedule = make_schedule(input);
    {
        const auto& grc = schedule.guideRateConfig(0);
        const auto& w1_node = grc.well("W1");
        BOOST_CHECK(w1_node.target == Well::GuideRateTarget::OIL);

        const auto& w2_node = grc.well("W2");
        BOOST_CHECK(w2_node.target == Well::GuideRateTarget::GAS);

        BOOST_CHECK(!grc.has_group("G1"));
        BOOST_CHECK(grc.has_group("G2"));
    }
    {
        const auto& grc = schedule.guideRateConfig(2);
        const auto& w1_node = grc.well("W1");
        BOOST_CHECK(w1_node.target == Well::GuideRateTarget::WAT);
        BOOST_CHECK_EQUAL(w1_node.guide_rate, 0.75);

        BOOST_CHECK(grc.has_well("W1"));
        BOOST_CHECK(!grc.has_well("W2"));
        BOOST_CHECK_THROW( grc.well("W2"), std::out_of_range);

        BOOST_CHECK(grc.has_group("G1"));
        BOOST_CHECK(!grc.has_group("G2"));
    }

    {
        GuideRate gr(schedule);
        double oil_pot = 1;
        double gas_pot = 1;
        double wat_pot = 1;

        gr.compute("XYZ",1, 1.0, oil_pot, gas_pot, wat_pot);
    }
    {
        const auto& changed_wells = schedule.changed_wells(0);
        BOOST_CHECK_EQUAL( changed_wells.size() , 3U);
        for (const auto& wname : {"W1", "W2", "W2"}) {
            auto find_well = std::find(changed_wells.begin(), changed_wells.end(), wname);
            BOOST_CHECK(find_well != changed_wells.end());
        }
    }
    {
        const auto& changed_wells = schedule.changed_wells(2);
        BOOST_CHECK_EQUAL( changed_wells.size(), 0U);
    }
    {
        const auto& changed_wells = schedule.changed_wells(4);
        BOOST_CHECK_EQUAL( changed_wells.size(), 0U);
    }
    {
        const auto& changed_wells = schedule.changed_wells(5);
        BOOST_CHECK_EQUAL( changed_wells.size(), 1U);
        BOOST_CHECK_EQUAL( changed_wells[0], "W4");
    }
    {
        const auto& changed_wells = schedule.changed_wells(6);
        BOOST_CHECK_EQUAL( changed_wells.size(), 1U);
        BOOST_CHECK_EQUAL( changed_wells[0], "W4");
    }
}

BOOST_AUTO_TEST_CASE(Injection_Control_Mode_From_Well) {
    const auto input = R"(RUNSPEC

SCHEDULE
WELSPECS
     'W1'    'G1'   1 2  3.33       'OIL'  7*/
     'W2'    'G2'   1 3  3.33       'OIL'  3*  YES /
     'W3'    'G3'   1 4  3.92       'OIL'  3*  NO /
     'W4'    'G3'   2 2  3.92       'OIL'  3*  NO /
     'W5'    'G3'   2 3  3.92       'OIL'  3*  NO /
     'W6'    'G3'   2 4  3.92       'OIL'  3*  NO /
     'W7'    'G3'   3 2  3.92       'OIL'  3*  NO /
/

WCONINJE
  'W1' 'WATER'  'OPEN'  'GRUP' /
  'W2' 'GAS'  'OPEN'  'RATE'  200  1*  450.0 /
  'W3' 'OIL'  'OPEN'  'RATE'  200  1*  450.0 /
  'W4' 'WATER'  'OPEN'  'RATE'  200  1*  450.0 /
  'W5' 'WATER'  'OPEN'  'RESV'  200  175  450.0 /
  'W6' 'GAS'  'OPEN'  'BHP'  200  1*  450.0 /
  'W7' 'GAS'  'OPEN'  'THP'  200  1*  450.0 150 /
/

TSTEP
  30*30 /

END
)";

    const auto sched = make_schedule(input);
    const auto st = ::Ewoms::SummaryState{ std::chrono::system_clock::now() };

    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W1", 10), st), -1);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W2", 10), st), 3);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W3", 10), st), 1);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W4", 10), st), 2);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W5", 10), st), 5);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W6", 10), st), 7);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W7", 10), st), 6);
}

BOOST_AUTO_TEST_CASE(Production_Control_Mode_From_Well) {
    const auto input = R"(RUNSPEC

SCHEDULE
WELSPECS
     'W1'    'G1'   1 2  3.33       'OIL'  7*/
     'W2'    'G2'   1 3  3.33       'OIL'  3*  YES /
     'W3'    'G3'   1 4  3.92       'OIL'  3*  NO /
     'W4'    'G3'   2 2  3.92       'OIL'  3*  NO /
     'W5'    'G3'   2 3  3.92       'OIL'  3*  NO /
     'W6'    'G3'   2 4  3.92       'OIL'  3*  NO /
     'W7'    'G3'   3 2  3.92       'OIL'  3*  NO /
     'W8'    'G3'   3 3  3.92       'OIL'  3*  NO /
/

WCONPROD
  'W1' 'OPEN'  'GRUP' /
  'W2' 'OPEN'  'ORAT' 1000.0 /
  'W3' 'OPEN'  'WRAT' 1000.0 250.0 /
  'W4' 'OPEN'  'GRAT' 1000.0 250.0 30.0e3 /
  'W5' 'OPEN'  'LRAT' 1000.0 250.0 30.0e3 1500.0 /
  'W6' 'OPEN'  'RESV' 1000.0 250.0 30.0e3 1500.0 314.15 /
  'W7' 'OPEN'  'BHP' 1000.0 250.0 30.0e3 1500.0 314.15 27.1828 /
  'W8' 'OPEN'  'THP' 1000.0 250.0 30.0e3 1500.0 314.15 27.1828 31.415 /
/

TSTEP
  30*30 /

END
)";

    const auto sched = make_schedule(input);
    const auto st = ::Ewoms::SummaryState{ std::chrono::system_clock::now() };

    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W1", 10), st), -1);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W2", 10), st), 1);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W3", 10), st), 2);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W4", 10), st), 3);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W5", 10), st), 4);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W6", 10), st), 5);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W7", 10), st), 7);
    BOOST_CHECK_EQUAL(eclipseControlMode(sched.getWell("W8", 10), st), 6);
}

BOOST_AUTO_TEST_CASE(SKIPREST_VFP) {
    Parser parser;
    auto deck = parser.parseFile("MODEL2_RESTART.DATA");
    EclipseState es{ deck };
    const auto& init_config = es.getInitConfig();
    auto report_step = init_config.getRestartStep();
    const auto& rst_filename = es.getIOConfig().getRestartFileName( init_config.getRestartRootName(), report_step, false );
    Ewoms::EclIO::ERst rst_file(rst_filename);
    const auto& rst = Ewoms::RestartIO::RstState::load(rst_file, report_step);
    const auto sched = Schedule{ deck, es , &rst};
    const auto& tables = sched.getVFPProdTables(3);
    BOOST_CHECK( !tables.empty() );
}

BOOST_AUTO_TEST_CASE(GASLIFT_OPT) {
    GasLiftOpt glo;
    BOOST_CHECK(!glo.active());
    BOOST_CHECK_THROW(glo.group("NO_SUCH_GROUP"), std::out_of_range);
    BOOST_CHECK_THROW(glo.well("NO_SUCH_WELL"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(GASLIFT_OPT_DECK) {
    const auto input = R"(-- Turns on gas lift optimization
SCHEDULE

GRUPTREE
 'PROD'    'FIELD' /

 'M5S'    'PLAT-A'  /
 'M5N'    'PLAT-A'  /

 'C1'     'M5N'  /
 'F1'     'M5N'  /
 'B1'     'M5S'  /
 'G1'     'M5S'  /
 /

LIFTOPT
 12500 5E-3 0.0 YES /

-- Group lift gas limits for gas lift optimization
GLIFTOPT
 'PLAT-A'  200000 /  --
/

WELSPECS
--WELL     GROUP  IHEEL JHEEL   DREF PHASE   DRAD INFEQ SIINS XFLOW PRTAB  DENS
 'B-1H'  'B1'   11    3      1*   OIL     1*   1*   SHUT 1* 1* 1* /
 'B-2H'  'B1'    4    7      1*   OIL     1*   1*   SHUT 1* 1* 1* /
 'B-3H'  'B1'   11   12      1*   OIL     1*   1*   SHUT 1* 1* 1* /
 'C-1H'  'C1'   13   20      1*   OIL     1*   1*   SHUT 1* 1* 1* /
 'C-2H'  'C1'   12   27      1*   OIL     1*   1*   SHUT 1* 1* 1* /
/

-- well savailable for gass lift
-- minimum gas lift rate, enough to keep well flowing
WLIFTOPT
 'B-1H'   YES   150000   1.01   -1.0  /
 'B-2H'   YES   150000   1.01   -1.0  /
 'B-3H'   YES   150000   1.01   -1.0  /
 'C-1H'   YES   150000   1.01   -1.0  1.0 YES/
 'C-2H'   NO    150000   1.01   -1.0  /
/
)";
    Ewoms::UnitSystem unitSystem = UnitSystem( UnitSystem::UnitType::UNIT_TYPE_METRIC );
    double siFactorG = unitSystem.parse("GasSurfaceVolume/Time").getSIScaling();
    const auto sched = make_schedule(input);
    const auto& glo = sched.glo(0);
    const auto& plat_group = glo.group("PLAT-A");
    BOOST_CHECK_EQUAL( *plat_group.max_lift_gas(), siFactorG * 200000);
    BOOST_CHECK(!static_cast<bool>(plat_group.max_total_gas()));
    BOOST_CHECK(glo.has_group("PLAT-A"));
    BOOST_CHECK(!glo.has_well("NO-GROUP"));

    const auto& w1 = glo.well("B-1H");
    BOOST_CHECK(w1.use_glo());
    BOOST_CHECK_EQUAL(*w1.max_rate(), 150000 * siFactorG);
    BOOST_CHECK_EQUAL(w1.weight_factor(), 1.01);

    const auto& w2 = glo.well("C-2H");
    BOOST_CHECK_EQUAL(w2.weight_factor(), 1.00);
    BOOST_CHECK_EQUAL(w2.min_rate(), 0.00);
    BOOST_CHECK_EQUAL(w2.inc_weight_factor(), 0.00);
    BOOST_CHECK(!w2.alloc_extra_gas());

    const auto& w3 = glo.well("C-1H");
    BOOST_CHECK_EQUAL(w3.min_rate(), -1.00 * siFactorG);
    BOOST_CHECK_EQUAL(w3.inc_weight_factor(), 1.00);
    BOOST_CHECK(w3.alloc_extra_gas());
    BOOST_CHECK(glo.has_well("C-1H"));
    BOOST_CHECK(!glo.has_well("NO-WELL"));
}

BOOST_AUTO_TEST_CASE(WellPI) {
    const auto deck = Parser{}.parseString(R"(RUNSPEC
START
7 OCT 2020 /

DIMENS
  10 10 3 /

GRID
DXV
  10*100.0 /
DYV
  10*100.0 /
DZV
  3*10.0 /

DEPTHZ
  121*2000.0 /

PERMX
  300*100.0 /
PERMY
  300*100.0 /
PERMZ
  300*10.0 /
PORO
  300*0.3 /

SCHEDULE
WELSPECS
  'P' 'G' 10 10 2005 'LIQ' /
/
COMPDAT
  'P' 0 0 1 3 OPEN 1 100 /
/

TSTEP
  10
/

WELPI
  'P'  200.0 /
/

TSTEP
  10
/

COMPDAT
  'P' 0 0 2 2 OPEN 1 50 /
/

TSTEP
  10
/

END
)");

    const auto es    = EclipseState{ deck };
    const auto sched = Schedule{ deck, es };

    // Apply WELPI before seeing WELPI data
    {
        const auto expectCF = 100.0*cp_rm3_per_db();
        auto wellP = sched.getWell("P", 0);

        std::vector<bool> scalingApplicable;
        wellP.applyWellProdIndexScaling(2.7182818, scalingApplicable);
        for (const auto& conn : wellP.getConnections()) {
            BOOST_CHECK_CLOSE(conn.CF(), expectCF, 1.0e-10);
        }

        for (const bool applicable : scalingApplicable) {
            BOOST_CHECK_MESSAGE(! applicable, "No connection must be eligible for WELPI scaling");
        }
    }

    // Apply WELPI after seeing WELPI data.
    {
        const auto expectCF = (200.0 / 100.0) * 100.0*cp_rm3_per_db();
        auto wellP = sched.getWell("P", 1);

        const auto scalingFactor = wellP.getWellPIScalingFactor(100.0*liquid_PI_unit());
        BOOST_CHECK_CLOSE(scalingFactor, 2.0, 1.0e-10);

        std::vector<bool> scalingApplicable;
        wellP.applyWellProdIndexScaling(scalingFactor, scalingApplicable);
        for (const auto& conn : wellP.getConnections()) {
            BOOST_CHECK_CLOSE(conn.CF(), expectCF, 1.0e-10);
        }

        for (const bool applicable : scalingApplicable) {
            BOOST_CHECK_MESSAGE(applicable, "All connections must be eligible for WELPI scaling");
        }
    }

    // Apply WELPI after new COMPDAT.
    {
        const auto expectCF = (200.0 / 100.0) * 100.0*cp_rm3_per_db();
        auto wellP = sched.getWell("P", 2);

        const auto scalingFactor = wellP.getWellPIScalingFactor(100.0*liquid_PI_unit());
        BOOST_CHECK_CLOSE(scalingFactor, 2.0, 1.0e-10);

        std::vector<bool> scalingApplicable;
        wellP.applyWellProdIndexScaling(scalingFactor, scalingApplicable);
        const auto& connP = wellP.getConnections();
        BOOST_CHECK_CLOSE(connP[0].CF(), expectCF          , 1.0e-10);
        BOOST_CHECK_CLOSE(connP[1].CF(), 50*cp_rm3_per_db(), 1.0e-10);
        BOOST_CHECK_CLOSE(connP[2].CF(), expectCF          , 1.0e-10);

        BOOST_CHECK_MESSAGE(bool(scalingApplicable[0]), "Connection[0] must be eligible for WELPI scaling");
        BOOST_CHECK_MESSAGE(!    scalingApplicable[1] , "Connection[1] must NOT be eligible for WELPI scaling");
        BOOST_CHECK_MESSAGE(bool(scalingApplicable[0]), "Connection[2] must be eligible for WELPI scaling");
    }
}

BOOST_AUTO_TEST_CASE(Schedule_ApplyWellProdIndexScaling) {
    const auto deck = Parser{}.parseString(R"(RUNSPEC
START
7 OCT 2020 /

DIMENS
  10 10 3 /

GRID
DXV
  10*100.0 /
DYV
  10*100.0 /
DZV
  3*10.0 /

DEPTHZ
  121*2000.0 /

PERMX
  300*100.0 /
PERMY
  300*100.0 /
PERMZ
  300*10.0 /
PORO
  300*0.3 /

SCHEDULE
WELSPECS -- 0
  'P' 'G' 10 10 2005 'LIQ' /
/
COMPDAT
  'P' 0 0 1 3 OPEN 1 100 /
/

TSTEP -- 1
  10
/

WELPI -- 1
  'P'  200.0 /
/

TSTEP -- 2
  10
/

COMPDAT -- 2
  'P' 0 0 2 2 OPEN 1 50 /
/

TSTEP -- 3
  10
/

WELPI --3
  'P'  50.0 /
/

TSTEP -- 4
  10
/

COMPDAT -- 4
  'P' 10 9 2 2 OPEN 1 100 1.0 3* 'Y' /
  'P' 10 8 2 2 OPEN 1  75 1.0 3* 'Y' /
  'P' 10 7 2 2 OPEN 1  25 1.0 3* 'Y' /
/

TSTEP -- 5
  10
/

END
)");

    const auto es    = EclipseState{ deck };
    auto       sched = Schedule{ deck, es };

    BOOST_REQUIRE_EQUAL(sched.getTimeMap().size(),         std::size_t{6});
    BOOST_REQUIRE_EQUAL(sched.getTimeMap().numTimesteps(), std::size_t{5});
    BOOST_REQUIRE_EQUAL(sched.getTimeMap().last(),         std::size_t{5});

    BOOST_REQUIRE_MESSAGE(sched.hasWellGroupEvent("P", ScheduleEvents::Events::WELL_PRODUCTIVITY_INDEX, 1),
                          R"(Schedule must have WELL_PRODUCTIVITY_INDEX Event for well "P" at report step 1)");

    BOOST_REQUIRE_MESSAGE(sched.hasWellGroupEvent("P", ScheduleEvents::Events::WELL_PRODUCTIVITY_INDEX, 3),
                          R"(Schedule must have WELL_PRODUCTIVITY_INDEX Event for well "P" at report step 3)");

    BOOST_REQUIRE_MESSAGE(sched.getEvents().hasEvent(ScheduleEvents::Events::WELL_PRODUCTIVITY_INDEX, 1),
                          "Schedule must have WELL_PRODUCTIVITY_INDEX Event at report step 1");

    BOOST_REQUIRE_MESSAGE(sched.getEvents().hasEvent(ScheduleEvents::Events::WELL_PRODUCTIVITY_INDEX, 3),
                          "Schedule must have WELL_PRODUCTIVITY_INDEX Event at report step 3");

    auto getScalingFactor = [&sched](const std::size_t report_step, const double wellPI) -> double
    {
        return sched.getWell("P", report_step).getWellPIScalingFactor(wellPI);
    };

    auto applyWellPIScaling = [&sched](const std::size_t report_step, const double scalingFactor)
    {
        sched.applyWellProdIndexScaling("P", report_step, scalingFactor);
    };

    auto getConnections = [&sched](const std::size_t report_step)
    {
        return sched.getWell("P", report_step).getConnections();
    };

    // Apply WELPI scaling after end of time series => no change to CTFs
    {
        const auto report_step   = std::size_t{1};
        const auto scalingFactor = getScalingFactor(report_step, 100.0*liquid_PI_unit());

        BOOST_CHECK_CLOSE(scalingFactor, 2.0, 1.0e-10);

        applyWellPIScaling(1729, scalingFactor);

        {
            const auto expectCF = 100.0*cp_rm3_per_db();

            const auto& conns = getConnections(0);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF, 1.0e-10);
        }

        {
            const auto expectCF = 100.0*cp_rm3_per_db();

            const auto& conns = getConnections(1);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF, 1.0e-10);
        }

        {
            const auto expectCF = 100.0*cp_rm3_per_db();

            const auto& conns = getConnections(2);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,             1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), 50.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,             1.0e-10);
        }

        {
            const auto expectCF = 100.0*cp_rm3_per_db();

            const auto& conns = getConnections(3);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,             1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), 50.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,             1.0e-10);
        }

        {
            const auto& conns = getConnections(4);
            BOOST_REQUIRE_EQUAL(conns.size(), 6);

            BOOST_CHECK_CLOSE(conns[0].CF(), 100.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(),  50.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), 100.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[3].CF(), 100.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[4].CF(),  75.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[5].CF(),  25.0*cp_rm3_per_db(), 1.0e-10);
        }
    }

    // Apply WELPI scaling after first WELPI specification
    {
        const auto report_step   = std::size_t{1};
        const auto scalingFactor = getScalingFactor(report_step, 100.0*liquid_PI_unit());

        BOOST_CHECK_CLOSE(scalingFactor, 2.0, 1.0e-10);

        applyWellPIScaling(report_step, scalingFactor);

        {
            const auto expectCF = 100.0*cp_rm3_per_db();

            const auto& conns = getConnections(0);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF, 1.0e-10);
        }

        {
            const auto expectCF = 200.0*cp_rm3_per_db();

            const auto& conns = getConnections(1);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF, 1.0e-10);
        }

        {
            const auto expectCF = 200.0*cp_rm3_per_db();

            const auto& conns = getConnections(2);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,             1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), 50.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,             1.0e-10);
        }

        {
            const auto expectCF = 200.0*cp_rm3_per_db();

            const auto& conns = getConnections(3);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,             1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), 50.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,             1.0e-10);
        }

        {
            const auto expectCF = 200.0*cp_rm3_per_db();

            const auto& conns = getConnections(4);
            BOOST_REQUIRE_EQUAL(conns.size(), 6);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,              1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(),  50.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,              1.0e-10);
            BOOST_CHECK_CLOSE(conns[3].CF(), 100.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[4].CF(),  75.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[5].CF(),  25.0*cp_rm3_per_db(), 1.0e-10);
        }
    }

    // Apply WELPI scaling after second WELPI specification
    {
        const auto report_step   = std::size_t{3};
        const auto scalingFactor = getScalingFactor(report_step, 200.0*liquid_PI_unit());

        BOOST_CHECK_CLOSE(scalingFactor, 0.25, 1.0e-10);

        applyWellPIScaling(report_step, scalingFactor);

        {
            const auto expectCF = 100.0*cp_rm3_per_db();

            const auto& conns = getConnections(0);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF, 1.0e-10);
        }

        {
            const auto expectCF = 200.0*cp_rm3_per_db();

            const auto& conns = getConnections(1);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF, 1.0e-10);
        }

        {
            const auto expectCF = 200.0*cp_rm3_per_db();

            const auto& conns = getConnections(2);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,             1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), 50.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,             1.0e-10);
        }

        {
            const auto expectCF = 50.0*cp_rm3_per_db();

            const auto& conns = getConnections(3);
            BOOST_REQUIRE_EQUAL(conns.size(), 3);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,      1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), 0.25*expectCF, 1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,      1.0e-10);
        }

        {
            const auto expectCF = 50.0*cp_rm3_per_db();

            const auto& conns = getConnections(4);
            BOOST_REQUIRE_EQUAL(conns.size(), 6);

            BOOST_CHECK_CLOSE(conns[0].CF(), expectCF,              1.0e-10);
            BOOST_CHECK_CLOSE(conns[1].CF(), 0.25*expectCF,         1.0e-10);
            BOOST_CHECK_CLOSE(conns[2].CF(), expectCF,              1.0e-10);
            BOOST_CHECK_CLOSE(conns[3].CF(), 100.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[4].CF(),  75.0*cp_rm3_per_db(), 1.0e-10);
            BOOST_CHECK_CLOSE(conns[5].CF(),  25.0*cp_rm3_per_db(), 1.0e-10);
        }
    }
}

void cmp_vector(const std::vector<double>&v1, const std::vector<double>& v2) {
    BOOST_CHECK_EQUAL(v1.size(), v2.size());
    for (std::size_t i = 0; i < v1.size(); i++)
        BOOST_CHECK_CLOSE(v1[i], v2[i], 1e-4);
}

BOOST_AUTO_TEST_CASE(VFPPROD_SCALING) {
    const auto deck = Parser{}.parseFile("VFP_CASE.DATA");
    const auto es    = EclipseState{ deck };
    const auto sched = Schedule{ deck, es };
    const auto& vfp_table = sched.getVFPProdTable(1, 0);
    const std::vector<double> flo = { 0.000578704,  0.001157407,  0.002893519,  0.005787037,  0.008680556,  0.011574074,  0.017361111,  0.023148148,  0.034722222,  0.046296296};
    const std::vector<double> thp = {1300000.000000000, 2500000.000000000, 5000000.000000000, 7500000.000000000, 10000000.000000000};
    const std::vector<double> wfr = { 0.000000000,  0.100000000,  0.200000000,  0.300000000,  0.400000000,  0.500000000,  0.600000000,  0.700000000,  0.800000000,  0.990000000};
    const std::vector<double> gfr = {100.000000000, 200.000000000, 300.000000000, 400.000000000, 500.000000000, 750.000000000, 1000.000000000, 2000.000000000};
    const std::vector<double> alq = { 0.000000000,  0.000578704,  0.001157407,  0.001736111,  0.002314815};

    cmp_vector(flo, vfp_table.getFloAxis());
    cmp_vector(thp, vfp_table.getTHPAxis());
    cmp_vector(wfr, vfp_table.getWFRAxis());
    cmp_vector(gfr, vfp_table.getGFRAxis());
    cmp_vector(alq, vfp_table.getALQAxis());
}

BOOST_AUTO_TEST_CASE(WPAVE) {
    const std::string deck_string = R"(
START
7 OCT 2020 /

DIMENS
  10 10 3 /

GRID
DXV
  10*100.0 /
DYV
  10*100.0 /
DZV
  3*10.0 /

DEPTHZ
  121*2000.0 /

PORO
  300*0.3 /

SCHEDULE
WELSPECS -- 0
  'P1' 'G' 10 10 2005 'LIQ' /
  'P2' 'G' 1 10 2005 'LIQ' /
  'P3' 'G' 2 10 2005 'LIQ' /
  'P4' 'G' 3 10 2005 'LIQ' /
/

TSTEP -- 1
  10
/

WPAVE   -- PAVG1
  0.75 0.25 /

TSTEP -- 2
  10
/

WWPAVE
  P1 0.30 0.60 /   -- PAVG2
  P3 0.40 0.70 /   -- PAVG3
/

TSTEP -- 3
  10
/

WPAVE   -- PAVG4
  0.10 0.10 /

TSTEP -- 4
  10
/

TSTEP -- 5
  10
/

END
)";

    const auto deck = Parser{}.parseString(deck_string);
    const auto es    = EclipseState{ deck };
    auto       sched = Schedule{ deck, es };

    PAvg pavg0;
    PAvg pavg1( deck.getKeyword("WPAVE", 0).getRecord(0) );
    PAvg pavg2( deck.getKeyword("WWPAVE", 0).getRecord(0) );
    PAvg pavg3( deck.getKeyword("WWPAVE", 0).getRecord(1) );
    PAvg pavg4( deck.getKeyword("WPAVE", 1).getRecord(0) );

    {
        const auto& w1 = sched.getWell("P1", 0);
        const auto& w4 = sched.getWell("P4", 0);

        BOOST_CHECK(w1.pavg() == pavg0);
        BOOST_CHECK(w4.pavg() == pavg0);
    }

    {
        const auto& w1 = sched.getWell("P1", 1);
        const auto& w4 = sched.getWell("P4", 1);

        BOOST_CHECK(w1.pavg() == pavg1);
        BOOST_CHECK(w4.pavg() == pavg1);
    }

    {
        const auto& w1 = sched.getWell("P1", 2);
        const auto& w3 = sched.getWell("P3", 2);
        const auto& w4 = sched.getWell("P4", 2);

        BOOST_CHECK(w1.pavg() == pavg2);
        BOOST_CHECK(w3.pavg() == pavg3);
        BOOST_CHECK(w4.pavg() == pavg1);
    }

    {
        const auto& w1 = sched.getWell("P1", 3);
        const auto& w2 = sched.getWell("P2", 3);
        const auto& w3 = sched.getWell("P3", 3);
        const auto& w4 = sched.getWell("P4", 3);

        BOOST_CHECK(w1.pavg() == pavg4);
        BOOST_CHECK(w2.pavg() == pavg4);
        BOOST_CHECK(w3.pavg() == pavg4);
        BOOST_CHECK(w4.pavg() == pavg4);
    }
}

BOOST_AUTO_TEST_CASE(WELL_STATUS) {
    const std::string deck_string = R"(
START
7 OCT 2020 /

DIMENS
  10 10 3 /

GRID
DXV
  10*100.0 /
DYV
  10*100.0 /
DZV
  3*10.0 /

DEPTHZ
  121*2000.0 /

PORO
  300*0.3 /

SCHEDULE
WELSPECS -- 0
  'P1' 'G' 10 10 2005 'LIQ' /
/

COMPDAT
  'P1'  9  9   1   1 'OPEN' 1*   32.948   0.311  3047.839 1*  1*  'X'  22.100 /
/

WCONPROD
  'P1' 'OPEN' 'ORAT'  123.4  4*  50.0 /
/

TSTEP -- 1
  10 /

WELPI
  'P1'  200.0 /
/

TSTEP -- 2
  10 /

WELOPEN
   'P1' SHUT /
/

TSTEP -- 3,4,5
  10 10 10 /

WELOPEN
   'P1' OPEN /
/

TSTEP -- 6,7,8
  10 10 10/

END

END
)";

    const auto deck = Parser{}.parseString(deck_string);
    const auto es    = EclipseState{ deck };
    auto       sched = Schedule{ deck, es };
    {
        const auto& well = sched.getWell("P1", 0);
        BOOST_CHECK( well.getStatus() ==  Well::Status::OPEN);
    }
    {
        const auto& well = sched.getWell("P1", 1);
        BOOST_CHECK( well.getStatus() ==  Well::Status::OPEN);
    }

    {
        const auto& well = sched.getWell("P1", 2);
        BOOST_CHECK( well.getStatus() ==  Well::Status::SHUT);
    }
    {
        const auto& well = sched.getWell("P1", 5);
        BOOST_CHECK( well.getStatus() ==  Well::Status::OPEN);
    }

    sched.shut_well("P1", 0);
    {
        const auto& well = sched.getWell("P1", 0);
        BOOST_CHECK( well.getStatus() ==  Well::Status::SHUT);
    }
    {
        const auto& well = sched.getWell("P1", 1);
        BOOST_CHECK( well.getStatus() ==  Well::Status::SHUT);
    }
    {
        const auto& well = sched.getWell("P1", 2);
        BOOST_CHECK( well.getStatus() ==  Well::Status::SHUT);
    }
    {
        const auto& well = sched.getWell("P1", 5);
        BOOST_CHECK( well.getStatus() ==  Well::Status::OPEN);
    }

    //sched.open_well("P1", 2);
}

