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



#define BOOST_TEST_MODULE ThresholdPressureTests

#include <algorithm>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/ThresholdPressure.hpp>


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

const std::string& inputStrNoSolutionSection =  "RUNSPEC\n"
                                                "EQLOPTS\n"
                                                "THPRES /\n "
                                                "\n";


const std::string& inputStrIrrevers = "RUNSPEC\n"
                                      "EQLOPTS\n"
                                      "THPRES IRREVERS/\n "
                                      "\n"

                                      "SOLUTION\n"
                                      "THPRES\n"
                                      "/\n"
                                      "\n";

const std::string& inputStrInconsistency =  "RUNSPEC\n"
                                            "EQLOPTS\n"
                                            "THPRES /\n "
                                            "\n"

                                            "SOLUTION\n"
                                            "\n";

const std::string& inputStrTooHighRegionNumbers = "RUNSPEC\n"
                                                  "EQLOPTS\n"
                                                  "THPRES /\n "
                                                  "\n"

                                                  "SOLUTION\n"
                                                  "THPRES\n"
                                                  "1 2 12.0/\n"
                                                  "4 3 5.0/\n"
                                                  "2 3 7.0/\n"
                                                  "/\n"
                                                  "\n";


const std::string& inputStrMissingData = "RUNSPEC\n"
                                         "EQLOPTS\n"
                                         "THPRES /\n "
                                         "\n"

                                         "SOLUTION\n"
                                         "THPRES\n"
                                         "1 2 12.0/\n"
                                         "4 3 5.0/\n"
                                         "2 3 /\n"
                                         "/\n"
                                         "\n";



static DeckPtr createDeck(const std::string& input) {
    Opm::Parser parser;
    return parser.parseString(input);
}



BOOST_AUTO_TEST_CASE(ThresholdPressureTest) {

    DeckPtr deck = createDeck(inputStr);
    int maxEqlnum = 3;

    ThresholdPressureConstPtr tresholdPressurePtr = std::make_shared<ThresholdPressure>(deck, maxEqlnum);
    const std::vector<double>& thresholdPressureTable = tresholdPressurePtr->getThresholdPressureTable();

    double pressureList[] = {0.0, 1200000.0, 500000.0, 1200000.0, 0.0, 700000.0, 500000.0, 700000.0, 0.0};
    std::vector<double> wantedResultVec(pressureList, pressureList + sizeof(pressureList) / sizeof(double));

    BOOST_CHECK_EQUAL(thresholdPressureTable.size(), wantedResultVec.size());
    BOOST_CHECK(std::equal(thresholdPressureTable.begin(), thresholdPressureTable.end(), wantedResultVec.begin()));
}


BOOST_AUTO_TEST_CASE(ThresholdPressureEmptyTest) {

    DeckPtr deck = createDeck(inputStrNoSolutionSection);
    int maxEqlnum = 3;

    ThresholdPressureConstPtr tresholdPressurePtr = std::make_shared<ThresholdPressure>(deck, maxEqlnum);
    const std::vector<double>& thresholdPressureTable = tresholdPressurePtr->getThresholdPressureTable();

    BOOST_CHECK_EQUAL(0, thresholdPressureTable.size());
}


BOOST_AUTO_TEST_CASE(ThresholdPressureThrowTest) {

    DeckPtr deck_irrevers      = createDeck(inputStrIrrevers);
    DeckPtr deck_inconsistency = createDeck(inputStrInconsistency);
    DeckPtr deck_highRegNum    = createDeck(inputStrTooHighRegionNumbers);
    DeckPtr deck_missingData   = createDeck(inputStrMissingData);
    int maxEqlnum = 3;

    BOOST_CHECK_THROW(std::make_shared<ThresholdPressure>(deck_irrevers, maxEqlnum), std::runtime_error);
    BOOST_CHECK_THROW(std::make_shared<ThresholdPressure>(deck_inconsistency, maxEqlnum), std::runtime_error);
    BOOST_CHECK_THROW(std::make_shared<ThresholdPressure>(deck_highRegNum, maxEqlnum), std::runtime_error);
    BOOST_CHECK_THROW(std::make_shared<ThresholdPressure>(deck_missingData, maxEqlnum), std::runtime_error);
}
