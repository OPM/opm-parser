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


#include <stdexcept>
#include <string>

#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>


namespace Opm {

    DeckRecord::DeckRecord() {

    }

    size_t DeckRecord::size() const {
        return m_items.size();
    }

    void DeckRecord::addItem(DeckItemPtr deckItem) {
        if (m_itemMap.find(deckItem->name()) == m_itemMap.end()) {
            m_itemMap[deckItem->name()] = deckItem;
            m_items.push_back(deckItem);
        } else
            throw std::invalid_argument("Item with name: " + deckItem->name() + " already exists in DeckRecord");
    }

    DeckItemPtr DeckRecord::getItem(size_t index) const {
        if (index < m_items.size())
            return m_items[index];
        else
            throw std::range_error("Index out of range.");
    }


    bool DeckRecord::hasItem(const std::string& name) const {
        if (m_itemMap.find(name) == m_itemMap.end())
            return false;
        else
            return true;
    }

    
    DeckItemPtr DeckRecord::getItem(const std::string& name) const {
        if (hasItem(name))
            return m_itemMap.find(name)->second;
        else
            throw std::invalid_argument("Itemname: " + name + " does not exist.");
    }


    DeckItemPtr DeckRecord::getDataItem() const {
        if (m_items.size() == 1)
            return getItem(0);
        else
            throw std::range_error("Not a data keyword ?");
    }

    DeckRecord::iterator DeckRecord::begin() {
        return DeckRecord::iterator( this->m_items.begin() );
    }

    DeckRecord::iterator DeckRecord::end() {
        return DeckRecord::iterator( this->m_items.end() );
    }

    DeckRecord::const_iterator DeckRecord::begin() const {
        return DeckRecord::const_iterator( 
                const_cast< DeckRecord* >( this )->m_items.begin()
            );
    }

    DeckRecord::const_iterator DeckRecord::end() const {
        return DeckRecord::const_iterator( 
                const_cast< DeckRecord* >( this )->m_items.end()
            );
    }

    DeckRecord::const_iterator DeckRecord::cbegin() const {
        return this->begin();
    }

    DeckRecord::const_iterator DeckRecord::cend() const {
        return this->end();
    }
}
