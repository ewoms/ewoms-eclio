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

#define BOOST_TEST_MODULE TABDIMS_TESTS
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/eclipsestate/tables/tabdims.hh>

BOOST_AUTO_TEST_CASE(TEST_CREATE) {
    Ewoms::Tabdims tabdims;

    BOOST_CHECK_EQUAL( tabdims.getNumSatNodes() , 20U );
}
