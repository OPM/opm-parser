/*
  Copyright 2013 Statoil ASA.

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

#include <config.h>
#if HAVE_DYNAMIC_BOOST_TEST
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE ResinsightIntegrationTests
#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/F.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/G.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/S.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridDims.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/FaultCollection.hpp>

using namespace Opm;

inline std::string pathprefix() {
    return boost::unit_test::framework::master_test_suite().argv[1];
}

BOOST_AUTO_TEST_CASE( test_parse ) {
    Parser parser(false);
    ParseContext parseContext;

    parseContext.update( ParseContext::PARSE_UNKNOWN_KEYWORD , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_TEXT , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_SLASH , InputError::IGNORE );

    parser.addKeyword<ParserKeywords::SPECGRID>();
    parser.addKeyword<ParserKeywords::FAULTS>();

    auto deck = parser.parseFile(pathprefix() + "Resinsight/DECK1.DATA" , parseContext);

    BOOST_CHECK( deck.hasKeyword<ParserKeywords::SPECGRID>() );
    BOOST_CHECK( deck.hasKeyword<ParserKeywords::FAULTS>() );

    BOOST_CHECK_NO_THROW( GridDims{deck} );
    BOOST_CHECK_NO_THROW( GridDims{deck}.getCartesianSize() );
}


BOOST_AUTO_TEST_CASE( test_state ) {
    Parser parser(false);
    ParseContext parseContext;

    parseContext.update( ParseContext::PARSE_UNKNOWN_KEYWORD , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_TEXT , InputError::IGNORE );
    parseContext.update( ParseContext::PARSE_RANDOM_SLASH , InputError::IGNORE );

    parser.addKeyword<ParserKeywords::SPECGRID>();
    parser.addKeyword<ParserKeywords::FAULTS>();
    parser.addKeyword<ParserKeywords::GRID>();
    auto deck = parser.parseFile(pathprefix() + "Resinsight/DECK1.DATA" , parseContext);

    GridDims grid(deck);
    GRIDSection gsec(deck);
    FaultCollection faults(gsec, grid);
    BOOST_CHECK_EQUAL( grid.getNX(), 20U);
    BOOST_CHECK_EQUAL( grid.getNY(), 20U);
    BOOST_CHECK_EQUAL( grid.getNZ(), 10U);
    BOOST_CHECK_EQUAL( faults.size(), 2U);
}
