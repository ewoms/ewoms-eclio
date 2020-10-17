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

#define NVERBOSE // to suppress our messages when throwing

#define BOOST_TEST_MODULE UniformTableLinearTests
#include <boost/test/unit_test.hpp>
#include <ewoms/eclio/utility/numeric/uniformtablelinear.hh>

BOOST_AUTO_TEST_CASE(table_operations)
{
    // Make a simple table.
    double yva[] = { 1.0, -1.0, 3.0, 4.0, 2.0 };
    const int numvals = sizeof(yva)/sizeof(yva[0]);
    std::vector<double> yv(yva, yva + numvals);
    const double xmin = 1.0;
    const double xdelta = 2.5;
    const double xmax = xmin + (numvals - 1)*xdelta;
    Ewoms::utils::UniformTableLinear<double> t1(xmin, xmax, yv);
    Ewoms::utils::UniformTableLinear<double> t1_copy1(1.0, 11.0, yv);
    Ewoms::utils::UniformTableLinear<double> t1_copy2(t1);

    // Check equality.
    BOOST_CHECK(t1 == t1_copy1);
    BOOST_CHECK(t1 == t1_copy2);

    // Check some evaluations.
    for (int i = 0; i < numvals; ++i) {
        BOOST_CHECK_EQUAL(t1(xmin + i*xdelta), yv[i]);
    }
    BOOST_CHECK_EQUAL(t1(2.25), 0.0);
    BOOST_CHECK_EQUAL(t1(9.75), 3.0);
    BOOST_CHECK_EQUAL(t1.derivative(9.75), -2.0/xdelta);
    // Until we implement anything but the ClosestValue end policy, we only test that.
    BOOST_CHECK_EQUAL(t1(xmin - 1.0), yv[0]);
    BOOST_CHECK_EQUAL(t1(xmax + 1.0), yv.back());

    // Domains.
    BOOST_CHECK_EQUAL(t1.domain().first, xmin);
    BOOST_CHECK_EQUAL(t1.domain().second, xmin + (numvals-1)*xdelta);
    std::pair<double, double> new_domain(-100.0, 20.0);
    t1.rescaleDomain(new_domain);
    BOOST_CHECK_EQUAL(t1.domain().first, new_domain.first);
    BOOST_CHECK_EQUAL(t1.domain().second, new_domain.second);
    for (int i = 0; i < numvals; ++i) {
        BOOST_CHECK_EQUAL(t1(-100.0 + i*120.0/(double(numvals - 1))), yv[i]);
    }
    BOOST_CHECK_EQUAL(t1(-85.0), 0.0);
    BOOST_CHECK(std::fabs(t1.derivative(0.0)  + 2.0/30.0) < 1e-14);
}
