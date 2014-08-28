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

#include <opm/parser/eclipse/Deck/DeckItem.hpp>

#include <stdexcept>

namespace Opm {

    DeckItem::DeckItem(const std::string& name_) {
        m_name = name_;
        m_defaultApplied = false;
    }

    const std::string& DeckItem::name() const {
        return m_name;
    }

    bool DeckItem::defaultApplied() const {
        return m_defaultApplied;
    }

    void DeckItem::setDefaultApplied(bool yesno) {
        m_defaultApplied = yesno;
    }

}
