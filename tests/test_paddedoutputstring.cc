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

#define BOOST_TEST_MODULE Padded_Output_String

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/io/paddedoutputstring.hh>
#include <ewoms/common/string.hh>

// Convenience alias.
template <std::size_t N>
using PadString = ::Ewoms::EclIO::PaddedOutputString<N>;

// =====================================================================

BOOST_AUTO_TEST_SUITE(PadString8)

BOOST_AUTO_TEST_CASE (Basic_Operations)
{
    // Default Constructor
    {
        const auto s = PadString<8>{};

        BOOST_CHECK_EQUAL(s.c_str(), std::string(8, ' '));
    }

    // Construct from Constant String
    {
        const auto s = PadString<8>{"Inj-1"};

        BOOST_CHECK_EQUAL(s.c_str(), std::string{"Inj-1   "});
    }

    // Copy Construction
    {
        const auto s1 = PadString<8>{"Inj-1"};
        const auto s2 = s1;

        BOOST_CHECK_EQUAL(s2.c_str(), std::string{"Inj-1   "});
    }

    // Move Construction
    {
        auto s1 = PadString<8>{"Inj-1"};
        const auto s2 = std::move(s1);

        BOOST_CHECK_EQUAL(s2.c_str(), std::string{"Inj-1   "});
    }

    // Assignment Operator
    {
        const auto s1 = PadString<8>{"Inj-1"};
        auto s2 = PadString<8>{"Prod-2"};

        s2 = s1;
        BOOST_CHECK_EQUAL(s2.c_str(), std::string{"Inj-1   "});
    }

    // Move Assignment Operator
    {
        auto s1 = PadString<8>{"Inj-1"};
        auto s2 = PadString<8>{"Prod-2"};

        s2 = std::move(s1);
        BOOST_CHECK_EQUAL(s2.c_str(), std::string{"Inj-1   "});
    }

    // Assign std::string
    {
        auto s = PadString<8>{"@Hi Hoo@"};

        s = "Prod-2";
        BOOST_CHECK_EQUAL(s.c_str(), std::string{"Prod-2  "});
    }
}

BOOST_AUTO_TEST_CASE (String_Shortening)
{
    // Construct from string of more than N characters
    {
        const auto s = PadString<10>{
            "String too long"
        };

        BOOST_CHECK_EQUAL(s.c_str(), std::string{"String too"});
    }

    // Assign string of more than N characters
    {
        auto s = PadString<11>{};

        s = "This string has too many characters";

        BOOST_CHECK_EQUAL(s.c_str(), std::string{"This string"});
    }
}

BOOST_AUTO_TEST_CASE (Trim) {
    const auto s1 = PadString<4>{"X"};
    BOOST_CHECK_EQUAL(Ewoms::trim_copy(s1), std::string{"X"});

    const auto s2 = PadString<4>{"ABCD"};
    BOOST_CHECK_EQUAL(Ewoms::trim_copy(s2), std::string{"ABCD"});

    const auto s3 = PadString<4>{""};
    BOOST_CHECK_EQUAL(Ewoms::trim_copy(s3), std::string{""});
}

BOOST_AUTO_TEST_SUITE_END()
