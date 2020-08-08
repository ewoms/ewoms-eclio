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

#define BOOST_TEST_MODULE RawKeywordTests
#include <cstring>
#include <stdexcept>
#include <boost/test/unit_test.hpp>

#include "ewoms/eclio/parser/rawdeck/rawenums.hh"
#include "ewoms/eclio/parser/rawdeck/rawkeyword.hh"
#include "ewoms/eclio/parser/rawdeck/rawrecord.hh"

using namespace Ewoms;

BOOST_AUTO_TEST_CASE(RawKeywordConstructor) {
    BOOST_CHECK_THROW( RawKeyword("NAME", "file", 10, false, Raw::FIXED), std::logic_error);
    BOOST_CHECK_THROW( RawKeyword("NAME", "file", 10, false, Raw::TABLE_COLLECTION), std::logic_error);
    BOOST_CHECK_THROW( RawKeyword("NAME", "file", 10, false, Raw::TABLE_COLLECTION, 0), std::logic_error);
    BOOST_CHECK_THROW( RawKeyword("NAME", "file", 10, false, Raw::SLASH_TERMINATED, 5), std::logic_error);
    BOOST_CHECK_THROW( RawKeyword("NAME", "file", 10, false, Raw::UNKNOWN, 5), std::logic_error);
    BOOST_CHECK_THROW( RawKeyword("NAME", "file", 10, false, Raw::CODE, 2), std::logic_error);
    RawKeyword kw1("NAME", "file", 10, false, Raw::SLASH_TERMINATED);
    RawKeyword kw2("NAME", "file", 10, false, Raw::FIXED, 10);
    RawKeyword kw3("NAME", "file", 10, false, Raw::TABLE_COLLECTION, 7);
    RawKeyword kw4("NAME", "file", 10, false, Raw::CODE);
}

BOOST_AUTO_TEST_CASE(IsFinished) {
    std::string storage = "RecordString";
    Ewoms::string_view line(storage);
    RawRecord rec(line);

    RawKeyword kw1("NAME", "file", 10, false, Raw::FIXED, 0);
    BOOST_CHECK(kw1.isFinished());

    {
        RawKeyword kw2("NAME", "file", 10, false, Raw::FIXED, 2);
        BOOST_CHECK(!kw2.isFinished());

        BOOST_CHECK(!kw2.addRecord(rec));
        BOOST_CHECK(!kw2.isFinished());

        BOOST_CHECK(kw2.addRecord(rec));
        BOOST_CHECK(kw2.isFinished());
    }

    {
        RawKeyword kw("NAME", "file", 10, false, Raw::CODE);
        BOOST_CHECK(!kw.isFinished());

        BOOST_CHECK(kw.addRecord(rec));
        BOOST_CHECK(kw.isFinished());
    }

    {
        RawKeyword kw3("NAME", "file", 10, false, Raw::TABLE_COLLECTION, 2);
        BOOST_CHECK(!kw3.isFinished());

        BOOST_CHECK(!kw3.terminateKeyword());
        BOOST_CHECK(!kw3.isFinished());

        BOOST_CHECK(kw3.terminateKeyword());
        BOOST_CHECK(kw3.isFinished());
    }

    {
        RawKeyword kw4("NAME", "file", 10, false, Raw::SLASH_TERMINATED);
        BOOST_CHECK(!kw4.isFinished());

        BOOST_CHECK(kw4.terminateKeyword());
        BOOST_CHECK(kw4.isFinished());
    }

    {
        RawKeyword kw5("NAME", "file", 10, false, Raw::UNKNOWN);
        BOOST_CHECK(!kw5.isFinished());

        BOOST_CHECK(kw5.terminateKeyword());
        BOOST_CHECK(kw5.isFinished());
    }
}

