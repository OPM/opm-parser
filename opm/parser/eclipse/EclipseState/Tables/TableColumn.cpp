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
#include <stdexcept>
#include <algorithm>

#include <opm/parser/eclipse/EclipseState/Tables/TableColumn.hpp>

namespace Opm {

    TableColumn::TableColumn(const ColumnSchema& schema) :
        m_schema( schema )
    {
        m_defaultCount = 0;
    }



    size_t TableColumn::size() const {
        return m_values.size();
    }


    void TableColumn::assertOrder(double value1 , double value2) const {
        if (!m_schema.validOrder( value1 , value2) )
            throw std::invalid_argument("Incorrect ordering of values");
    }



    void TableColumn::assertNext(size_t index , double value) const {
        size_t nextIndex = index + 1;
        if (nextIndex < m_values.size()) {
            if (!m_default[nextIndex]) {
                double nextValue = m_values[nextIndex];
                assertOrder( value , nextValue );
            }
        }
    }


    void TableColumn::assertPrevious(size_t index , double value) const {
        if (index > 0) {
            size_t prevIndex = index - 1;
            if (!m_default[prevIndex]) {
                double prevValue = m_values[prevIndex];
                assertOrder( prevValue , value );
            }
        }
    }


    void TableColumn::assertUpdate(size_t index, double value) const {
        assertNext( index , value );
        assertPrevious( index, value );
    }




    void TableColumn::addValue(double value) {
        assertUpdate( m_values.size() , value );
        m_values.push_back( value );
        m_default.push_back( false );
    }


    void TableColumn::addDefault() {
        if (m_schema.acceptsDefault()) {
            m_values.push_back( -1 ); // Should never even be read.
            m_default.push_back( true );
            m_defaultCount += 1;
        } else
            throw std::invalid_argument("The column does not accept default values");
    }


    void TableColumn::updateValue(  size_t index , double value ) {
        assertUpdate( index , value );
        m_values[index] = value;
        if (m_default[index]) {
            m_default[index] = false;
            m_defaultCount -= 1;
        }
    }

    bool TableColumn::defaultApplied(size_t index) const {
        if (index >= m_values.size())
            throw std::invalid_argument("Value: " + std::to_string( index ) + " out of range: [0," + std::to_string( m_values.size()) + ")");

        return m_default[index];
    }

    double TableColumn::operator[](size_t index) const {
        if (index >= m_values.size())
            throw std::invalid_argument("Value: " + std::to_string( index ) + " out of range: [0," + std::to_string( m_values.size()) + ")");

        if (m_default[index])
            throw std::invalid_argument("Value at index " + std::to_string( index ) + " is defaulted - can not ask!");

        return m_values[index];
    }

    double TableColumn::back() const {
        return m_values.back( );
    }


    double TableColumn::front() const {
        return m_values.front( );
    }

    double TableColumn::max( ) const {
        if (hasDefault())
            throw std::invalid_argument("Can not lookup elements in a column with defaulted values.");
        if (m_values.size() > 0)
            return *std::max_element( m_values.begin() , m_values.end());
        else
            throw std::invalid_argument("Can not find max in empty column");
    }


    double TableColumn::min( ) const {
        if (hasDefault())
            throw std::invalid_argument("Can not lookup elements in a column with defaulted values.");
        if (m_values.size() > 0)
            return *std::min_element( m_values.begin() , m_values.end());
        else
            throw std::invalid_argument("Can not find max in empty column");
    }


    bool TableColumn::inRange( double arg ) const {
        if (m_values.size( ) >= 2) {
            if (!m_schema.lookupValid( ))
                throw std::invalid_argument("Must have an ordered column to check in range.");

            if ((arg >= min()) && (arg <= max()))
                return true;
            else
                return false;

        } else
            throw std::invalid_argument("Minimum size 2 ");
    }


    TableIndex TableColumn::lookup( double argValue ) const {
        if (!m_schema.lookupValid( ))
            throw std::invalid_argument("Must have an ordered column to perform table argument lookup.");

        if (size() < 2)
            throw std::invalid_argument("Must have at least two elements in column for table argument lookup.");

        if (hasDefault())
            throw std::invalid_argument("Can not lookup elements in a column with defaulted values.");

        if (!inRange(argValue))
            throw std::invalid_argument("Value not in range.");

    }


    bool TableColumn::hasDefault( ) const {
        if (m_defaultCount > 0)
            return true;
        else
            return false;
    }

}


