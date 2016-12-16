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

#include <iostream>
#include <exception>
#include <algorithm>
#include <cassert>
#include <set>
#include <string>

#include <opm/parser/eclipse/bits/Deck/Deck.hpp>
#include <opm/parser/eclipse/bits/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/bits/Deck/Section.hpp>
#include <opm/parser/eclipse/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParserKeyword.hpp>

namespace Opm {

    static bool isSectionDelimiter( const DeckKeyword& keyword ) {
        const auto& name = keyword.name();
        for( const auto& x : { "RUNSPEC", "GRID", "EDIT", "PROPS",
                               "REGIONS", "SOLUTION", "SUMMARY", "SCHEDULE" } )
            if( name == x ) return true;

        return false;
    }

    static std::pair< DeckView::const_iterator, DeckView::const_iterator >
    find_section( const Deck& deck, const std::string& keyword ) {

        const auto fn = [&keyword]( const DeckKeyword& kw ) {
            return kw.name() == keyword;
        };

        auto first = std::find_if( deck.begin(), deck.end(), fn );

        if( first == deck.end() )
            return { first, first };

        auto last = std::find_if( first + 1, deck.end(), isSectionDelimiter );

        if( last != deck.end() && last->name() == keyword )
            throw std::invalid_argument( std::string( "Deck contains the '" ) + keyword + "' section multiple times" );

        return { first, last };
    }

    Section::Section( const Deck& deck, const std::string& section )
        : DeckView( find_section( deck, section ) ),
          section_name( section ),
          units( deck.getActiveUnitSystem() )
    {}

    const std::string& Section::name() const {
        return this->section_name;
    }

    const UnitSystem& Section::unitSystem() const {
        return this->units;
    }

    bool Section::hasRUNSPEC(const Deck& deck) { return deck.hasKeyword( "RUNSPEC" ); }
    bool Section::hasGRID(const Deck& deck) { return deck.hasKeyword( "GRID" ); }
    bool Section::hasEDIT(const Deck& deck) { return deck.hasKeyword( "EDIT" ); }
    bool Section::hasPROPS(const Deck& deck) { return deck.hasKeyword( "PROPS" ); }
    bool Section::hasREGIONS(const Deck& deck) { return deck.hasKeyword( "REGIONS" ); }
    bool Section::hasSOLUTION(const Deck& deck) { return deck.hasKeyword( "SOLUTION" ); }
    bool Section::hasSUMMARY(const Deck& deck) { return deck.hasKeyword( "SUMMARY" ); }
    bool Section::hasSCHEDULE(const Deck& deck) { return deck.hasKeyword( "SCHEDULE" ); }

}
