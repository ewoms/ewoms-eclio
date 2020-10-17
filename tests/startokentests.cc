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

#define BOOST_TEST_MODULE ParserTests
#include <stdexcept>
#include <boost/test/unit_test.hpp>

#include "ewoms/eclio/parser/rawdeck/startoken.hh"

BOOST_AUTO_TEST_CASE(NoStarThrows) {
    BOOST_REQUIRE_THROW(Ewoms::StarToken st("Hei...") , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(InvalidCountThrow) {
    BOOST_REQUIRE_THROW( Ewoms::StarToken st("X*") , std::invalid_argument);
    BOOST_REQUIRE_THROW( Ewoms::StarToken st("1.25*") , std::invalid_argument);
    BOOST_REQUIRE_THROW( Ewoms::StarToken st("-3*") , std::invalid_argument);
    BOOST_REQUIRE_THROW( Ewoms::StarToken st("0*") , std::invalid_argument);
    BOOST_REQUIRE_THROW( Ewoms::StarToken st("*123") , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(CountCorrect) {
    Ewoms::StarToken st1("*");
    Ewoms::StarToken st2("5*");
    Ewoms::StarToken st3("54*");
    BOOST_CHECK(st1.countString() == "");
    BOOST_CHECK(st2.countString() == "5");
    BOOST_CHECK(st3.countString() == "54");

    BOOST_CHECK(st1.valueString() == "");
    BOOST_CHECK(st2.valueString() == "");
    BOOST_CHECK(st3.valueString() == "");

    BOOST_CHECK(!st1.hasValue());
    BOOST_CHECK(!st2.hasValue());
    BOOST_CHECK(!st3.hasValue());

    BOOST_REQUIRE_EQUAL(1U , st1.count());
    BOOST_REQUIRE_EQUAL(5U , st2.count());
    BOOST_REQUIRE_EQUAL(54U , st3.count());
}

BOOST_AUTO_TEST_CASE(NoValueGetValueThrow) {
    Ewoms::StarToken st1("*");
    Ewoms::StarToken st2("5*");
    BOOST_CHECK_EQUAL( false , st1.hasValue());
    BOOST_CHECK_EQUAL( false , st2.hasValue());
}

BOOST_AUTO_TEST_CASE(StarNoCountThrows) {
    BOOST_CHECK_THROW( Ewoms::StarToken st1("*10") , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(CorrectValueString) {
    Ewoms::StarToken st1("1*10.09");
    Ewoms::StarToken st2("5*20.13");
    Ewoms::StarToken st3("1*'123'");
    Ewoms::StarToken st4("1*123*456");
    BOOST_CHECK_EQUAL( true , st1.hasValue());
    BOOST_CHECK_EQUAL( true , st2.hasValue());
    BOOST_CHECK_EQUAL( true , st3.hasValue());
    BOOST_CHECK_EQUAL( true , st4.hasValue());

    BOOST_CHECK_EQUAL( "10.09" , st1.valueString());
    BOOST_CHECK_EQUAL( "20.13" , st2.valueString());
    BOOST_CHECK_EQUAL( "'123'" , st3.valueString());
    BOOST_CHECK_EQUAL( "123*456" , st4.valueString());
}

BOOST_AUTO_TEST_CASE( ContainsStar_WithStar_ReturnsTrue ) {
    std::string countString, valueString;
    BOOST_CHECK_EQUAL( true , Ewoms::isStarToken("*", countString, valueString) );
    BOOST_CHECK_EQUAL( true , Ewoms::isStarToken("*1", countString, valueString) );
    BOOST_CHECK_EQUAL( true , Ewoms::isStarToken("1*", countString, valueString) );
    BOOST_CHECK_EQUAL( true , Ewoms::isStarToken("1*2", countString, valueString) );

    BOOST_CHECK_EQUAL( false , Ewoms::isStarToken("12", countString, valueString) );
    BOOST_CHECK_EQUAL( false , Ewoms::isStarToken("'12*34'", countString, valueString) );
}

BOOST_AUTO_TEST_CASE( readValueToken_basic_validity_tests ) {
    BOOST_CHECK_THROW( Ewoms::readValueToken<int>( std::string( "3.3" ) ), std::invalid_argument );
    BOOST_CHECK_EQUAL( 3, Ewoms::readValueToken<int>( std::string( "3" ) ) );
    BOOST_CHECK_EQUAL( 3, Ewoms::readValueToken<int>( std::string( "+3" ) ) );
    BOOST_CHECK_EQUAL( -3, Ewoms::readValueToken<int>( std::string( "-3" ) ) );
    BOOST_CHECK_THROW( Ewoms::readValueToken<double>( std::string( "truls" ) ), std::invalid_argument );
    BOOST_CHECK_EQUAL( 0, Ewoms::readValueToken<double>( std::string( "0" ) ) );
    BOOST_CHECK_EQUAL( 0, Ewoms::readValueToken<double>( std::string( "0.0" ) ) );
    BOOST_CHECK_EQUAL( 0, Ewoms::readValueToken<double>( std::string( "+0.0" ) ) );
    BOOST_CHECK_EQUAL( 0, Ewoms::readValueToken<double>( std::string( "-0.0" ) ) );
    BOOST_CHECK_EQUAL( 0, Ewoms::readValueToken<double>( std::string( ".0" ) ) );
    BOOST_CHECK_THROW( Ewoms::readValueToken<double>( std::string( "1.0.0" ) ), std::invalid_argument );
    BOOST_CHECK_THROW( Ewoms::readValueToken<double>( std::string( "1g0" ) ), std::invalid_argument );
    BOOST_CHECK_THROW( Ewoms::readValueToken<double>( std::string( "1.23h" ) ), std::invalid_argument );
    BOOST_CHECK_THROW( Ewoms::readValueToken<double>( std::string( "+1.23h" ) ), std::invalid_argument );
    BOOST_CHECK_THROW( Ewoms::readValueToken<double>( std::string( "-1.23h" ) ), std::invalid_argument );
    BOOST_CHECK_EQUAL( 3.3, Ewoms::readValueToken<double>( std::string( "3.3" ) ) );
    BOOST_CHECK_CLOSE( 3.3, Ewoms::readValueToken<double>( std::string( "3.3e0" ) ), 1e-6 );
    BOOST_CHECK_CLOSE( 3.3, Ewoms::readValueToken<double>( std::string( "3.3d0" ) ), 1e-6 );
    BOOST_CHECK_CLOSE( 3.3, Ewoms::readValueToken<double>( std::string( "3.3E0" ) ), 1e-6 );
    BOOST_CHECK_CLOSE( 3.3, Ewoms::readValueToken<double>( std::string( "3.3D0" ) ), 1e-6 );
    BOOST_CHECK_EQUAL( "OLGA", Ewoms::readValueToken<std::string>( std::string( "OLGA" ) ) );
    BOOST_CHECK_EQUAL( "OLGA", Ewoms::readValueToken<std::string>( std::string( "'OLGA'" ) ) );
    BOOST_CHECK_EQUAL( "123*456", Ewoms::readValueToken<std::string>( std::string( "123*456" ) ) );
    BOOST_CHECK_EQUAL( "123*456", Ewoms::readValueToken<std::string>( std::string( "'123*456'" ) ) );
}
