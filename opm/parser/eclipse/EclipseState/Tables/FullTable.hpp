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
#ifndef OPM_PARSER_FULL_TABLE_HPP
#define	OPM_PARSER_FULL_TABLE_HPP

#include <opm/parser/eclipse/EclipseState/Tables/MultiRecordTable.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/SingleRecordTable.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cassert>

namespace Opm {
    template <class OuterTable, class InnerTable>
    class FullTable
    {
        typedef FullTable<OuterTable, InnerTable> Self;
        typedef std::shared_ptr<const OuterTable> OuterTableConstPtr;
        typedef std::shared_ptr<InnerTable> InnerTablePtr;
        typedef std::shared_ptr<const InnerTable> InnerTableConstPtr;

    protected:
        FullTable(const FullTable&) = default;

        /*!
         * \brief Read full tables from keywords like PVTO
         *
         * The data for these keywords can be considered a 2D table:
         * The outer one is a multi-record table for a given state,
         * the inner one is a normal table which extends this
         * state. For the PVTO keyword, the outer table represents the
         * gas dissolution factor, pressure, volume factor and
         * viscosity at the oil's saturation point, the inner table is
         * the pressure, volume factor and viscosity of untersaturated
         * oil with the same gas dissolution factor.
         */
        void init(Opm::DeckKeywordConstPtr keyword, size_t tableIdx)
        {
            OuterTable* outerTable = new OuterTable;
            outerTable->init(keyword, tableIdx);
            m_outerTable.reset(outerTable);

            for (size_t rowIdx = 0; rowIdx < m_outerTable->numRecords(); ++rowIdx) {
                InnerTable *curRow = new InnerTable;
                curRow->init(keyword,
                             /*recordIdx=*/m_outerTable->firstRecordIndex() + rowIdx);
                m_innerTables.push_back(std::shared_ptr<InnerTable>(curRow));
            }
        }

    public:
        FullTable() = default;

        typedef std::shared_ptr<Self> Pointer;
        typedef std::shared_ptr<const Self> ConstPointer;

        static size_t numTables(Opm::DeckKeywordConstPtr keyword)
        { return OuterTable::numTables(keyword); }

        std::shared_ptr<const OuterTable> getOuterTable() const
        { return m_outerTable; }

        std::shared_ptr<const InnerTable> getInnerTable(size_t rowIdx) const
        {
            assert(rowIdx < m_innerTables.size());
            return m_innerTables[rowIdx];
        }

        void assignFrom(const FullTable& other) {
            m_outerTable->assignFrom(*other.m_outerTable);
            m_innerTables.resize(other.m_innerTables.size());

            for (size_t i = 0; i < other.m_innerTables.size(); ++i)
                m_innerTables[i]->assignFrom(*other.m_innerTables[i]);
        }

    protected:
        std::shared_ptr<OuterTable> m_outerTable;
        std::vector<std::shared_ptr<InnerTable> > m_innerTables;

    };

    typedef FullTable<Opm::MultiRecordTable, Opm::SingleRecordTable>::Pointer FullTablePtr;
    typedef FullTable<Opm::MultiRecordTable, Opm::SingleRecordTable>::ConstPointer FullTableConstPtr;
}

#endif

