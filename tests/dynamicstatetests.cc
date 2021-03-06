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

#include <stdexcept>
#include <iostream>

#define BOOST_TEST_MODULE DynamicStateTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/schedule/timemap.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/dynamicstate.hh>

Ewoms::TimeMap make_timemap(int num) {
    std::vector<std::time_t> tp;
    for (int i = 0; i < num; i++)
        tp.push_back( Ewoms::asTimeT(Ewoms::TimeStampUTC(2010,1,i+1)));

    Ewoms::TimeMap timeMap{ tp };
    return timeMap;
}

BOOST_AUTO_TEST_CASE(CreateDynamicTest) {
    const std::time_t startDate = Ewoms::TimeMap::mkdate(2010, 1, 1);
    Ewoms::TimeMap timeMap({ startDate });
    Ewoms::DynamicState<double> state(timeMap , 9.99);
}

BOOST_AUTO_TEST_CASE(DynamicStateGetOutOfRangeThrows) {
    const std::time_t startDate = Ewoms::TimeMap::mkdate(2010, 1, 1);
    Ewoms::TimeMap timeMap({ startDate });
    Ewoms::DynamicState<double> state(timeMap , 9.99);
    BOOST_CHECK_THROW( state.get(1) , std::out_of_range );
}

BOOST_AUTO_TEST_CASE(DynamicStateGetDefault) {
    const std::time_t startDate = Ewoms::TimeMap::mkdate(2010, 1, 1);
    Ewoms::TimeMap timeMap( { startDate } );
    Ewoms::DynamicState<int> state(timeMap , 137);
    BOOST_CHECK_EQUAL( 137 , state.get(0));
    BOOST_CHECK_EQUAL( 137 , state.back() );
}

BOOST_AUTO_TEST_CASE(DynamicStateSetOutOfRangeThrows) {
    Ewoms::TimeMap timeMap = make_timemap(3);
    Ewoms::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_THROW( state.update(3 , 100) , std::out_of_range );
}

BOOST_AUTO_TEST_CASE(DynamicStateSetOK) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 137);

    state.update(2 , 23 );
    BOOST_CHECK_EQUAL( 137 , state.get(0));
    BOOST_CHECK_EQUAL( 137 , state.get(1));
    BOOST_CHECK_EQUAL( 23 , state.get(2));
    BOOST_CHECK_EQUAL( 23 , state.get(5));

    state.update(2 , 17);
    BOOST_CHECK_EQUAL( 137 , state.get(0));
    BOOST_CHECK_EQUAL( 137 , state.get(1));
    BOOST_CHECK_EQUAL( 17 , state.get(2));
    BOOST_CHECK_EQUAL( 17 , state.get(5));

    state.update(6 , 60);
    BOOST_CHECK_EQUAL( 17 , state.get(2));
    BOOST_CHECK_EQUAL( 17 , state.get(5));
    BOOST_CHECK_EQUAL( 60 , state.get(6));
    BOOST_CHECK_EQUAL( 60 , state.get(8));
    BOOST_CHECK_EQUAL( 60 , state.get(9));
    BOOST_CHECK_EQUAL( 60 , state.back());
}

BOOST_AUTO_TEST_CASE(DynamicStateAddAt) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 0);

    state.update( 10 , 77 );
    {
        const int& v1 = state.at(10);
        int v2 = state.get(10);
        BOOST_CHECK_EQUAL( v1 , 77 );
        BOOST_CHECK_EQUAL( v1 , v2 );
        BOOST_CHECK( &v1 != &v2 );
    }
}

BOOST_AUTO_TEST_CASE(DynamicStateOperatorSubscript) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 137);

    state.update( 10 , 200 );
    BOOST_CHECK_EQUAL( state[9] , 137 );
    BOOST_CHECK_EQUAL( state[0] , 137 );

}

