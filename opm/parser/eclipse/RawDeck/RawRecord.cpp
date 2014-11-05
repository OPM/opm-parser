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
#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include <opm/parser/eclipse/RawDeck/RawRecord.hpp>
#include <opm/parser/eclipse/RawDeck/RawConsts.hpp>

using namespace Opm;
using namespace std;

namespace Opm {

    /*
     * It is assumed that after a record is terminated, there is no quote marks
     * in the subsequent comment. This is in accordance with the Eclipse user
     * manual.
     * 
     * If a "non-complete" record string is supplied, an invalid_argument
     * exception is thrown.
     * 
     */
    RawRecord::RawRecord(const std::string& singleRecordString, ParserLogPtr parserLog, const std::string& fileName, int lineNumber, const std::string& keywordName)
        : m_parserLog(parserLog), m_fileName(fileName), m_lineNumber(lineNumber), m_keywordName(keywordName){
        if (isTerminatedRecordString(singleRecordString)) {
            setRecordString(singleRecordString);
            splitSingleRecordString();
        } else {
            parserLog->addError(m_fileName, m_lineNumber,
                                "Input string is not a complete record string.");
        }
    }
    
    const std::string& RawRecord::getFileName() const {
        return m_fileName;
    }

    int RawRecord::getLineNumber() const {
        return m_lineNumber;
    }

    const std::string& RawRecord::getKeywordName() const {
        return m_keywordName;
    }

    ParserLogPtr RawRecord::getParserLog() const {
        return m_parserLog;
    }

    std::string RawRecord::pop_front() {
        std::string front = m_recordItems.front();
        m_recordItems.pop_front();
        return front;
    }


    void RawRecord::push_front(std::string token) {
        m_recordItems.push_front( token );
    }


    size_t RawRecord::size() const {
        return m_recordItems.size();
    }

    void RawRecord::dump() const {
        std::cout << "RecordDump: ";
        for (size_t i = 0; i < m_recordItems.size(); i++)
            std::cout << m_recordItems[i] << "/" << getItem(i) << " ";
        std::cout << std::endl;
    }


    const std::string& RawRecord::getItem(size_t index) const {
        if (index < m_recordItems.size())
            return m_recordItems[index];
        else
            throw std::out_of_range("Lookup index out of range");
    }

    const std::string& RawRecord::getRecordString() const {
        return m_sanitizedRecordString;
    }

    bool RawRecord::isTerminatedRecordString(const std::string& candidateRecordString) {
        unsigned int terminatingSlash = findTerminatingSlash(candidateRecordString);
        bool hasTerminatingSlash = (terminatingSlash < candidateRecordString.size());
        int numberOfQuotes = std::count(candidateRecordString.begin(), candidateRecordString.end(), RawConsts::quote);
        bool hasEvenNumberOfQuotes = (numberOfQuotes % 2) == 0;
        return hasTerminatingSlash && hasEvenNumberOfQuotes;
    }

    void RawRecord::splitSingleRecordString() {
        char currentChar;
        char tokenStartCharacter=' ';
        std::string currentToken = "";
        bool inQuote = false;
        for (unsigned i = 0; i < m_sanitizedRecordString.size(); i++) {
            currentChar = m_sanitizedRecordString[i];
            if (!inQuote && charIsSeparator(currentChar)) {
                processSeparatorCharacter(currentToken, currentChar, tokenStartCharacter);
            } else if (currentChar == RawConsts::quote) {
                inQuote = !inQuote;
                processNonSpecialCharacters(currentToken, currentChar);
            } else {
                processNonSpecialCharacters(currentToken, currentChar);
            }
        }
        if (currentToken.size() > 0) {
            m_recordItems.push_back(currentToken);
            currentToken.clear();
        }
    }

    void RawRecord::processSeparatorCharacter(std::string& currentToken, const char& currentChar, char& tokenStartCharacter) {
        if (tokenStartCharacter == RawConsts::quote) {
            currentToken += currentChar;
        } else {
            if (currentToken.size() > 0) {
                m_recordItems.push_back(currentToken);
                currentToken.clear();
            }
            tokenStartCharacter = currentChar;
        }
    }

    void RawRecord::processNonSpecialCharacters(std::string& currentToken, const char& currentChar) {
        currentToken += currentChar;
    }

    bool RawRecord::charIsSeparator(char candidate) {
        return std::string::npos != RawConsts::separators.find(candidate);
    }

    void RawRecord::setRecordString(const std::string& singleRecordString) {
        unsigned terminatingSlash = findTerminatingSlash(singleRecordString);
        m_sanitizedRecordString = singleRecordString.substr(0, terminatingSlash);
        boost::trim(m_sanitizedRecordString);
    }

    unsigned int RawRecord::findTerminatingSlash(const std::string& singleRecordString) {
        unsigned int terminatingSlash = singleRecordString.find_first_of(RawConsts::slash);
        unsigned int lastQuotePosition = singleRecordString.find_last_of(RawConsts::quote);

        // Checks lastQuotePosition vs terminatingSlashPosition, 
        // since specifications of WELLS, FILENAMES etc can include slash, but 
        // these are always in quotes (and there are no quotes after record-end).
        if (terminatingSlash < lastQuotePosition && lastQuotePosition < singleRecordString.size()) {
            terminatingSlash = singleRecordString.find_first_of(RawConsts::slash, lastQuotePosition);
        }
        return terminatingSlash;
    }

    RawRecord::~RawRecord() {
    }


}
