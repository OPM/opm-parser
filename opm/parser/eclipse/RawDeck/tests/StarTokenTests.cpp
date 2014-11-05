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

#define BOOST_TEST_MODULE ParserTests
#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include <opm/parser/eclipse/RawDeck/StarToken.hpp>
#include <opm/parser/eclipse/Parser/ParserLog.hpp>

BOOST_AUTO_TEST_CASE(NoStar) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::StarToken st("Hei...", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);
}


BOOST_AUTO_TEST_CASE(InvalidCount) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::StarToken st("X*", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::StarToken st("1.25*", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::StarToken st("-3*", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::StarToken st("0*", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::StarToken st("*123", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);
}


BOOST_AUTO_TEST_CASE(CountCorrect) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    Opm::StarToken st1("*", parserLog, "", -1);
    Opm::StarToken st2("5*", parserLog, "", -1);
    Opm::StarToken st3("54*", parserLog, "", -1);
    BOOST_CHECK(st1.countString() == "");
    BOOST_CHECK(st2.countString() == "5");
    BOOST_CHECK(st3.countString() == "54");

    BOOST_CHECK(st1.valueString() == "");
    BOOST_CHECK(st2.valueString() == "");
    BOOST_CHECK(st3.valueString() == "");

    BOOST_CHECK(!st1.hasValue());
    BOOST_CHECK(!st2.hasValue());
    BOOST_CHECK(!st3.hasValue());

    BOOST_CHECK_EQUAL(1U , st1.count());
    BOOST_CHECK_EQUAL(5U , st2.count());
    BOOST_CHECK_EQUAL(54U , st3.count());
}


BOOST_AUTO_TEST_CASE(NoValueGetValueThrow) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    Opm::StarToken st1("*", parserLog, "", -1);
    Opm::StarToken st2("5*", parserLog, "", -1);
    BOOST_CHECK_EQUAL( false , st1.hasValue());
    BOOST_CHECK_EQUAL( false , st2.hasValue());
}

BOOST_AUTO_TEST_CASE(StarNoCountThrows) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    parserLog->clear();
    BOOST_CHECK_NO_THROW(Opm::StarToken st("*10", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);
}


BOOST_AUTO_TEST_CASE(CorrectValueString) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    Opm::StarToken st1("1*10.09", parserLog, "", -1);
    Opm::StarToken st2("5*20.13", parserLog, "", -1);
    Opm::StarToken st3("1*'123'", parserLog, "", -1);
    Opm::StarToken st4("1*123*456", parserLog, "", -1);
    BOOST_CHECK_EQUAL( true , st1.hasValue());
    BOOST_CHECK_EQUAL( true , st2.hasValue());
    BOOST_CHECK_EQUAL( true , st3.hasValue());
    BOOST_CHECK_EQUAL( true , st4.hasValue());

    BOOST_CHECK_EQUAL( "10.09" , st1.valueString());
    BOOST_CHECK_EQUAL( "20.13" , st2.valueString());
    BOOST_CHECK_EQUAL( "'123'" , st3.valueString());
    BOOST_CHECK_EQUAL( "123*456" , st4.valueString());
}

BOOST_AUTO_TEST_CASE( ContainsStar_WithStar_ReturnsTrue ) {
    std::string countString, valueString;
    BOOST_CHECK_EQUAL( true , Opm::isStarToken("*", countString, valueString) );
    BOOST_CHECK_EQUAL( true , Opm::isStarToken("*1", countString, valueString) );
    BOOST_CHECK_EQUAL( true , Opm::isStarToken("1*", countString, valueString) );
    BOOST_CHECK_EQUAL( true , Opm::isStarToken("1*2", countString, valueString) );
    
    BOOST_CHECK_EQUAL( false , Opm::isStarToken("12", countString, valueString) );
    BOOST_CHECK_EQUAL( false , Opm::isStarToken("'12*34'", countString, valueString) );
}

BOOST_AUTO_TEST_CASE( readValueToken_basic_validity_tests ) {
    Opm::ParserLogPtr parserLog(new Opm::ParserLog);

    parserLog->clear();
    BOOST_CHECK_NO_THROW( Opm::readValueToken<int>("3.3", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    parserLog->clear();
    BOOST_CHECK_NO_THROW( Opm::readValueToken<double>("truls", parserLog, "", -1));
    BOOST_CHECK_EQUAL(parserLog->size(), 1);

    BOOST_CHECK_EQUAL("3.3", Opm::readValueToken<std::string>("3.3", parserLog, "", -1));
    BOOST_CHECK_EQUAL("OLGA", Opm::readValueToken<std::string>("OLGA", parserLog, "", -1));
    BOOST_CHECK_EQUAL("OLGA", Opm::readValueToken<std::string>("'OLGA'", parserLog, "", -1));
    BOOST_CHECK_EQUAL("123*456", Opm::readValueToken<std::string>("123*456", parserLog, "", -1));
    BOOST_CHECK_EQUAL("123*456", Opm::readValueToken<std::string>("'123*456'", parserLog, "", -1));
}
