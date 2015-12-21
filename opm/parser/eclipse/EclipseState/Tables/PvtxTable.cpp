/*
  Copyright 2015 Statoil ASA.

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

#include <opm/parser/eclipse/EclipseState/Tables/PvtxTable.hpp>

namespace Opm {

    PvtxTable::PvtxTable(const std::string& columnName) :
        m_outerColumnSchema( columnName , Table::STRICTLY_INCREASING , Table::DEFAULT_NONE ),
        m_outerColumn( m_outerColumnSchema )
    {

    }

    void PvtxTable::init(Opm::DeckKeywordConstPtr keyword , size_t tableIdx) {
        auto ranges = recordRanges( keyword );
        if (tableIdx >= ranges.size())
            throw std::invalid_argument("Asked for table: " + std::to_string( tableIdx ) + " in keyword + " + keyword->name() + " which only has " + std::to_string( ranges.size() ) + " tables");

        {
            auto range = ranges[ tableIdx ];
            for (size_t  rowIdx = range.first; rowIdx < range.second; rowIdx++) {
                Opm::DeckRecordConstPtr deckRecord = keyword->getRecord(rowIdx);
                {
                    Opm::DeckItemConstPtr indexItem = deckRecord->getItem(0);
                    m_outerColumn.addValue( indexItem->getSIDouble( 0 ));
                }
                {
                    Opm::DeckItemConstPtr dataItem = deckRecord->getItem(1);
                    std::shared_ptr<SimpleTable> innerTable = std::make_shared<SimpleTable>(m_innerSchema , dataItem);
                    m_innerTables.push_back( innerTable );
                }
            }
        }
    }


    double PvtxTable::evaluate(const std::string& column, double outerArg, double innerArg) const
    {
        TableIndex outerIndex = m_outerColumn.lookup( outerArg );
        const auto& innerTable1 = getInnerTable( outerIndex.getIndex1( ) );
        double weight1 = outerIndex.getWeight1( );
        double value = weight1 * innerTable1.evaluate( column , innerArg );

        if (weight1 < 1) {
            const auto& innerTable2 = getInnerTable( outerIndex.getIndex2( ) );
            double weight2 = outerIndex.getWeight2( );

            value += weight2 * innerTable2.evaluate( column , innerArg );
        }

        return value;
    }


    const SimpleTable& PvtxTable::getInnerTable(size_t tableNumber) const {
        if (tableNumber >= size())
            throw std::invalid_argument("Invalid table number: " + std::to_string( tableNumber) + " max: " + std::to_string( size() - 1 ));
        return *m_innerTables[ tableNumber ];
    }


    size_t PvtxTable::size() const
    {
        return m_outerColumn.size();
    }

    
    size_t PvtxTable::numTables(Opm::DeckKeywordConstPtr keyword)
    {
        auto ranges = recordRanges(keyword);
        return ranges.size();
    }


    std::vector<std::pair<size_t , size_t> > PvtxTable::recordRanges(Opm::DeckKeywordConstPtr keyword) {
        std::vector<std::pair<size_t,size_t> > ranges;
        size_t startRecord = 0;
        size_t recordIndex = 0;
        while (recordIndex < keyword->size()) {
            auto item = keyword->getRecord(recordIndex)->getItem(0);
            if (item->size( ) == 0) {
                ranges.push_back( std::make_pair( startRecord , recordIndex ) );
                startRecord = recordIndex + 1;
            }
            recordIndex++;
        }
        ranges.push_back( std::make_pair( startRecord , recordIndex ) );
        return ranges;
    }


}

