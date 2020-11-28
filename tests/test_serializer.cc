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

#define BOOST_TEST_MODULE "Serializer"
#include <boost/test/unit_test.hpp>
#include <string>
#include <unordered_map>

#include <ewoms/eclio/utility/serializer.hh>

BOOST_AUTO_TEST_CASE(SERIALIZER) {
    Ewoms::Serializer ser;

    int int_value = 100;
    double double_value = 3.14;
    std::string string_value = "String";
    std::unordered_map<std::string, int> m = {{"A", 1}, {"B", 2}, {"C", 3}};
    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};

    ser.put(int_value);
    ser.put(double_value);
    ser.put(string_value);
    ser.put_map(m);
    ser.put_vector(v);

    Ewoms::Serializer ser2(ser.buffer);
    BOOST_CHECK_EQUAL(ser2.get<int>(), int_value);
    BOOST_CHECK_EQUAL(ser2.get<double>(), double_value);
    BOOST_CHECK_EQUAL(ser2.get<std::string>(), string_value);

    std::unordered_map<std::string, int> m2 = ser2.get_map<std::string,int>();
    BOOST_CHECK(m2 == m);

    std::vector<int> v2 = ser2.get_vector<int>();
    BOOST_CHECK(v2 == v);
}

BOOST_AUTO_TEST_CASE(EMPTY_STRING) {
    Ewoms::Serializer ser;
    ser.put(std::string{});
    BOOST_CHECK_THROW( ser.put(""), std::logic_error);

    Ewoms::Serializer ser2(ser.buffer);
    BOOST_CHECK_EQUAL(ser2.get<std::string>(), "");
}

