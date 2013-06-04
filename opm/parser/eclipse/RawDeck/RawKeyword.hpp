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

#ifndef RAWKEYWORD_HPP
#define RAWKEYWORD_HPP

#include <string>
#include <utility>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "opm/parser/eclipse/Logger/Logger.hpp"
#include "RawRecord.hpp"

namespace Opm {

    /// Class representing a RawKeyword, meaning both the actual keyword phrase, and the records,
    /// represented as a list of RawRecord objects.
    /// The class also contains static functions to aid the parsing of the input file.
    /// The creating of an instance is performed by calling the addRawRecordString method repeatedly.

    class RawKeyword {
    public:
        RawKeyword(const std::string& name);
        const std::string& getKeywordName() const;
        void addRawRecordString(const std::string& partialRecordString);
        size_t size() const;
        RawRecordPtr getRecord(size_t index) const;
        
        static bool tryParseKeyword(const std::string& line, std::string& result);
        static bool lineContainsData(const std::string& line);
        static bool lineTerminatesKeyword(const std::string& line);

        bool isPartialRecordStringEmpty() const;

    private:
        std::string m_name;
        std::vector<RawRecordPtr> m_records;
        std::string m_partialRecordString;
        void setKeywordName(const std::string& keyword);
        static bool isValidKeyword(const std::string& keywordCandidate);
    };
    typedef boost::shared_ptr<RawKeyword> RawKeywordPtr;
    typedef boost::shared_ptr<const RawKeyword> RawKeywordConstPtr;

}
#endif  /* RAWKEYWORD_HPP */

