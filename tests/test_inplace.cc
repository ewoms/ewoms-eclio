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

#define BOOST_TEST_MODULE Inplace
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/output/inplace.hh>

using namespace Ewoms;

bool contains(const std::vector<Inplace::Phase>& phases, Inplace::Phase phase) {
    auto find_iter = std::find(phases.begin(), phases.end(), phase);
    return find_iter != phases.end();
}

BOOST_AUTO_TEST_CASE(TESTInplace) {
    Inplace oip;

    oip.add("FIPNUM", Inplace::Phase::OIL, 3, 100);
    oip.add("FIPNUM", Inplace::Phase::OIL, 6, 50);

    BOOST_CHECK_EQUAL( oip.get("FIPNUM", Inplace::Phase::OIL, 3) , 100);
    BOOST_CHECK_EQUAL( oip.get("FIPNUM", Inplace::Phase::OIL, 6) , 50);

    BOOST_CHECK_THROW( oip.get("FIPNUM", Inplace::Phase::OIL, 4), std::exception);
    BOOST_CHECK_THROW( oip.get("FIPNUM", Inplace::Phase::GAS, 3), std::exception);
    BOOST_CHECK_THROW( oip.get("FIPX", Inplace::Phase::OIL, 3)  , std::exception);

    BOOST_CHECK_EQUAL( oip.max_region(), 6);
    BOOST_CHECK_EQUAL( oip.max_region("FIPNUM"), 6);
    BOOST_CHECK_THROW( oip.max_region("FIPX"), std::exception);

    oip.add(Inplace::Phase::GAS, 100);
    BOOST_CHECK_EQUAL( oip.get(Inplace::Phase::GAS) , 100);
    BOOST_CHECK_THROW( oip.get(Inplace::Phase::OIL), std::exception);

    const auto& phases = Inplace::phases();
    BOOST_CHECK(contains(phases, Inplace::Phase::WATER));
    BOOST_CHECK(contains(phases, Inplace::Phase::OIL));
    BOOST_CHECK(contains(phases, Inplace::Phase::GAS));
    BOOST_CHECK(contains(phases, Inplace::Phase::OilInLiquidPhase));
    BOOST_CHECK(contains(phases, Inplace::Phase::OilInGasPhase));
    BOOST_CHECK(contains(phases, Inplace::Phase::GasInLiquidPhase));
    BOOST_CHECK(contains(phases, Inplace::Phase::GasInGasPhase));
    BOOST_CHECK(contains(phases, Inplace::Phase::PoreVolume));

    auto v1 = oip.get_vector("FIPNUM", Inplace::Phase::OIL);
    std::vector<double> e1 = {0,0,100,0,0,50};
    BOOST_CHECK( v1 == e1 );

}
