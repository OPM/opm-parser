/*
  Copyright 2015 Statoil ASA.

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



#define BOOST_TEST_MODULE SimulationConfigTests

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/SimulationConfig.hpp>


using namespace Opm;

const std::string& inputStr = "RUNSPEC\n"
                              "EQLOPTS\n"
                              "THPRES /\n "
                              "\n"

                              "SOLUTION\n"
                              "THPRES\n"
                              "1 2 12.0/\n"
                              "1 3 5.0/\n"
                              "2 3 7.0/\n"
                              "/\n"
                              "\n";


static DeckPtr createDeck(const std::string& input) {
    Opm::Parser parser;
    return parser.parseString(input);
}



BOOST_AUTO_TEST_CASE(SimulationConfigTest) {

    DeckPtr deck = createDeck(inputStr);
    LoggerPtr logger(new Logger());
    int maxEqlnum = 3;

    BOOST_CHECK_NO_THROW(std::make_shared<const SimulationConfig>(deck, maxEqlnum, logger));
}


BOOST_AUTO_TEST_CASE(SimulationConfigGetThresholdPressureTableTest) {
    DeckPtr deck = createDeck(inputStr);
    LoggerPtr logger(new Logger());
    int maxEqlnum = 3;

    SimulationConfigConstPtr simulationConfigPtr = std::make_shared<const SimulationConfig>(deck, maxEqlnum, logger);

    const std::vector<double>& thresholdPressureTable = simulationConfigPtr->getThresholdPressureTable();

    double pressureList[] = {0.0, 1200000.0, 500000.0, 1200000.0, 0.0, 700000.0, 500000.0, 700000.0, 0.0};
    std::vector<double> wantedResultVec(pressureList, pressureList + sizeof(pressureList) / sizeof(double));

    BOOST_CHECK_EQUAL(thresholdPressureTable.size(), wantedResultVec.size());
    BOOST_CHECK(std::equal(thresholdPressureTable.begin(), thresholdPressureTable.end(), wantedResultVec.begin()));
}

