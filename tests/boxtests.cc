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

#include <stdexcept>
#include <iostream>
#include <memory>

#define BOOST_TEST_MODULE BoxManagereTests

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/boxmanager.hh>

BOOST_AUTO_TEST_CASE(CreateBox) {
    Ewoms::EclipseGrid grid(4,3,2);
    Ewoms::Box box(grid);
    BOOST_CHECK_EQUAL( 24U , box.size() );
    BOOST_CHECK( box.isGlobal() );
    BOOST_CHECK_EQUAL( 4U , box.getDim(0) );
    BOOST_CHECK_EQUAL( 3U , box.getDim(1) );
    BOOST_CHECK_EQUAL( 2U , box.getDim(2) );

    BOOST_CHECK_THROW( box.getDim(5) , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(CreateSubBox) {
    Ewoms::EclipseGrid grid(10,10,10);
    Ewoms::Box globalBox(grid);

    BOOST_CHECK_THROW( new Ewoms::Box( grid , -1 , 9 , 1 , 8 , 1, 8)  , std::invalid_argument);   //  Negative throw
    BOOST_CHECK_THROW( new Ewoms::Box( grid ,  1 , 19 , 1 , 8 , 1, 8) , std::invalid_argument);   //  Bigger than global: throw
    BOOST_CHECK_THROW( new Ewoms::Box( grid ,  9 , 1  , 1 , 8 , 1, 8) , std::invalid_argument);   //  Inverted order: throw

    Ewoms::Box subBox1(grid, 0,9,0,9,0,9);
    BOOST_CHECK( subBox1.isGlobal());

    Ewoms::Box subBox2(grid, 1,3,1,4,1,5);
    BOOST_CHECK( !subBox2.isGlobal());
    BOOST_CHECK_EQUAL( 60U , subBox2.size() );
}

BOOST_AUTO_TEST_CASE(BoxEqual) {
    Ewoms::EclipseGrid grid1(10,10,10);
    Ewoms::EclipseGrid grid3(10,10,11);
    Ewoms::EclipseGrid grid4(20,20,20);
    Ewoms::Box globalBox1( grid1 );
    Ewoms::Box globalBox2( grid1 );
    Ewoms::Box globalBox3( grid3 );

    Ewoms::Box globalBox4(grid4);
    Ewoms::Box subBox1( grid1 , 0 , 9 , 0 , 9 , 0, 9);
    Ewoms::Box subBox4( grid4 , 0 , 9 , 0 , 9 , 0, 9);
    Ewoms::Box subBox5( grid4 , 10 , 19 , 10 , 19 , 10, 19);

    BOOST_CHECK( globalBox1.equal( globalBox2 ));
    BOOST_CHECK( !globalBox1.equal( globalBox3 ));
    BOOST_CHECK( globalBox1.equal( subBox1 ));

    BOOST_CHECK( !globalBox4.equal( subBox4 ));
    BOOST_CHECK( !subBox4.equal( subBox5 ));
}

BOOST_AUTO_TEST_CASE(CreateBoxManager) {
    Ewoms::EclipseGrid grid(10,10,10);
    Ewoms::BoxManager boxManager(grid);
    Ewoms::Box box(grid);

    BOOST_CHECK( box.equal( boxManager.getActiveBox()) );
}

BOOST_AUTO_TEST_CASE(TestInputBox) {
    Ewoms::EclipseGrid grid(10,10,10);
    Ewoms::BoxManager boxManager(grid);
    Ewoms::Box inputBox( grid, 0,4,0,4,0,4);
    Ewoms::Box globalBox( grid );

    boxManager.setInputBox( 0,4,0,4,0,4 );
    BOOST_CHECK( inputBox.equal( boxManager.getActiveBox()) );
    boxManager.endSection();
    BOOST_CHECK( boxManager.getActiveBox().equal(globalBox));
}

BOOST_AUTO_TEST_CASE(TestKeywordBox) {
    Ewoms::EclipseGrid grid(10,10,10);
    Ewoms::BoxManager boxManager(grid);
    Ewoms::Box inputBox( grid, 0,4,0,4,0,4);
    Ewoms::Box keywordBox( grid, 0,2,0,2,0,2);
    Ewoms::Box globalBox( grid );

    boxManager.setInputBox( 0,4,0,4,0,4 );
    BOOST_CHECK( inputBox.equal( boxManager.getActiveBox()) );

    boxManager.setKeywordBox( 0,2,0,2,0,2 );
    BOOST_CHECK( keywordBox.equal( boxManager.getActiveBox()) );

    // Must end keyword first
    BOOST_CHECK_THROW( boxManager.endSection() , std::invalid_argument );

    boxManager.endKeyword();
    BOOST_CHECK( inputBox.equal( boxManager.getActiveBox()) );

    boxManager.endSection();
    BOOST_CHECK( boxManager.getActiveBox().equal(globalBox));
}

BOOST_AUTO_TEST_CASE(BoxNineArg) {
    const size_t nx = 10;
    const size_t ny = 7;
    const size_t nz = 6;
    Ewoms::EclipseGrid grid(nx,ny,nz);
    BOOST_CHECK_NO_THROW( Ewoms::Box(grid,0,7,0,5,1,2) );

    // J2 < J1
    BOOST_CHECK_THROW( Ewoms::Box(grid,1,1,4,3,2,2), std::invalid_argument);

    // K2 >= Nz
    BOOST_CHECK_THROW( Ewoms::Box(grid,1,1,2,2,3,nz), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(TestKeywordBox2) {
    Ewoms::EclipseGrid grid(10,10,10);
    std::vector<int> actnum(grid.getCartesianSize(), 1);
    actnum[0] = 0;
    grid.resetACTNUM(actnum);

    Ewoms::BoxManager boxManager(grid);
    const auto& box = boxManager.getActiveBox();

    for (const auto& p : box.index_list())
        BOOST_CHECK_EQUAL(p.active_index + 1, p.global_index);
    BOOST_CHECK_EQUAL(box.index_list().size() + 1, grid.getCartesianSize());

    const auto& global_index_list = box.global_index_list();
    BOOST_CHECK_EQUAL( global_index_list.size(), grid.getCartesianSize());
    const auto& c0 = global_index_list[0];
    BOOST_CHECK_EQUAL(c0.global_index, 0U);
    BOOST_CHECK_EQUAL(c0.active_index, c0.global_index);
    BOOST_CHECK_EQUAL(c0.data_index, 0U);

    Ewoms::Box box2(grid,9,9,9,9,0,9);
    const auto& il = box2.index_list();
    BOOST_CHECK_EQUAL(il.size(), 10U);

    for (std::size_t i=0; i < 10; i++) {
        BOOST_CHECK_EQUAL(il[i].data_index, i);
        BOOST_CHECK_EQUAL(il[i].global_index, 99 + i*100);
        BOOST_CHECK_EQUAL(il[i].active_index, 98 + i*100);
    }
}
