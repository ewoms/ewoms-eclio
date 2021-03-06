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

#define BOOST_TEST_MODULE ColumnSchemaTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/eclipsestate/tables/columnschema.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE( CreateTest ) {
    ColumnSchema schema("Name" , Table::INCREASING , Table::DEFAULT_NONE);
    BOOST_CHECK_EQUAL( schema.name() , "Name");
    BOOST_CHECK_THROW( schema.getDefaultValue() , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE( CreateDefaultConst ) {
    ColumnSchema schema("Name" , Table::INCREASING , 1.76);
    BOOST_CHECK_EQUAL( schema.name() , "Name");
    BOOST_CHECK_EQUAL( schema.getDefaultMode() , Table::DEFAULT_CONST );
    BOOST_CHECK_EQUAL( schema.getDefaultValue() , 1.76 );
}

BOOST_AUTO_TEST_CASE( TestOrder) {
    {
        ColumnSchema schema("Name" , Table::INCREASING , Table::DEFAULT_NONE);
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 1 , 0 ) );
    }

    {
        ColumnSchema schema("Name" , Table::DECREASING , Table::DEFAULT_NONE);
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 1 , 0 ) );
    }

    {
        ColumnSchema schema("Name" , Table::STRICTLY_INCREASING  , Table::DEFAULT_NONE);
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 1 , 0 ) );
    }

    {
        ColumnSchema schema("Name" , Table::STRICTLY_DECREASING  , Table::DEFAULT_NONE);
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 0 ) );
        BOOST_CHECK_EQUAL( false , schema.validOrder( 0 , 1 ) );
        BOOST_CHECK_EQUAL( true  , schema.validOrder( 1 , 0 ) );
    }
}

BOOST_AUTO_TEST_CASE( CanLookup ) {
    ColumnSchema schema1("Name" , Table::INCREASING , Table::DEFAULT_NONE);
    ColumnSchema schema2("Name" , Table::RANDOM , Table::DEFAULT_NONE);

    BOOST_CHECK( schema1.lookupValid( ) );
    BOOST_CHECK( !schema2.lookupValid( ) );
}