BOOST_AUTO_TEST_CASE(DynamicStateInitial) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 137);
    Ewoms::DynamicState<int> state2(timeMap , 137);

    state.update( 10 , 200 );
    BOOST_CHECK_EQUAL( state[9] , 137 );
    BOOST_CHECK_EQUAL( state[0] , 137 );
    BOOST_CHECK_EQUAL( state[10] , 200 );

    state.updateInitial( 63 );
    BOOST_CHECK_EQUAL( state[9] , 63 );
    BOOST_CHECK_EQUAL( state[0] , 63 );
    BOOST_CHECK_EQUAL( state[10] , 200 );

    state.updateInitial( 73 );
    BOOST_CHECK_EQUAL( state[9] , 73 );
    BOOST_CHECK_EQUAL( state[0] , 73 );
    BOOST_CHECK_EQUAL( state[10] , 200 );

    state2.update( 10 , 200 );
    BOOST_CHECK_EQUAL( state2[9] , 137 );
    BOOST_CHECK_EQUAL( state2[0] , 137 );
    BOOST_CHECK_EQUAL( state2[10] , 200 );
    state.updateInitial( 73 );
    BOOST_CHECK_EQUAL( state2[9] , 137 );
    BOOST_CHECK_EQUAL( state2[0] , 137 );
    BOOST_CHECK_EQUAL( state2[10] , 200 );
}

BOOST_AUTO_TEST_CASE( ResetGlobal ) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 137);

    state.update(5 , 100);
    BOOST_CHECK_EQUAL( state[0] , 137 );
    BOOST_CHECK_EQUAL( state[4] , 137 );
    BOOST_CHECK_EQUAL( state[5] , 100 );
    BOOST_CHECK_EQUAL( state[9] , 100 );

    state.updateInitial( 22 );
    BOOST_CHECK_EQUAL( state[0] , 22 );
    BOOST_CHECK_EQUAL( state[4] , 22 );
    BOOST_CHECK_EQUAL( state[5] , 100 );
    BOOST_CHECK_EQUAL( state[9] , 100 );

    state.globalReset( 88 );
    BOOST_CHECK_EQUAL( state[0] , 88 );
    BOOST_CHECK_EQUAL( state[4] , 88 );
    BOOST_CHECK_EQUAL( state[5] , 88 );
    BOOST_CHECK_EQUAL( state[9] , 88 );
}

BOOST_AUTO_TEST_CASE( CheckReturn ) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_EQUAL( false , state.update( 0 , 137 ));
    BOOST_CHECK_EQUAL( false , state.update( 3 , 137 ));
    BOOST_CHECK_EQUAL( true , state.update( 5 , 200 ));
}

BOOST_AUTO_TEST_CASE( UpdateEmptyInitial ) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_EQUAL( state[5] , 137 );
    state.updateInitial( 99 );
    BOOST_CHECK_EQUAL( state[5] , 99 );
}

BOOST_AUTO_TEST_CASE( find ) {
    Ewoms::TimeMap timeMap = make_timemap(6);
    Ewoms::DynamicState<int> state(timeMap , 137);

    BOOST_CHECK_EQUAL( state.find( 137 ).value() , 0U );
    BOOST_CHECK_EQUAL( state.find_not(200).value(), 0U);
    BOOST_CHECK( !state.find( 200 ));
    BOOST_CHECK( !state.find_not(137));

    state.update( 0 , 200 );
    BOOST_CHECK( !state.find( 137 ) );
    BOOST_CHECK_EQUAL( state.find( 200 ).value() ,  0U );

    state.update( 2 , 300 );
    BOOST_CHECK_EQUAL( state.find( 200 ).value() ,  0U );
    BOOST_CHECK_EQUAL( state.find( 300 ).value() ,  2U );
    BOOST_CHECK_EQUAL( state.find_not( 200 ).value() ,  2U );

    state.update( 4 , 400 );
    BOOST_CHECK_EQUAL( state.find( 200 ).value() ,  0U );
    BOOST_CHECK_EQUAL( state.find( 300 ).value() ,  2U );
    BOOST_CHECK_EQUAL( state.find( 400 ).value() ,  4U );
    BOOST_CHECK( !state.find( 500 ));

    auto pred = [] (const int& elm) { return elm == 400 ;};
    BOOST_CHECK_EQUAL( state.find_if(pred).value(), 4U);
}

