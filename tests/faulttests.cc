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

#define BOOST_TEST_MODULE FaultTests

#include <boost/test/unit_test.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ewoms/eclio/parser/eclipsestate/grid/faultcollection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fault.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/faultface.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/facedir.hh>

BOOST_AUTO_TEST_CASE(CreateInvalidFace) {
    // I out of range
    BOOST_CHECK_THROW( Ewoms::FaultFace(10,10,10,10 , 10 , 1 , 1 , 5 , 5 , Ewoms::FaceDir::XPlus) , std::invalid_argument );

    // I1 != I2 when face == X
    BOOST_CHECK_THROW( Ewoms::FaultFace( 10,10,10, 1 , 3  , 1 , 1 , 5 , 5 , Ewoms::FaceDir::XPlus) , std::invalid_argument );

    // J1 < J2
    BOOST_CHECK_THROW( Ewoms::FaultFace(  10,10,10,3 , 3  , 3 , 1 , 5 , 5 , Ewoms::FaceDir::XPlus) , std::invalid_argument );

}

BOOST_AUTO_TEST_CASE(CreateFace) {
    Ewoms::FaultFace face1(10,10,10,0, 2  , 0 , 0 , 0 , 0 , Ewoms::FaceDir::YPlus);
    Ewoms::FaultFace face2(10,10,10,0, 2  , 1 , 1 , 0 , 0 , Ewoms::FaceDir::YPlus);
    Ewoms::FaultFace face3(10,10,10,0, 2  , 0 , 0 , 1 , 1 , Ewoms::FaceDir::YPlus);

    std::vector<size_t> trueValues1{0,1,2};
    std::vector<size_t> trueValues2{10,11,12};
    std::vector<size_t> trueValues3{100,101,102};
    size_t i = 0;

    {
        auto iter3 = face3.begin();
        auto iter2 = face2.begin();
        for (auto iter1 = face1.begin(); iter1 != face1.end(); ++iter1) {
            size_t index1 = *iter1;
            size_t index2 = *iter2;
            size_t index3 = *iter3;

            BOOST_CHECK_EQUAL( index1 , trueValues1[i] );
            BOOST_CHECK_EQUAL( index2 , trueValues2[i] );
            BOOST_CHECK_EQUAL( index3 , trueValues3[i] );

            ++iter2;
            ++iter3;
            ++i;
        }
    }
    BOOST_CHECK_EQUAL( face1.getDir() , Ewoms::FaceDir::YPlus);
}

BOOST_AUTO_TEST_CASE(CreateFault) {
    Ewoms::Fault fault("FAULT1");
    BOOST_CHECK_EQUAL( "FAULT1" , fault.getName());
    BOOST_CHECK_EQUAL( 1.0 , fault.getTransMult());
}

namespace Ewoms {

inline std::ostream& operator<<( std::ostream& stream, const FaultFace& face ) {
    stream << face.getDir() << ": [ ";
    for( auto index : face ) stream << index << " ";
    return stream << "]";
}

}

BOOST_AUTO_TEST_CASE(AddFaceToFaults) {
    Ewoms::Fault fault("FAULT1");
    Ewoms::FaultFace face1( 10,10,10,0, 2  , 0 , 0 , 0 , 0 , Ewoms::FaceDir::YPlus );
    Ewoms::FaultFace face2( 10,10,10,0, 2  , 1 , 1 , 0 , 0 , Ewoms::FaceDir::YPlus );
    Ewoms::FaultFace face3( 10,10,10,0, 2  , 0 , 0 , 1 , 1 , Ewoms::FaceDir::YPlus );
    fault.addFace( face1 );
    fault.addFace( face2 );
    fault.addFace( face3 );

    {
        auto iter = fault.begin();
        BOOST_CHECK_EQUAL( *iter , face1 ); ++iter;
        BOOST_CHECK_EQUAL( *iter , face2 ); ++iter;
        BOOST_CHECK_EQUAL( *iter , face3 ); ++iter;
    }

}

BOOST_AUTO_TEST_CASE(CreateFaultCollection) {
    Ewoms::FaultCollection faults;
    BOOST_CHECK_EQUAL( faults.size() , 0U );
    BOOST_CHECK(! faults.hasFault("NO-NotThisOne"));
    BOOST_CHECK_THROW( faults.getFault("NO") , std::invalid_argument );
}

BOOST_AUTO_TEST_CASE(AddFaultsToCollection) {
    Ewoms::FaultCollection faults;

    faults.addFault("FAULT");
    BOOST_CHECK_EQUAL( faults.size() , 1U );
    BOOST_CHECK(faults.hasFault("FAULT"));

    const auto& fault1 = faults.getFault("FAULT");
    const auto& fault2 = faults.getFault(0);
    BOOST_CHECK_EQUAL(fault1.getName(), fault2.getName());

    faults.addFault("FAULTX");
    const auto& faultx = faults.getFault("FAULTX");
    BOOST_CHECK_EQUAL( faults.size() , 2U );
    BOOST_CHECK(faults.hasFault("FAULTX"));
    BOOST_CHECK_EQUAL( faultx.getName() , faults.getFault(1).getName());
}
