/*
  Copyright 2014 Andreas Lauser

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
#include "checkDeck.hpp"

#include <opm/parser/eclipse/Parser/ParserKeyword.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>

namespace Opm {
bool checkDeck(DeckConstPtr deck, LoggerPtr logger, size_t enabledChecks) {
    bool deckValid = true;

    // make sure that the deck does not contain unknown keywords
    if (enabledChecks & UnknownKeywords) {
        size_t keywordIdx = 0;
        for (; keywordIdx < deck->size(); keywordIdx++) {
            const auto keyword = deck->getKeyword(keywordIdx);
            if (!keyword->hasParserKeyword()) {
                std::string msg("Keyword '" + keyword->name() + "' is unknown.");
                logger->addWarning(keyword->getFileName(), keyword->getLineNumber(), msg);
                deckValid = false;
            }
        }
    }

    // make sure that the sizes of the tables have been specified correctly
    if (enabledChecks & TableSizes) {
        size_t keywordIdx = 0;
        for (; keywordIdx < deck->size(); keywordIdx++) {
            const auto keyword = deck->getKeyword(keywordIdx);
            if (!keyword->hasParserKeyword())
                continue;

            const auto parserKeyword = keyword->getParserKeyword();
            if (parserKeyword->getSizeType() != OTHER_KEYWORD_IN_DECK)
                continue;

            const auto& sizeDefPair = parserKeyword->getSizeDefinitionPair();
            const std::string& sizeKeywordName = sizeDefPair.first;
            if (!deck->hasKeyword(sizeKeywordName)) {
                std::string msg("Deck does not feature the keyword '" + sizeKeywordName +
                                "' which specifies the size of keyword " + keyword->name() + ".");
                logger->addWarning(keyword->getFileName(), keyword->getLineNumber(), msg);
                deckValid = false;
                continue;
            }

            const auto sizeKeyword = deck->getKeyword(sizeKeywordName);
            const std::string& sizeItemName = sizeDefPair.second;
            DeckItemConstPtr deckItem;
            try {
                deckItem = sizeKeyword->getRecord(0)->getItem(sizeItemName);
            }
            catch (const std::invalid_argument& e) {
                // this error is quite bad: it is an incorrect (or at least inconsistent)
                // keyword definition. throwing an exception here does not seem
                // appropriate, though...
                std::string msg("Keyword '" + sizeKeywordName +
                                "' does not contain an item named '" + sizeItemName +
                                "' which is specified as the size of keyword " + keyword->name() + ".");
                logger->addError(keyword->getFileName(), keyword->getLineNumber(), msg);
                logger->addError(keyword->getFileName(), keyword->getLineNumber(),
                                 "(this is an error in opm-parser, please create send a bug "
                                 "report to opm@opm-project.org)" );
                deckValid = false;
                continue;
            }

            int kwMaxSize = sizeKeyword->getRecord(0)->getItem(sizeItemName)->getInt(0);
            if (static_cast<int>(keyword->size()) > kwMaxSize) {
                std::string msg("Keyword '" + keyword->name() +
                                "' has " + std::to_string((long long) keyword->size()) +
                                " items but at most " + std::to_string((long long) kwMaxSize) +
                                " are allowed by the " + sizeItemName + " item of keyword " +
                                sizeKeywordName + ".");
                logger->addWarning(keyword->getFileName(), keyword->getLineNumber(), msg);
                deckValid = false;
                continue;
            }
        }

    }

    // make sure all mandatory sections are present and that their order is correct
    if (enabledChecks & SectionTopology) {
        bool ensureKeywordSection = enabledChecks & KeywordSection;
        deckValid = deckValid && Section::checkSectionTopology(deck, logger, ensureKeywordSection);
    }

    return deckValid;
}
}
