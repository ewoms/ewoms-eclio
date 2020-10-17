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

#define BOOST_TEST_MODULE TableSchemaTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/eclipsestate/tables/tableschema.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/columnschema.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tableenums.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE( CreateTest ) {
    TableSchema schema;
    ColumnSchema col1("Name1" , Table::INCREASING , Table::DEFAULT_NONE);
    ColumnSchema col2("Name2" , Table::INCREASING , Table::DEFAULT_NONE);
    BOOST_CHECK_EQUAL( 0U , schema.size( ) );

    schema.addColumn( col1 );
    BOOST_CHECK_EQUAL( 1U , schema.size( ) );

    schema.addColumn( col2 );
    BOOST_CHECK_EQUAL( 2U , schema.size( ) );

    BOOST_CHECK_THROW( schema.getColumn( "NO/NOT/THIS/COLUMN" ) , std::invalid_argument );
    BOOST_CHECK_THROW( schema.getColumn( 5 ) , std::invalid_argument );
}

