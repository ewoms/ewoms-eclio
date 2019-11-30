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

#define BOOST_TEST_MODULE MESSAGELIMITER_TESTS

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/opmlog/messagelimiter.hh>
#include <ewoms/eclio/opmlog/logutil.hh>

using namespace Ewoms;

BOOST_AUTO_TEST_CASE(ConstructionAndLimits)
{
    MessageLimiter m1;
    BOOST_CHECK_EQUAL(m1.tagMessageLimit(), MessageLimiter::NoLimit);
    MessageLimiter m2(0);
    BOOST_CHECK_EQUAL(m2.tagMessageLimit(), 0);
    MessageLimiter m3(1);
    BOOST_CHECK_EQUAL(m3.tagMessageLimit(), 1);
    MessageLimiter m4(-4);
    BOOST_CHECK_EQUAL(m4.tagMessageLimit(), MessageLimiter::NoLimit);
}

BOOST_AUTO_TEST_CASE(TagResponse)
{
    using namespace Ewoms;
    {
        // No limits.
        MessageLimiter m;
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 6);
    }

    {
        // Limit == 0.
        MessageLimiter m(0);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 0);
    }

    {
        // Limit == 1.
        MessageLimiter m(1);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 2);
    }
}

BOOST_AUTO_TEST_CASE(CategoryResponse)
{
    using namespace Ewoms;
    {
        // No limits.
        MessageLimiter m;
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 3);
    }

    {
        // Limit == 0.
        MessageLimiter m(MessageLimiter::NoLimit,
                         {{ Log::MessageType::Info, 0 }});
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::JustOverCategoryLimit);
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::OverCategoryLimit);
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::OverCategoryLimit);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 3);
    }

    {
        // Limit == 1.
        MessageLimiter m(MessageLimiter::NoLimit,
                         {{ Log::MessageType::Info, 1 }});
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::JustOverCategoryLimit);
        BOOST_CHECK(m.handleMessageLimits("", Log::MessageType::Info) == MessageLimiter::Response::OverCategoryLimit);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 3);
    }
}

BOOST_AUTO_TEST_CASE(MixedResponse)
{
    using namespace Ewoms;
    {
        // Tag Limit == 1. Category limit = 0.
        MessageLimiter m(1, {{ Log::MessageType::Info, 0 }});
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::JustOverCategoryLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverCategoryLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 2);
    }

    {
        // Tag Limit == 0. Category limit = 1.
        MessageLimiter m(0, {{ Log::MessageType::Info, 1 }});
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 0);
    }

    {
        // Tag Limit == 1. Category limit = 1.
        MessageLimiter m(1, {{ Log::MessageType::Info, 1 }});
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::PrintMessage);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::JustOverCategoryLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::JustOverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag1", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.handleMessageLimits("tag2", Log::MessageType::Info) == MessageLimiter::Response::OverTagLimit);
        BOOST_CHECK(m.categoryMessageCounts().at(Log::MessageType::Info) == 2);
    }

}
