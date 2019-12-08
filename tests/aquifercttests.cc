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

#define BOOST_TEST_MODULE AquiferCTTest

#include <boost/test/unit_test.hpp>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/aquiferct.hh>

using namespace Ewoms;

inline Deck createAquiferCTDeck() {
    const char *deckData =
        "DIMENS\n"
        "3 3 3 /\n"
        "\n"
        "AQUDIMS\n"
        "1* 1* 2 100 1 1000 /\n"
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
        "PROPS\n"
        "AQUTAB\n"
        " 0.01 0.112 \n"
        " 0.05 0.229 /\n"
        "SOLUTION\n"
        "\n"
        "AQUCT\n"
        "   1 2000.0 1.5 100 .3 3.0e-5 330 10 360.0 1 2 /\n"
        "/ \n";

    Parser parser;
    return parser.parseString(deckData);
}

inline Deck createAquiferCTDeckDefaultP0() {
    const char *deckData =
        "DIMENS\n"
        "3 3 3 /\n"
        "\n"
        "AQUDIMS\n"
        "1* 1* 2 100 1 1000 /\n"
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
        "PROPS\n"
        "AQUTAB\n"
        " 0.01 0.112 \n"
        " 0.05 0.229 /\n"
        "SOLUTION\n"
        "\n"
        "AQUCT\n"
        "   1 2000.0 1* 100 .3 3.0e-5 330 10 360.0 1 2 /\n"
        "/ \n";

    Parser parser;
    return parser.parseString(deckData);
}

inline std::vector<AquiferCT::AQUCT_data> init_aquiferct(Deck& deck){
    EclipseState eclState( deck );
    AquiferCT aquct( eclState, deck);
    std::vector<AquiferCT::AQUCT_data> aquiferct = aquct.getAquifers();

    return aquiferct;
}

BOOST_AUTO_TEST_CASE(AquiferCTTest){
    auto deck = createAquiferCTDeck();
    std::vector< AquiferCT::AQUCT_data > aquiferct = init_aquiferct(deck);
    for (const auto& it : aquiferct){
        BOOST_CHECK_EQUAL(it.aquiferID , 1);
        BOOST_CHECK_EQUAL(it.phi_aq , 0.3);
        BOOST_CHECK_EQUAL(it.inftableID , 2);
        BOOST_CHECK_CLOSE(*(it.p0), 1.5e5, 1e-6);
    }

    auto deck_default_p0 = createAquiferCTDeckDefaultP0();
    aquiferct = init_aquiferct(deck_default_p0);
    for (const auto& it : aquiferct){
        BOOST_CHECK_EQUAL(it.aquiferID , 1);
        BOOST_CHECK_EQUAL(it.phi_aq , 0.3);
        BOOST_CHECK_EQUAL(it.inftableID , 2);
        BOOST_CHECK(it.p0 == nullptr);
    }
}
