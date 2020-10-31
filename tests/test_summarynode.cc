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

#define BOOST_TEST_MODULE SummaryNode

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/io/summarynode.hh>

namespace {
    void expect_key(const Ewoms::EclIO::SummaryNode& node, const std::string& unique_key) {
        BOOST_CHECK_EQUAL(node.unique_key(), unique_key);
    }
}

BOOST_AUTO_TEST_SUITE(UniqueKey)

BOOST_AUTO_TEST_CASE(UniqueKey) {
    using Category = Ewoms::EclIO::SummaryNode::Category;
    using Type = Ewoms::EclIO::SummaryNode::Type;


    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Well, Type::Rate, std::string("NORA"), 1, std::string("") };
        expect_key(node, "KEYW:NORA"); }
    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Group,         Type::Rate, std::string("NORA"), 2 ,std::string("")};
        expect_key(node, "KEYW:NORA" ); };
    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Field,         Type::Rate, std::string("NORA"), 3 ,std::string("")};
        expect_key(node, "KEYW" ); };
    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Region,        Type::Rate, std::string("NORA"), 4 ,std::string("")};
        expect_key(node, "KEYW:4" ); };
    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Block,         Type::Rate, std::string("NORA"), 5 ,std::string("")};
        expect_key(node, "KEYW:5" ); };
    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Connection,    Type::Rate, std::string("NORA"), 6 ,std::string("")};
        expect_key(node, "KEYW:NORA:6" ); };
    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Segment,       Type::Rate, std::string("NORA"), 7 ,std::string("")};
        expect_key(node, "KEYW:NORA:7" ); };
    { Ewoms::EclIO::SummaryNode node = { "KEYW", Category::Miscellaneous, Type::Rate, std::string("NORA"), 8 ,std::string("")};
        expect_key(node, "KEYW" ); };
}

BOOST_AUTO_TEST_CASE(InjectedNumberRenderer) {
    using Category = Ewoms::EclIO::SummaryNode::Category;
    using Type = Ewoms::EclIO::SummaryNode::Type;

    Ewoms::EclIO::SummaryNode positiveNode = {
      "SIGN",
      Category::Region,
      Type::Undefined,
      std::string("-"),
      2,
      std::string("")
    };

    Ewoms::EclIO::SummaryNode negativeNode = {
      "SIGN",
      Category::Region,
      Type::Undefined,
      std::string("-"),
      -2,
      std::string("")
    };

    auto chooseSign = [](const Ewoms::EclIO::SummaryNode& node) -> std::string {
        return node.number > 0 ? "+" : "-";
    };

    BOOST_CHECK_EQUAL(positiveNode.unique_key(chooseSign), "SIGN:+");
    BOOST_CHECK_EQUAL(negativeNode.unique_key(chooseSign), "SIGN:-");
}

BOOST_AUTO_TEST_CASE(user_defined) {
    Ewoms::EclIO::SummaryNode summary_node = {"FU_VAR1", Ewoms::EclIO::SummaryNode::Category::Field, Ewoms::EclIO::SummaryNode::Type::Undefined, std::string(""), -1 , std::string("")};
    BOOST_CHECK( summary_node.is_user_defined() );
}

BOOST_AUTO_TEST_SUITE_END() // UniqueKey
