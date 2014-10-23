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

#ifndef STAR_TOKEN_HPP
#define STAR_TOKEN_HPP

#include <opm/parser/eclipse/Parser/ParserLog.hpp>

#include <string>
#include <iostream>

#include <boost/lexical_cast.hpp>

namespace Opm {
    bool isStarToken(const std::string& token,
                     std::string& countString,
                     std::string& valueString);

    template <class T>
    T readValueToken(const std::string& valueString,
                     ParserLogPtr parserLog,
                     const std::string& fileName,
                     int lineNumber)
    {
        try {
            return boost::lexical_cast<T>(valueString);
        }
        catch (boost::bad_lexical_cast& e) {
            std::string msg("Unable to parse string" + valueString + ": " + e.what());
            parserLog->addWarning(fileName, lineNumber, msg);
            return T();
        }
    }

    template <>
    inline std::string readValueToken<std::string>(const std::string& valueString,
                                                   ParserLogPtr parserLog,
                                                   const std::string& fileName,
                                                   int lineNumber)
    {
        if (valueString.size() > 0 && valueString[0] == '\'') {
            if (valueString.size() < 2 || valueString[valueString.size() - 1] != '\'') {
                std::string msg("Unable to parse string '" + valueString + "' as a string token");
                parserLog->addWarning(fileName, lineNumber, msg);
                return valueString;
            }
            return valueString.substr(1, valueString.size() - 2);
        }
        else
            return valueString;
    }


class StarToken {
public:
    StarToken(const std::string& token,
              ParserLogPtr parserLog,
              const std::string& fileName,
              int lineNumber)
    {
        if (!isStarToken(token, m_countString, m_valueString)) {
            parserLog->addWarning(fileName,
                                  lineNumber,
                                  "Token \""+token+"\" is not a repetition specifier");
            return;
        }
        init_(token, parserLog, fileName, lineNumber);
    }

    StarToken(const std::string& token,
              const std::string& countStr,
              const std::string& valueStr,
              ParserLogPtr parserLog,
              const std::string& fileName,
              int lineNumber)
        : m_countString(countStr)
        , m_valueString(valueStr)
    {
        init_(token, parserLog, fileName, lineNumber);
    }

    size_t count() const {
        return m_count;
    }

    bool hasValue() const {
        return !m_valueString.empty();
    }

    // returns the coubt as rendered in the deck. note that this might be different
    // than just converting the return value of count() to a string because an empty
    // count is interpreted as 1...
    const std::string& countString() const {
        return m_countString;
    }

    // returns the value as rendered in the deck. note that this might be different
    // than just converting the return value of value() to a string because values
    // might have different representations in the deck (e.g. strings can be
    // specified with and without quotes and but spaces are only allowed using the
    // first representation.)
    const std::string& valueString() const {
        return m_valueString;
    }

private:
    // internal initialization method. the m_countString and m_valueString attributes
    // must be set before calling this method.
    void init_(const std::string& token,
               ParserLogPtr parserLog,
               const std::string& fileName,
               int lineNumber) {
        // special-case the interpretation of a lone star as "1*" but do not
        // allow constructs like "*123"...
        if (m_countString == "") {
            if (m_valueString != "")
                parserLog->addWarning(fileName,
                                      lineNumber,
                                      "Not specifying a repetition count should imply not specifying a value. Token \'" + token + "\'.");
            else
                parserLog->addWarning(fileName,
                                      lineNumber,
                                      "Not specifying a repetion count for '*' is explicitly forbidden by the Eclipse documentation.");

            // TODO: since this is explicitly forbidden by the documentation it might
            // be a good idea to decorate the deck with a warning?
            m_count = 1;
        }
        else {
            m_count = boost::lexical_cast<int>(m_countString);

            if (m_count == 0) {
                parserLog->addWarning(fileName,
                                      lineNumber,
                                      "Specifing zero repetitions is not allowed. Changing to 1. Token: \'" + token + "\'.");
                m_count = 1;
            }
        }
    }

    ssize_t m_count;
    std::string m_countString;
    std::string m_valueString;
};
}


#endif
