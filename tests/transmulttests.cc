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

#define BOOST_TEST_MODULE EclipseGridTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipse3dproperties.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/transmult.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/transmult.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/gridproperty.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/griddims.hh>

BOOST_AUTO_TEST_CASE(Empty) {
    Ewoms::Eclipse3DProperties props;
    Ewoms::EclipseGrid grid(10,10,10);
    Ewoms::FieldPropsManager fp(Ewoms::Deck(), grid, Ewoms::TableManager());
    Ewoms::TransMult transMult(grid ,{} , fp);

    BOOST_CHECK_THROW( transMult.getMultiplier(12,10,10 , Ewoms::FaceDir::XPlus) , std::invalid_argument );
    BOOST_CHECK_THROW( transMult.getMultiplier(1000 , Ewoms::FaceDir::XPlus) , std::invalid_argument );

    BOOST_CHECK_EQUAL( transMult.getMultiplier(9,9,9, Ewoms::FaceDir::YPlus) , 1.0 );
    BOOST_CHECK_EQUAL( transMult.getMultiplier(100 , Ewoms::FaceDir::ZPlus) , 1.0 );

    BOOST_CHECK_EQUAL( transMult.getMultiplier(9,9,9, Ewoms::FaceDir::YMinus) , 1.0 );
    BOOST_CHECK_EQUAL( transMult.getMultiplier(100 , Ewoms::FaceDir::ZMinus) , 1.0 );
}

BOOST_AUTO_TEST_CASE(GridAndEdit) {
    const std::string deck_string = R"(
RUNSPEC
GRIDOPTS
  'YES'  2 /

DIMENS
 5 5 5 /
GRID
MULTZ
  125*2 /
EDIT
MULTZ
  125*2 /
)";

    Ewoms::Parser parser;
    Ewoms::Deck deck = parser.parseString(deck_string);
    Ewoms::TableManager tables(deck);
    Ewoms::EclipseGrid grid(5,5,5);
    Ewoms::FieldPropsManager fp(deck, grid, tables);
    Ewoms::TransMult transMult(grid, deck, fp);

    transMult.applyMULT(fp.get_global<double>("MULTZ"), Ewoms::FaceDir::ZPlus);
    BOOST_CHECK_EQUAL( transMult.getMultiplier(0,0,0 , Ewoms::FaceDir::ZPlus) , 4.0 );
}
