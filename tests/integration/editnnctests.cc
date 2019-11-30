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

#include <ewoms/eclio/parser/eclipsestate/edit/editnnc.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/units/units.hh>

#define BOOST_TEST_MODULE NNCTests

#include <boost/test/unit_test.hpp>

using namespace Ewoms;

inline std::string pathprefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

BOOST_AUTO_TEST_CASE(noNNC)
{
    Parser parser;
    auto deck = parser.parseFile(pathprefix() + "NNC/noNNC.DATA");
    EclipseState eclipseState(deck);
    const auto& editnnc = eclipseState.getInputEDITNNC();
    BOOST_CHECK(!eclipseState.hasInputEDITNNC());
    BOOST_CHECK(editnnc.empty());
}

BOOST_AUTO_TEST_CASE(readDeck)
{
    Parser parser;
    auto deck = parser.parseFile(pathprefix() + "EDITNNC/EDITNNC.DATA");
    EclipseState eclipseState(deck);
    const auto& editnnc = eclipseState.getInputEDITNNC();
    BOOST_CHECK(!editnnc.empty());
    const std::vector<NNCdata>& data = editnnc.data();

    // test the NNCs in nnc.DATA
    BOOST_CHECK_EQUAL(editnnc.size(), 3); //neighbouring connections in EDITNNC are ignored
    BOOST_CHECK_EQUAL(data[0].cell1, 1);
    BOOST_CHECK_EQUAL(data[0].cell2, 21);
    BOOST_CHECK_EQUAL(data[0].trans, 0.1);
    BOOST_CHECK_EQUAL(data[1].cell1, 1);
    BOOST_CHECK_EQUAL(data[1].cell2, 21);
    BOOST_CHECK_EQUAL(data[1].trans, 0.1);
    BOOST_CHECK_EQUAL(data[2].cell1, 2);
    BOOST_CHECK_EQUAL(data[2].cell2, 0);
    BOOST_CHECK_EQUAL(data[2].trans, 0.1);

}
