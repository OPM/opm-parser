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

#define BOOST_TEST_MODULE ParserIntegrationTests
#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/Parser/Deck.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>

#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Well.hpp>
#include <opm/parser/eclipse/Units/Units.hpp>

using namespace Opm;


BOOST_AUTO_TEST_CASE( parse_WCONPROD_OK ) {
    Parser parser;
    std::string wconprodFile("testdata/integration_tests/WellWithWildcards/WCONPROD1");
    auto deck =  parser.parseFile(wconprodFile, ParseContext());
    EclipseGrid grid(30,30,30);
    Schedule sched(ParseContext(), grid, deck, Phases(true, true, true) );

    BOOST_CHECK_EQUAL(5U, sched.numWells());
    BOOST_CHECK(sched.hasWell("INJE1"));
    BOOST_CHECK(sched.hasWell("PROD2"));
    BOOST_CHECK(sched.hasWell("PROD3"));
    BOOST_CHECK(sched.hasWell("PROD4"));
    BOOST_CHECK(sched.hasWell("PROX5"));

    {
        auto* well = sched.getWell("PROD2");
        BOOST_CHECK_CLOSE(1000/Metric::Time, well->getProductionProperties(0).OilRate, 0.001);
        BOOST_CHECK_CLOSE(1500/Metric::Time, well->getProductionProperties(1).OilRate, 0.001);
    }

    {
        auto* well = sched.getWell("PROD3");
        BOOST_CHECK_CLOSE(0/Metric::Time, well->getProductionProperties(0).OilRate, 0.001);
        BOOST_CHECK_CLOSE(1500/Metric::Time, well->getProductionProperties(1).OilRate, 0.001);
    }

    {
        auto* well = sched.getWell("PROX5");
        BOOST_CHECK_CLOSE(2000/Metric::Time, well->getProductionProperties(0).OilRate, 0.001);
        BOOST_CHECK_CLOSE(2000/Metric::Time, well->getProductionProperties(1).OilRate, 0.001);
    }
}


BOOST_AUTO_TEST_CASE( parse_WCONINJE_OK ) {
    ParseContext parseContext;
    Parser parser;
    std::string wconprodFile("testdata/integration_tests/WellWithWildcards/WCONINJE1");
    auto deck = parser.parseFile(wconprodFile, parseContext);
    EclipseGrid grid(30,30,30);
    Schedule sched( parseContext, grid, deck, Phases(true, true, true) );

    BOOST_CHECK_EQUAL(5U, sched.numWells());
    BOOST_CHECK(sched.hasWell("PROD1"));
    BOOST_CHECK(sched.hasWell("INJE2"));
    BOOST_CHECK(sched.hasWell("INJE3"));
    BOOST_CHECK(sched.hasWell("PROD4"));
    BOOST_CHECK(sched.hasWell("INJX5"));

    {
        auto* well = sched.getWell("INJE2");
        BOOST_CHECK_CLOSE(1000/Metric::Time, well->getInjectionProperties(0).surfaceInjectionRate, 0.001);
        BOOST_CHECK_CLOSE(1500/Metric::Time, well->getInjectionProperties(1).surfaceInjectionRate, 0.001);
    }

    {
        auto* well = sched.getWell("INJE3");
        BOOST_CHECK_CLOSE(0/Metric::Time, well->getInjectionProperties(0).surfaceInjectionRate, 0.001);
        BOOST_CHECK_CLOSE(1500/Metric::Time, well->getInjectionProperties(1).surfaceInjectionRate, 0.001);
    }

    {
        auto* well = sched.getWell("INJX5");
        BOOST_CHECK_CLOSE(2000/Metric::Time, well->getInjectionProperties(0).surfaceInjectionRate, 0.001);
        BOOST_CHECK_CLOSE(2000/Metric::Time, well->getInjectionProperties(1).surfaceInjectionRate, 0.001);
    }
}
