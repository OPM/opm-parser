/*
  Copyright 2014 Statoil ASA.

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

#include <opm/parser/eclipse/EclipseState/Grid/Box.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperty.hpp>

namespace Opm {

template<>  
void GridProperty<int>::loadFromDeckKeyword(Opm::DeckKeywordConstPtr deckKeyword) {
    if (deckKeyword->isDataKeyword()) {
        if (!deckKeyword->getRecord(0)->getItem(0)->defaultApplied()) {
            // only use the array from the keyword if it does not contain garbage
            const std::vector<int>& data = deckKeyword->getIntData();
            setFromVector(data);
        }
    } else
        throw std::invalid_argument("Can only load from DATA keywords");
}

template<>  
void GridProperty<int>::loadFromDeckKeyword(std::shared_ptr<const Box> inputBox , Opm::DeckKeywordConstPtr deckKeyword) {
    if (deckKeyword->isDataKeyword()) {
        const std::vector<int>& data = deckKeyword->getIntData();
        setFromVector(inputBox , data);
    } else
        throw std::invalid_argument("Can only load from DATA keywords");
}

/*****************************************************************/

template<>  
void GridProperty<double>::loadFromDeckKeyword(Opm::DeckKeywordConstPtr deckKeyword) {
    if (deckKeyword->isDataKeyword()) {
        const std::vector<double>& data = deckKeyword->getSIDoubleData();
        setFromVector(data);
    } else
        throw std::invalid_argument("Can only load from DATA keywords");
}

template<>  
void GridProperty<double>::loadFromDeckKeyword(std::shared_ptr<const Box> inputBox , Opm::DeckKeywordConstPtr deckKeyword) {
    if (deckKeyword->isDataKeyword()) {
        if (!deckKeyword->getRecord(0)->getItem(0)->defaultApplied()) {
            // only use the array from the keyword if it does not contain garbage
            const std::vector<double>& data = deckKeyword->getSIDoubleData();
            setFromVector(inputBox , data);
        }
    } else
        throw std::invalid_argument("Can only load from DATA keywords");
}

}
