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

#include <string>
#include <stdexcept>

#include <opm/parser/eclipse/Parser/ParserConst.hpp>
#include <opm/parser/eclipse/Parser/ParserEnums.hpp>
#include <opm/parser/eclipse/Parser/ParserRecordSize.hpp>

namespace Opm {

    ParserRecordSize::ParserRecordSize() {
        recordSizeType = UNDEFINED;
        fixedSize = 0;
    }

    ParserRecordSize::ParserRecordSize(size_t fixedSize) {
        recordSizeType = FIXED;
        this->fixedSize = fixedSize;
    }

    size_t ParserRecordSize::recordSize() {
        if (recordSizeType == FIXED) {
            return fixedSize;
        } else
            throw std::logic_error("Only the FIXED recordSize is supported.\n");
    }

    ParserRecordSize::~ParserRecordSize() {

    }


}

