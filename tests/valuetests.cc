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

#define BOOST_TEST_MODULE VALUETESTS
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/util/value.hh>

BOOST_AUTO_TEST_CASE( check_default_constructor ) {
    Ewoms::Value<int> v("Value");

    BOOST_CHECK_EQUAL( false , v.hasValue() );
    BOOST_CHECK_THROW( v.getValue() , std::logic_error );

    v.setValue( 70 );
    BOOST_CHECK_EQUAL( 70 , v.getValue());
}

BOOST_AUTO_TEST_CASE( check_value_constructor ) {
    Ewoms::Value<int> v("Value" , 100);

    BOOST_CHECK_EQUAL( true , v.hasValue() );
    BOOST_CHECK_EQUAL( 100 , v.getValue());
}

BOOST_AUTO_TEST_CASE( check_equal1 ) {
    Ewoms::Value<int> v1("v1" , 100);
    Ewoms::Value<int> v2("v2" , 100);

    BOOST_CHECK(v1.equal( v2 ));

    v1.setValue(110);
    BOOST_CHECK_EQUAL( false , v1.equal(v2));
}

BOOST_AUTO_TEST_CASE( check_equal2 ) {
    Ewoms::Value<int> v1("v1");
    Ewoms::Value<int> v2("v2");

    BOOST_CHECK_EQUAL(true , v1.equal( v2 ));

    v1.setValue(110);
    BOOST_CHECK_EQUAL( false , v1.equal(v2));
    v2.setValue(110);
    BOOST_CHECK_EQUAL( true , v1.equal(v2));
}

BOOST_AUTO_TEST_CASE( check_assign) {
    Ewoms::Value<int> v1("v1",100);
    Ewoms::Value<int> v2(v1);

    BOOST_CHECK(v1.equal(v2));
}