BOOST_AUTO_TEST_CASE( update_elm ) {
    Ewoms::TimeMap timeMap = make_timemap(6);
    Ewoms::DynamicState<int> state(timeMap , 137);
    state.update( 5, 88 );
    BOOST_CHECK_THROW( state.update_elm(10,88) , std::out_of_range );
    BOOST_CHECK_EQUAL( state[2],137 );
    BOOST_CHECK_EQUAL( state[3],137 );
    BOOST_CHECK_EQUAL( state[4],137 );

    state.update_elm(3,88);
    BOOST_CHECK_EQUAL( state[2],137 );
    BOOST_CHECK_EQUAL( state[3],88 );
    BOOST_CHECK_EQUAL( state[4],137 );

    for (auto& v : state)
        v += 2;

    BOOST_CHECK_EQUAL( state[2],139 );
    BOOST_CHECK_EQUAL( state[3],90  );
    BOOST_CHECK_EQUAL( state[4],139 );
}

BOOST_AUTO_TEST_CASE( update_equal ) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 0);
    state.update( 5, 100 );
    BOOST_REQUIRE_THROW(state.update_equal(100, 100), std::out_of_range);

    BOOST_CHECK_EQUAL(state[0], 0);
    BOOST_CHECK_EQUAL(state[4], 0);
    BOOST_CHECK_EQUAL(state[5], 100);

    state.update_equal(3,50);
    BOOST_CHECK_EQUAL(state[2], 0);
    BOOST_CHECK_EQUAL(state[3], 50);
    BOOST_CHECK_EQUAL(state[4], 50);
    BOOST_CHECK_EQUAL(state[5], 100);

    {
        auto next_index = state.update_equal(4,50);
        BOOST_CHECK_EQUAL(state[4], 50);
        BOOST_CHECK_EQUAL(state[5], 100);
        BOOST_CHECK_EQUAL(next_index.value() , 5U);
    }

    {
        auto next_index = state.update_equal(9,200);
        BOOST_CHECK_EQUAL(state[8] , 100);
        BOOST_CHECK_EQUAL(state[9] , 200);
        BOOST_CHECK_EQUAL(state[10], 200);
        BOOST_CHECK(!next_index);
    }
}

BOOST_AUTO_TEST_CASE( update_range) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 0);

    BOOST_CHECK_THROW( state.update_range(5,1,99), std::exception);
    BOOST_CHECK_THROW( state.update_range(10,200,99), std::exception);
    state.update_range(3,5,99);
    BOOST_CHECK_EQUAL( state[3], 99);
    BOOST_CHECK_EQUAL( state[4], 99);
    BOOST_CHECK_EQUAL( state[5], 0);
}

BOOST_AUTO_TEST_CASE( UNIQUE ) {
    Ewoms::TimeMap timeMap = make_timemap(11);
    Ewoms::DynamicState<int> state(timeMap , 13);
    auto unique0 = state.unique();
    BOOST_CHECK_EQUAL(unique0.size(), 1U);
    BOOST_CHECK(unique0[0] == std::make_pair(std::size_t{0}, 13));

    state.update(3,300);
    state.update(6,600);
    auto unique1 = state.unique();
    BOOST_CHECK_EQUAL(unique1.size(), 3U);
    BOOST_CHECK(unique1[0] == std::make_pair(std::size_t{0}, 13));
    BOOST_CHECK(unique1[1] == std::make_pair(std::size_t{3}, 300));
    BOOST_CHECK(unique1[2] == std::make_pair(std::size_t{6}, 600));
}

