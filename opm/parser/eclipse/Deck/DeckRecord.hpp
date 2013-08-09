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

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>

namespace Opm {

    class DeckRecord {
    public:
        DeckRecord();
        size_t size() const;
        void addItem(DeckItemConstPtr deckItem);
        DeckItemConstPtr getItem(size_t index) const;
        DeckItemConstPtr getItem(const std::string& name) const;

    private:
        std::vector<DeckItemConstPtr> m_items;
        std::map<std::string, DeckItemConstPtr> m_itemMap;

    };
    typedef boost::shared_ptr<DeckRecord> DeckRecordPtr;
    typedef boost::shared_ptr<const DeckRecord> DeckRecordConstPtr;

}
#endif  /* DECKRECORD_HPP */

