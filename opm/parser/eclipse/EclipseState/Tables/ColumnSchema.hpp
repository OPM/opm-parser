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


#ifndef _COLUMN_SCHEMA_HPP_
#define _COLUMN_SCHEMA_HPP_

#include <string>
#include <vector>

#include <opm/parser/eclipse/EclipseState/Tables/TableEnums.hpp>

namespace Opm {

    class ColumnSchema {
    public:
        ColumnSchema(const std::string& name , Table::ColumnOrderEnum order, Table::DefaultAction defaultAction);
        const std::string& name() const;
        bool validOrder( double value1 , double value2) const;
    private:
        std::string m_name;
        Table::ColumnOrderEnum m_order;
        Table::DefaultAction m_defaultAction;
    };
}

#endif

