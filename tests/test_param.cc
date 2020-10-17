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
#include <config.h>

#define NVERBOSE // to suppress our messages when throwing

#define BOOST_TEST_MODULE ParameterTest
#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/utility/parameters/parametergroup.hh>
#include <cstddef>
#include <sstream>
#include <vector>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE(commandline_syntax_init)
{
    typedef const char* cp;
    std::vector<cp> argv = { "program_command",
                             "topitem=somestring",
                             "/slashtopitem=anotherstring",
                             "/group/item=1",
                             "/group/anotheritem=2",
                             "/group/subgroup/item=3",
                             "/group/subgroup/anotheritem=4",
                             "/group/item=overridingstring",
                             0 };
    const std::size_t argc = argv.size() - 1;
    ParameterGroup p(argc, argv.data());
    BOOST_CHECK(p.get<std::string>("topitem") == "somestring");
    std::ostringstream os;
    p.writeParamToStream(os);
    std::string correct_answer = "/group/anotheritem=2\n"
        "/group/item=overridingstring\n"
        "/group/subgroup/anotheritem=4\n"
        "/group/subgroup/item=3\n"
        "/slashtopitem=anotherstring\n"
        "/topitem=somestring\n";
    BOOST_CHECK(os.str() == correct_answer);
    BOOST_CHECK(p.unhandledArguments().empty());

    // Tests that only run in debug mode.
#ifndef NDEBUG
#endif
}

BOOST_AUTO_TEST_CASE(xml_syntax_init)
{
    typedef const char* cp;
    std::vector<cp> argv = { "program_command",
                             "testdata.param",
                             "/group/item=overridingstring",
                             "unhandledargument",
                             0};
    const std::size_t argc = argv.size() - 1;
    ParameterGroup p(argc, argv.data(), false);
    BOOST_CHECK(p.get<std::string>("topitem") == "somestring");
    std::ostringstream os;
    p.writeParamToStream(os);
    std::string correct_answer = "/group/anotheritem=2\n"
        "/group/item=overridingstring\n"
        "/group/subgroup/anotheritem=4\n"
        "/group/subgroup/item=3\n"
        "/slashtopitem=anotherstring\n"
        "/topitem=somestring\n";
    BOOST_CHECK(os.str() == correct_answer);
    BOOST_REQUIRE(p.unhandledArguments().size() == 1);
    BOOST_CHECK_EQUAL(p.unhandledArguments()[0], "unhandledargument");
    // Tests that only run in debug mode.
#ifndef NDEBUG
#endif
}

BOOST_AUTO_TEST_CASE(failing_strict_xml_syntax_init)
{
    typedef const char* cp;
    std::vector<cp> argv = { "program_command",
                             "testdata.param",
                             "/group/item=overridingstring",
                             "unhandledargument",
                             0 };
    const std::size_t argc = argv.size() - 1;
    BOOST_CHECK_THROW(ParameterGroup p(argc, argv.data()), std::runtime_error);
}
