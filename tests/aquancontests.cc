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

#define BOOST_TEST_MODULE AquanconTest

#include <boost/test/unit_test.hpp>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/aquancon.hh>

using namespace Ewoms;

inline Deck createAQUANCONDeck_DEFAULT_INFLUX2() {
    const char *deckData =
        "DIMENS\n"
        "3 3 3 /\n"
        "\n"
        "GRID\n"
        "\n"
        "ACTNUM\n"
        " 0 8*1 0 8*1 0 8*1 /\n"
        "DXV\n"
        "1 1 1 /\n"
        "\n"
        "DYV\n"
        "1 1 1 /\n"
        "\n"
        "DZV\n"
        "1 1 1 /\n"
        "\n"
        "TOPS\n"
        "9*100 /\n"
        "\n"
        "PORO\n"
        "  27*0.15 /\n"
        "SOLUTION\n"
        "\n"
        "AQUANCON\n"
        "   1      2  2  1    1   1  1  J-  1.0 /\n"
        "   1      2  2  1    1   1  1  J-   /\n"
        "/ \n";

    Parser parser;
    return parser.parseString(deckData);
}

inline Deck createAQUANCONDeck_DEFAULT_INFLUX1() {
  const char *deckData =
    "DIMENS\n"
    "3 3 3 /\n"
    "\n"
    "GRID\n"
    "\n"
    "ACTNUM\n"
    " 0 8*1 0 8*1 0 8*1 /\n"
    "DXV\n"
    "1 1 1 /\n"
    "\n"
    "DYV\n"
    "1 1 1 /\n"
    "\n"
    "DZV\n"
    "1 1 1 /\n"
    "\n"
    "TOPS\n"
    "9*100 /\n"
    "PORO\n"
    "  27*0.15 /\n"
    "\n"
    "SOLUTION\n"
    "\n"
    "AQUANCON\n"
    "   1      1  1  1    1   1  1  J-   /\n"
    "   1      1  1  1    1   1  1  J-   /\n"
    "/ \n";

  Parser parser;
  return parser.parseString(deckData);
}

inline Deck createAQUANCONDeck() {
    const char *deckData =
        "DIMENS\n"
        "3 3 3 /\n"
        "\n"
        "GRID\n"
        "\n"
        "ACTNUM\n"
        " 0 8*1 0 8*1 0 8*1 /\n"
        "DXV\n"
        "1 1 1 /\n"
        "\n"
        "DYV\n"
        "1 1 1 /\n"
        "\n"
        "DZV\n"
        "1 1 1 /\n"
        "\n"
        "TOPS\n"
        "9*100 /\n"
        "\n"
        "PORO\n"
        "  27*0.15 /\n"
        "SOLUTION\n"
        "\n"
        "AQUANCON\n"
        "   1      1  1  1    1   1  1  J-  1.0 1.0 NO /\n"
        "   1      1  3  1    3   3  3  I+  0.5 1.0 NO /\n"
        "   1      1  3  1    3   3  3  J+  0.75 1.0 NO /\n"
        "   1      1  3  1    3   3  3  J-  2.75 1.0 NO /\n"
        "   1      2  3  2    3   1  1  I+  2.75 1.0 NO /\n"
        "/ \n";

    Parser parser;
    return parser.parseString(deckData);
}

inline std::vector<Aquancon::AquanconOutput> init_aquancon(){
    auto deck = createAQUANCONDeck();
    EclipseState eclState( deck );
    Aquancon aqucon( eclState.getInputGrid(), deck);
    std::vector<Aquancon::AquanconOutput> aquifers = aqucon.getAquOutput();

    return aquifers;
}

inline std::vector<Aquancon::AquanconOutput> fill_result(){
    auto deck = createAQUANCONDeck();
    EclipseState eclState( deck );
    Aquancon aqucon( eclState.getInputGrid(), deck);
    std::vector<Aquancon::AquanconOutput> aquifers = aqucon.getAquOutput();

    return aquifers;
}

