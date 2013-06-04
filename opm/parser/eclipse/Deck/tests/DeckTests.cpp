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


#define BOOST_TEST_MODULE DeckTests

#include <stdexcept>
#include <boost/test/unit_test.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>

using namespace Opm;

BOOST_AUTO_TEST_CASE(Initialize) {
    BOOST_REQUIRE_NO_THROW(Deck deck);
    BOOST_REQUIRE_NO_THROW(DeckPtr deckPtr(new Deck()));
    BOOST_REQUIRE_NO_THROW(DeckConstPtr deckConstPtr(new Deck()));
}

BOOST_AUTO_TEST_CASE(hasKeyword_empty_returnFalse) {
    Deck deck;
    BOOST_CHECK_EQUAL(false, deck.hasKeyword("Bjarne"));
}

BOOST_AUTO_TEST_CASE(addKeyword_singlekeyword_keywordAdded) {
    Deck deck;
    DeckKWConstPtr keyword(new DeckKW("BJARNE"));
    BOOST_CHECK_NO_THROW(deck.addKeyword(keyword));
}

BOOST_AUTO_TEST_CASE(getKeyword_nosuchkeyword_throws) {
    Deck deck;
    BOOST_CHECK_THROW(deck.getKeyword("TRULS"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(getKeyword_singlekeyword_keywordreturned) {
    Deck deck;
    DeckKWConstPtr keyword(new DeckKW("BJARNE"));
    deck.addKeyword(keyword);
    BOOST_CHECK_EQUAL(keyword, deck.getKeyword("BJARNE"));
}




