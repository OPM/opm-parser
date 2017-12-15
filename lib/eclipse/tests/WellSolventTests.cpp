/*
  Copyright 2015 IRIS
  
  This file is part of the Open Porous Media project (OPM).
  
  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BOOST_TEST_MODULE WellSolventTests

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Well.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>

using namespace Opm;

static Deck createDeckWithOutSolvent() {
    Opm::Parser parser;
    std::string input =
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   2   2  1*       \'OIL\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'WATER' 'OPEN' 'BHP' 1 2 3/\n/\n";

    return parser.parseString(input, ParseContext());
}

static Deck createDeckWithGasInjector() {
    Opm::Parser parser;
    std::string input =
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   1   1  1*       \'GAS\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'GAS' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n";

    return parser.parseString(input, ParseContext());
}

static Deck createDeckWithDynamicWSOLVENT() {
    Opm::Parser parser;
    std::string input =
            "START             -- 0 \n"
            "1 JAN 2000 / \n"
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   1   1  1*       \'GAS\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'GAS' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "DATES             -- 2\n"
            " 1  MAY 2000 / \n"
            "/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n"
            "DATES             -- 3,4\n"
            " 1  JUL 2000 / \n"
            " 1  AUG 2000 / \n"
            "/\n"
            "WSOLVENT\n"
            "     'W_1'        0 / \n "
            "/\n";

    return parser.parseString(input, ParseContext());
}

static Deck createDeckWithOilInjector() {
    Opm::Parser parser;
    std::string input =
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   2   2  1*       \'OIL\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'OIL' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n";

    return parser.parseString(input, ParseContext());
}

static Deck createDeckWithWaterInjector() {
    Opm::Parser parser;
    std::string input =
            "SCHEDULE\n"
            "WELSPECS\n"
            "     'W_1'        'OP'   2   2  1*       \'OIL\'  7* /   \n"
            "/\n"
            "COMPDAT\n"
            " 'W_1'  2*  1   1 'OPEN' / \n"
            "/\n"
            "WCONINJE\n"
            "     'W_1' 'WATER' 'OPEN' 'BHP' 1 2 3/\n/\n"
            "WSOLVENT\n"
            "     'W_1'        1 / \n "
            "/\n";

    return parser.parseString(input, ParseContext());
}
BOOST_AUTO_TEST_CASE(TestNoSolvent) {
    auto deck = createDeckWithOutSolvent();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    Eclipse3DProperties eclipseProperties ( deck , table, grid);
    Schedule schedule(deck, grid , eclipseProperties, Phases(true, true, true) , ParseContext());
    BOOST_CHECK(!deck.hasKeyword("WSOLVENT"));
}

BOOST_AUTO_TEST_CASE(TestGasInjector) {
    auto deck = createDeckWithGasInjector();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    Eclipse3DProperties eclipseProperties ( deck , table, grid);
    Schedule schedule(deck, grid , eclipseProperties, Phases(true, true, true) , ParseContext());
    BOOST_CHECK(deck.hasKeyword("WSOLVENT"));

}

BOOST_AUTO_TEST_CASE(TestDynamicWSOLVENT) {
    auto deck = createDeckWithDynamicWSOLVENT();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    Eclipse3DProperties eclipseProperties ( deck , table, grid);
    Schedule schedule(deck, grid , eclipseProperties, Phases(true, true, true) , ParseContext());
    BOOST_CHECK(deck.hasKeyword("WSOLVENT"));
    const auto& keyword = deck.getKeyword("WSOLVENT");
    BOOST_CHECK_EQUAL(keyword.size(),1);
    const auto& record = keyword.getRecord(0);
    const std::string& wellNamesPattern = record.getItem("WELL").getTrimmedString(0);
    auto wells_solvent = schedule.getWellsMatching(wellNamesPattern);
    BOOST_CHECK_EQUAL(wellNamesPattern, "W_1");
    BOOST_CHECK_EQUAL(wells_solvent[0]->getSolventFraction(0),0); //default 0
    BOOST_CHECK_EQUAL(wells_solvent[0]->getSolventFraction(1),1);
    BOOST_CHECK_EQUAL(wells_solvent[0]->getSolventFraction(2),1);
    BOOST_CHECK_EQUAL(wells_solvent[0]->getSolventFraction(3),0);
}

BOOST_AUTO_TEST_CASE(TestOilInjector) {
    auto deck = createDeckWithOilInjector();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    Eclipse3DProperties eclipseProperties ( deck , table, grid);
    BOOST_CHECK_THROW (Schedule(deck , grid , eclipseProperties, Phases(true, true, true) , ParseContext()), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(TestWaterInjector) {
    auto deck = createDeckWithWaterInjector();
    EclipseGrid grid(10,10,10);
    TableManager table ( deck );
    Eclipse3DProperties eclipseProperties ( deck , table, grid);
    BOOST_CHECK_THROW (Schedule(deck, grid , eclipseProperties, Phases(true, true, true) , ParseContext()), std::invalid_argument);
}
