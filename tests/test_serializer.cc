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

    ser.put(int_value);
    ser.put(double_value);
    ser.put(string_value);
    ser.put(m);

    Ewoms::Serializer ser2(ser.buffer);
    BOOST_CHECK_EQUAL(ser2.get<int>(), int_value);
    BOOST_CHECK_EQUAL(ser2.get<double>(), double_value);
    BOOST_CHECK_EQUAL(ser2.get<std::string>(), string_value);

    std::unordered_map<std::string, int> m2 = ser2.get<std::string,int>();
    BOOST_CHECK(m2 == m);
}

BOOST_AUTO_TEST_CASE(EMPTY_STRING) {
    Ewoms::Serializer ser;
    ser.put(std::string{});
    BOOST_CHECK_THROW( ser.put(""), std::logic_error);

    Ewoms::Serializer ser2(ser.buffer);
    BOOST_CHECK_EQUAL(ser2.get<std::string>(), "");
}

