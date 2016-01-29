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

#ifndef DECKRECORD_HPP
#define DECKRECORD_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/Iterator.hpp>

namespace Opm {

    class DeckItem;

    class DeckRecord {
    public:

        typedef iterator_base< DeckItem > iterator;
        typedef iterator_base< const DeckItem, DeckItem > const_iterator;

        DeckRecord();
        size_t size() const;
        void addItem(std::shared_ptr< DeckItem > deckItem);
        std::shared_ptr< DeckItem > getItem(size_t index) const;
        std::shared_ptr< DeckItem > getItem(const std::string& name) const;
        std::shared_ptr< DeckItem > getDataItem() const;
        bool        hasItem(const std::string& name) const;

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;
        
    template <class Item>
    std::shared_ptr< DeckItem > getItem() const {
        return getItem( Item::itemName );
    }


    private:
        std::vector<std::shared_ptr< DeckItem >> m_items;
        std::map<std::string, std::shared_ptr< DeckItem >> m_itemMap;

    };
    typedef std::shared_ptr<DeckRecord> DeckRecordPtr;
    typedef std::shared_ptr<const DeckRecord> DeckRecordConstPtr;

}
#endif  /* DECKRECORD_HPP */

