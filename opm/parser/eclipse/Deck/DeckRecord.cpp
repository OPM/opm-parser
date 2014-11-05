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

#include <opm/parser/eclipse/Deck/DeckRecord.hpp>


namespace Opm {

    DeckRecord::DeckRecord(ParserLogPtr parserLog, const std::string& fileName, int lineNumber)
        : m_parserLog(parserLog),
          m_fileName(fileName),
          m_lineNumber(lineNumber) {
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

    DeckItemPtr DeckRecord::getItem(const std::string& name) const {
        if (m_itemMap.find(name) == m_itemMap.end())
            throw std::invalid_argument("Itemname: " + name + " does not exist.");
        else
            return m_itemMap.find(name)->second;
    }


    DeckItemPtr DeckRecord::getDataItem() const {
        if (m_items.size() == 1)
            return getItem(0);
        else
            throw std::range_error("Not a data keyword ?");
    }

    ParserLogPtr DeckRecord::getParserLog() const {
        return m_parserLog;
    }

    const std::string& DeckRecord::getFileName() const {
        return m_fileName;
    }

    int DeckRecord::getLineNumber() const {
        return m_lineNumber;
    }
}


