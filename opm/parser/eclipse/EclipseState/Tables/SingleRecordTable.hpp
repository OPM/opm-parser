/*
  Copyright (C) 2013 by Andreas Lauser

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
#ifndef OPM_PARSER_SINGLE_RECORD_TABLE_HPP
#define	OPM_PARSER_SINGLE_RECORD_TABLE_HPP

#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cassert>

namespace Opm {
    class SingleRecordTable {
    protected:
#ifdef BOOST_TEST_MODULE
    public:
#endif
        SingleRecordTable() = default;
    protected:
        SingleRecordTable(const SingleRecordTable&) = default;

        /*!
         * \brief Read simple tables from keywords like SWOF
         *
         * This requires all data to be a list of doubles in the first
         * item of a given record index.
         */
        void init(Opm::DeckKeywordConstPtr keyword,
                  const std::vector<std::string> &columnNames,
                  size_t recordIdx,
                  size_t firstEntityOffset);

    public:
        /*!
         * \brief Returns the number of tables in a keyword.
         *
         * For simple tables, that is identical to the number of
         * records.
         */
        static size_t numTables(Opm::DeckKeywordConstPtr keyword);

#ifdef BOOST_TEST_MODULE
        // DO NOT TRY TO CALL THIS METHOD! it is only for the unit tests!
        void initFORUNITTESTONLY(Opm::DeckKeywordConstPtr keyword,
                  const std::vector<std::string> &columnNames,
                  size_t recordIdx,
                  size_t firstEntityOffset)
        { init(keyword, columnNames, recordIdx, firstEntityOffset); }
#endif

        size_t numColumns() const;
        size_t numRows() const;
        const std::vector<double> &getColumn(const std::string &name) const;
        const std::vector<double> &getColumn(size_t colIdx) const;

        void assignFrom(const SingleRecordTable& other) {
            m_columnNames = other.m_columnNames;
            m_columns = other.m_columns;
            m_valueDefaulted = other.m_valueDefaulted;
        }

        /*!
         * \brief Evaluate a column of the table at a given position.
         *
         * This method uses linear interpolation and always uses the first column as the
         * X coordinate.
         */
        double evaluate(const std::string& columnName, double xPos) const;

    protected:
        void checkNonDefaultable(const std::string& columnName);
        void checkMonotonic(const std::string& columnName,
                             bool isAscending,
                             bool isStrictlyMonotonic = true);
        void applyDefaultsConstant(const std::string& columnName, double value);
        void applyDefaultsLinear(const std::string& columnName);
        void createColumns(const std::vector<std::string> &columnNames);
        size_t getNumFlatItems(Opm::DeckRecordConstPtr deckRecord) const;
        double getFlatRawDoubleData(Opm::DeckRecordConstPtr deckRecord, size_t flatItemIdx) const;
        double getFlatSiDoubleData(Opm::DeckRecordConstPtr deckRecord, size_t flatItemIdx) const;
        bool getFlatIsDefaulted(Opm::DeckRecordConstPtr deckRecord, size_t flatItemIdx) const;

        std::map<std::string, size_t> m_columnNames;
        std::vector<std::vector<double> > m_columns;
        std::vector<std::vector<bool> > m_valueDefaulted;
    };

    typedef std::shared_ptr<SingleRecordTable> SingleRecordTablePtr;
    typedef std::shared_ptr<const SingleRecordTable> SingleRecordTableConstPtr;
}

#endif
