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
#ifndef OPM_PARSER_SIMPLE_TABLE_HPP
#define	OPM_PARSER_SIMPLE_TABLE_HPP

#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableColumn.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/ColumnSchema.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableSchema.hpp>


#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cassert>

namespace Opm {
    class PlyshlogTable;
    class SimpleTable {
        friend class PlyshlogTable;
    protected:
        SimpleTable(const SimpleTable&) = default;
        void init(Opm::DeckItemConstPtr deckItem);

    public:
        SimpleTable();
        void addColumns();

        /*!
         * \brief Returns the number of tables in a keyword.
         *
         * For simple tables, that is identical to the number of
         * records.
         */
        static size_t numTables(Opm::DeckKeywordConstPtr keyword);

        size_t numColumns() const;
        size_t numRows() const;
        const TableColumn&& getColumn(const std::string &name) const;
        const TableColumn&& getColumn(size_t colIdx) const;

        TableColumn&& getColumn(const std::string &name);
        TableColumn&& getColumn(size_t colIdx);

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
        void assertUnitRange(const std::string& columnName);
        void applyDefaultsConstant(const std::string& columnName, double value);
        void applyDefaultsLinear(const std::string& columnName);
        //void createColumns(const TableSchema& tableSchema);
        //void createColumns(const std::vector<std::string>& columnNames);
        void addColumn(std::shared_ptr<const ColumnSchema> schema);

        std::map<std::string, size_t> m_columnNames;
        std::vector<std::vector<bool> > m_valueDefaulted;
        std::vector<TableColumn> m_columns2;
        std::shared_ptr<TableSchema> m_schema;
        OrderedMap<TableColumn> m_columns;
    };

    typedef std::shared_ptr<SimpleTable> SimpleTablePtr;
    typedef std::shared_ptr<const SimpleTable> SimpleTableConstPtr;
}

#endif
