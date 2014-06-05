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

#include <vector>
#include <iostream>

#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>

namespace Opm {
    Deck::Deck() {
        m_parser = 0;
        m_keywords = KeywordContainerPtr(new KeywordContainer());
    }

    Deck::Deck(const Parser* parser) {
        m_parser = parser;
        m_keywords = KeywordContainerPtr(new KeywordContainer());
    }

    bool Deck::hasKeyword(const std::string& keywordName) const {
        if (m_parser && !m_parser->canParseKeyword(keywordName))
            throw std::logic_error("Queried for unrecognized keyword '" + keywordName + "'");

        return m_keywords->hasKeyword(keywordName);
    }
    
    void Deck::addKeyword( DeckKeywordPtr keyword) {
        m_keywords->addKeyword(keyword);
    }
    
    DeckKeywordPtr Deck::getKeyword(const std::string& keyword, size_t index) const {
        return m_keywords->getKeyword(keyword , index);
    }

    DeckKeywordPtr Deck::getKeyword(const std::string& keyword) const {
        return m_keywords->getKeyword(keyword);
    }
    
    DeckKeywordPtr Deck::getKeyword(size_t index) const {
        return m_keywords->getKeyword(index);
    }
      
    size_t Deck::numKeywords(const std::string& keyword) {
        return m_keywords->numKeywords( keyword );
    }

    const std::vector<DeckKeywordPtr>& Deck::getKeywordList(const std::string& keyword) {
        return m_keywords->getKeywordList( keyword );
    }

    size_t Deck::size() const {
        return m_keywords->size();
    }

    size_t Deck::numWarnings() const {
        return m_warnings.size();
    }

    void Deck::addWarning(const std::string& warningText , const std::string& filename , size_t lineNR) {
        std::pair<std::string,size_t> location(filename , lineNR);
        std::pair<std::string , std::pair<std::string,size_t> > warning(warningText , location);

        m_warnings.push_back( warning );
    }

    const std::pair<std::string , std::pair<std::string,size_t> >& Deck::getWarning( size_t index ) const {
        if (index < m_warnings.size())
            return m_warnings[index];
        else
            throw std::invalid_argument("Index out of range");
    }
    
    
    void Deck::initUnitSystem() {
        m_defaultUnits = std::shared_ptr<UnitSystem>( UnitSystem::newMETRIC() );
        if (hasKeyword("FIELD"))
            m_activeUnits = std::shared_ptr<UnitSystem>( UnitSystem::newFIELD() );
        else
            m_activeUnits = m_defaultUnits;
    }

    std::shared_ptr<UnitSystem> Deck::getActiveUnitSystem() const {
        return m_activeUnits;
    }
    

    std::shared_ptr<UnitSystem> Deck::getDefaultUnitSystem() const {
        return m_defaultUnits;
    }

}