BOOST_AUTO_TEST_CASE(AquanconTest){
    std::vector< Aquancon::AquanconOutput > aquifers = init_aquancon();
    std::vector< Aquancon::AquanconOutput > expected_output = fill_result();

    BOOST_CHECK_EQUAL(aquifers.size(), expected_output.size());
    for (size_t i = 0; i < aquifers.size(); ++i)
    {
        BOOST_CHECK_EQUAL_COLLECTIONS( aquifers.at(i).global_index.begin(), aquifers.at(i).global_index.end(),
                                   expected_output.at(i).global_index.begin(), expected_output.at(i).global_index.end() );
        BOOST_CHECK_EQUAL_COLLECTIONS( aquifers.at(i).influx_multiplier.begin(), aquifers.at(i).influx_multiplier.end(),
                                   expected_output.at(i).influx_multiplier.begin(), expected_output.at(i).influx_multiplier.end() );
        BOOST_CHECK_EQUAL_COLLECTIONS( aquifers.at(i).reservoir_face_dir.begin(), aquifers.at(i).reservoir_face_dir.end(),
                                   expected_output.at(i).reservoir_face_dir.begin(), expected_output.at(i).reservoir_face_dir.end() );
        BOOST_CHECK_EQUAL_COLLECTIONS( aquifers.at(i).record_index.begin(), aquifers.at(i).record_index.end(),
                                   expected_output.at(i).record_index.begin(), expected_output.at(i).record_index.end() );
    }
}

BOOST_AUTO_TEST_CASE(AquanconTest_DEFAULT_INFLUX) {
    auto deck1 = createAQUANCONDeck_DEFAULT_INFLUX1();
    EclipseState eclState1( deck1 );
    BOOST_CHECK_NO_THROW(Aquancon( eclState1.getInputGrid(), deck1));

    auto deck2 = createAQUANCONDeck_DEFAULT_INFLUX2();
    EclipseState eclState2( deck2 );
    BOOST_CHECK_THROW(Aquancon( eclState2.getInputGrid(), deck2), std::invalid_argument);
}

// allowing aquifer exists inside the reservoir
inline Deck createAQUANCONDeck_ALLOW_INSIDE_AQUAN_OR_NOT() {
    const char *deckData =
        "DIMENS\n"
        "3 3 3 /\n"
        "\n"
        "GRID\n"
        "\n"
        "ACTNUM\n"
        " 0 8*1 0 8*1 0 8*1 /\n"
        "DXV\n"
        "1 1 1 /\n"
        "\n"
        "DYV\n"
        "1 1 1 /\n"
        "\n"
        "DZV\n"
        "1 1 1 /\n"
        "\n"
        "TOPS\n"
        "9*100 /\n"
        "\n"
        "PORO\n"
        "  27*0.15 /\n"
        "SOLUTION\n"
        "\n"
        "AQUFETP\n"
        "  1 20.0 1000.0 2000. 0.000001 200.0 /\n"
        "  2 20.0 1000.0 2000. 0.000001 200.0 /\n"
        "/\n"
        "AQUANCON\n"
        "   1      1  1   1   1   1  1  J-  2* YES /\n"
        "   1      2  2   1   1   1  1  J-  2* YES /\n"
        "   1      2  2   2   2   1  1  J-  2* YES /\n"
        "   2      1  1   1   1   3  3  J-  2* NO /\n"
        "   2      2  2   1   1   3  3  J-  2* NO /\n"
        "   2      2  2   2   2   3  3  J-  2* NO /\n"
        "/ \n";

    Parser parser;
    return parser.parseString(deckData);
}

BOOST_AUTO_TEST_CASE(AquanconTest_ALLOW_AQUIFER_INSIDE_OR_NOT) {
    auto deck = createAQUANCONDeck_ALLOW_INSIDE_AQUAN_OR_NOT();
    const EclipseState eclState( deck );
    const Aquancon aqucon( eclState.getInputGrid(), deck);
    const std::vector<Aquancon::AquanconOutput>& aquifer_cons = aqucon.getAquOutput();
    BOOST_CHECK_EQUAL(aquifer_cons[0].global_index.size(), 2);
    BOOST_CHECK_EQUAL(aquifer_cons[1].global_index.size(), 1);
}
