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
#include <regex.h>
#include <boost/algorithm/string.hpp>
#include "RawKeyword.hpp"
#include "RawConsts.hpp"
#include <iostream>

namespace Opm {

  RawKeyword::RawKeyword() {
  }

  RawKeyword::RawKeyword(const std::string& keyword) {
    setKeyword(keyword);
  }

  bool RawKeyword::tryParseKeyword(const std::string& keywordCandidate, std::string& result) {
    result = boost::trim_right_copy(keywordCandidate.substr(0, 8));
    if (isValidKeyword(result)) {
      Logger::debug("KEYWORD     <" + keywordCandidate + ">");
      return true;
    }
    return false;
  }

  bool RawKeyword::isValidKeyword(const std::string& keywordCandidate) {

    std::string keywordRegex = "^[A-Z][A-Z,0-9]{1,7}$";
    int status;
    regex_t regularExpression;
    regmatch_t rm;
    if (regcomp(&regularExpression, keywordRegex.c_str(), REG_EXTENDED) != 0) {
      throw std::runtime_error("Unable to compile regular expression for keyword! Expression: " + keywordRegex);
    }

    status = regexec(&regularExpression, keywordCandidate.c_str(), 1, &rm, 0);
    regfree(&regularExpression);

    if (status == 0) {
      return true;
    }
    return false;
  }

  bool RawKeyword::lineContainsData(const std::string& line) {
    if (boost::algorithm::trim_left_copy(line).substr(0, 2) == "--") {
      Logger::debug("COMMENT LINE   <" + line + ">");
      return false;
    } else if (boost::algorithm::trim_copy(line).length() == 0) {
      Logger::debug("EMPTY LINE     <" + line + ">");
      return false;
    } else if (lineTerminatesKeyword(line)) {
      Logger::debug("END OF RECORD  <" + line + ">");
      return false;
    } else {
      Logger::debug("LOOKS LIKE DATA<" + line + ">");
      return true;
    }
  }

  bool RawKeyword::lineTerminatesKeyword(const std::string& line) {
    std::string firstNonBlank = boost::algorithm::trim_left_copy(line).substr(0, 1);
    if (firstNonBlank.size() > (unsigned) 0) {
      return firstNonBlank[0] == Opm::RawConsts::slash;
    }
    return false;
  }

  void RawKeyword::setKeyword(const std::string& keyword) {
    m_keyword = boost::algorithm::trim_right_copy(keyword);
    if (!isValidKeyword(m_keyword)) {
      throw std::invalid_argument("Not a valid keyword:" + keyword);
    } else if (m_keyword.size() > Opm::RawConsts::maxKeywordLength) {
      throw std::invalid_argument("Too long keyword:" + keyword);
    } else if (boost::algorithm::trim_left_copy(m_keyword) != m_keyword) {
      throw std::invalid_argument("Illegal whitespace start of keyword:" + keyword);
    }
  }
  /// Important method, being repeatedly called. When a record is terminated,
  /// it is added to the list of records, and a new record is started.
  void RawKeyword::addRawRecordString(const std::string& partialRecordString) {
    m_partialRecordString += partialRecordString;
    if (RawRecord::isTerminatedRecordString(partialRecordString)) {
      RawRecordPtr record(new RawRecord(m_partialRecordString));
      m_records.push_back(record);
      m_partialRecordString.clear();
    }
  }

  bool RawKeyword::isPartialRecordStringEmpty() const {
    return m_partialRecordString.size() == 0;
  }

  unsigned int RawKeyword::getNumberOfRecords() const {
    return m_records.size();
  }

  const std::list<RawRecordConstPtr>& RawKeyword::getRecords() const {
    return m_records;
  }

  const std::string& RawKeyword::getKeyword() const {
    return m_keyword;
  }

  RawKeyword::~RawKeyword() {
  }
}

