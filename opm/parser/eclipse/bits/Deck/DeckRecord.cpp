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


#include <unordered_set>
#include <stdexcept>
#include <string>
#include <algorithm>

#include <opm/parser/eclipse/bits/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/bits/Deck/DeckRecord.hpp>


namespace Opm {


    DeckRecord::DeckRecord( std::vector< DeckItem >&& items ) :
        m_items( std::move( items ) ) {

        std::unordered_set< std::string > names;
        for( const auto& item : this->m_items )
            names.insert( item.name() );

        if( names.size() == this->m_items.size() )
            return;

        names.clear();
        std::string msg = "Duplicate item names in DeckRecord:";
        for( const auto& item : this->m_items ) {
            if( names.count( item.name() ) != 0 )
                msg += std::string( " " ) += item.name();

            names.insert( item.name() );
        }

        throw std::invalid_argument( msg );
    }

    size_t DeckRecord::size() const {
        return m_items.size();
    }

    void DeckRecord::addItem( DeckItem deckItem ) {
        if( this->hasItem( deckItem.name() ) )
            throw std::invalid_argument(
                    "Item with name: "
                    + deckItem.name()
                    + " already exists in DeckRecord");

        m_items.push_back( std::move( deckItem ) );
    }

    DeckItem& DeckRecord::getItem( size_t index ) {
        return this->m_items.at( index );
    }

    DeckItem& DeckRecord::getItem(const std::string& name) {
        const auto eq = [&name]( const DeckItem& e ) {
            return e.name() == name;
        };

        auto item = std::find_if( m_items.begin(), m_items.end(), eq );

        if( item == m_items.end() )
            throw std::invalid_argument("Item: " + name + " does not exist.");

        return *item;
    }

    DeckItem& DeckRecord::getDataItem() {
        if (m_items.size() == 1)
            return getItem(0);
        else
            throw std::range_error("Not a data keyword ?");
    }

    const DeckItem& DeckRecord::getItem( size_t index ) const {
        return this->m_items.at( index );
    }

    const DeckItem& DeckRecord::getItem(const std::string& name) const {
        const auto eq = [&name]( const DeckItem& e ) {
            return e.name() == name;
        };

        auto item = std::find_if( this->begin(), this->end(), eq );

        if( item == m_items.end() )
            throw std::invalid_argument("Item: " + name + " does not exist.");

        return *item;
    }

    const DeckItem& DeckRecord::getDataItem() const {
        if (m_items.size() == 1)
            return getItem(0);
        else
            throw std::range_error("Not a data keyword ?");
    }

    bool DeckRecord::hasItem(const std::string& name) const {
        const auto eq = [&name]( const DeckItem& e ) {
            return e.name() == name;
        };

        return std::any_of( this->begin(), this->end(), eq );
    }

    DeckRecord::const_iterator DeckRecord::begin() const {
        return this->m_items.begin();
    }

    DeckRecord::const_iterator DeckRecord::end() const {
        return this->m_items.end();
    }

}
