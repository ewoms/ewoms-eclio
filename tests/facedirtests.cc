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
#include <memory>

#define BOOST_TEST_MODULE FaceDirTests
#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>

namespace Ewoms {

BOOST_AUTO_TEST_CASE(CheckEnum) {
    BOOST_CHECK_EQUAL( FaceDir::XPlus , FaceDir::FromString( "X"));
    BOOST_CHECK_EQUAL( FaceDir::XPlus , FaceDir::FromString( "I"));
    BOOST_CHECK_EQUAL( FaceDir::XMinus , FaceDir::FromString( "X-"));
    BOOST_CHECK_EQUAL( FaceDir::XMinus , FaceDir::FromString( "I-"));

    BOOST_CHECK_EQUAL( FaceDir::YPlus , FaceDir::FromString( "Y"));
    BOOST_CHECK_EQUAL( FaceDir::YPlus , FaceDir::FromString( "J"));
    BOOST_CHECK_EQUAL( FaceDir::YMinus , FaceDir::FromString( "Y-"));
    BOOST_CHECK_EQUAL( FaceDir::YMinus , FaceDir::FromString( "J-"));

    BOOST_CHECK_EQUAL( FaceDir::ZPlus , FaceDir::FromString( "Z"));
    BOOST_CHECK_EQUAL( FaceDir::ZPlus , FaceDir::FromString( "K"));
    BOOST_CHECK_EQUAL( FaceDir::ZMinus , FaceDir::FromString( "Z-"));
    BOOST_CHECK_EQUAL( FaceDir::ZMinus , FaceDir::FromString( "K-"));

    BOOST_CHECK_THROW( FaceDir::FromString("??") , std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(CheckComposite) {
    BOOST_CHECK( FaceDir::XPlus & FaceDir::FromMULTREGTString("X"));
    BOOST_CHECK( FaceDir::XMinus & FaceDir::FromMULTREGTString("X"));
    BOOST_CHECK_EQUAL( FaceDir::XPlus  + FaceDir::XMinus ,  FaceDir::FromMULTREGTString("X"));

    BOOST_CHECK( FaceDir::YPlus & FaceDir::FromMULTREGTString("Y"));
    BOOST_CHECK( FaceDir::YMinus & FaceDir::FromMULTREGTString("Y"));
    BOOST_CHECK_EQUAL( FaceDir::YPlus  + FaceDir::YMinus ,  FaceDir::FromMULTREGTString("Y"));

    BOOST_CHECK( FaceDir::ZPlus & FaceDir::FromMULTREGTString("Z"));
    BOOST_CHECK( FaceDir::ZMinus & FaceDir::FromMULTREGTString("Z"));
    BOOST_CHECK_EQUAL( FaceDir::ZPlus  + FaceDir::ZMinus ,  FaceDir::FromMULTREGTString("Z"));

    BOOST_CHECK_EQUAL( FaceDir::XPlus + FaceDir::YPlus + FaceDir::XMinus + FaceDir::YMinus , FaceDir::FromMULTREGTString("XY"));
    BOOST_CHECK_EQUAL( FaceDir::XPlus + FaceDir::ZPlus + FaceDir::XMinus + FaceDir::ZMinus, FaceDir::FromMULTREGTString("XZ"));
    BOOST_CHECK_EQUAL( FaceDir::ZPlus + FaceDir::YPlus + FaceDir::ZMinus + FaceDir::YMinus, FaceDir::FromMULTREGTString("YZ"));
    BOOST_CHECK_EQUAL( FaceDir::ZPlus + FaceDir::XPlus + FaceDir::YPlus + FaceDir::ZMinus + FaceDir::XMinus + FaceDir::YMinus , FaceDir::FromMULTREGTString("XYZ"));

    BOOST_CHECK_THROW( FaceDir::FromString("??") , std::invalid_argument);
    BOOST_CHECK_THROW( FaceDir::FromString("x") , std::invalid_argument);
    BOOST_CHECK_THROW( FaceDir::FromString("ZY") , std::invalid_argument);
    BOOST_CHECK_THROW( FaceDir::FromString("YX") , std::invalid_argument);
}

}

