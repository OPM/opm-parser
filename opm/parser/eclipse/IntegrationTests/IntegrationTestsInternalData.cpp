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

#define BOOST_TEST_MODULE ParserIntegrationTestsInternalData

#include <stdexcept>
#include <iostream>
#include <boost/filesystem.hpp>

#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParserKeyword.hpp>
#include <opm/parser/eclipse/RawDeck/RawDeck.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
using namespace Opm;

//NOTE: needs statoil dataset

BOOST_AUTO_TEST_CASE(ParseFileWithManyKeywords) {
    boost::filesystem::path multipleKeywordFile("testdata/statoil/gurbat_trimmed.DATA");

    ParserPtr parser(new Parser());
    RawDeckPtr rawDeck = parser->readToRawDeck(multipleKeywordFile.string());

    //This check is not necessarily correct, 
    //as it depends on that all the fixed recordNum keywords are specified
    BOOST_CHECK_EQUAL((unsigned) 275, rawDeck->size());
}
