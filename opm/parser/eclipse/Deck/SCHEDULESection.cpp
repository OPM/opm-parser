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


#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/Deck/SCHEDULESection.hpp>

namespace Opm {

    SCHEDULESection::SCHEDULESection(DeckConstPtr deck) : Section(deck, "SCHEDULE") {
        populateDeckTimeSteps();
    }


    DeckTimeStepConstPtr SCHEDULESection::getDeckTimeStep(size_t timestep) const {
        if (timestep < m_decktimesteps.size()) {
            return m_decktimesteps[timestep];
        } else {
            throw std::out_of_range("No DeckTimeStep in ScheduleSection for timestep " + std::to_string(timestep));
        }
    }


    void SCHEDULESection::populateDeckTimeSteps() {
        DeckTimeStepPtr currentTimeStep = std::make_shared<DeckTimeStep>();

        for( size_t i = 0; i < this->size(); ++i ) {
            auto keyword = this->getKeyword( i );
            if (keyword->name() == "TSTEP") {
                DeckItemPtr items = keyword->getDataRecord()->getDataItem();
                for (size_t item_iter = 0; item_iter < items->size(); ++item_iter) {
                   m_decktimesteps.push_back(currentTimeStep);
                   currentTimeStep = std::make_shared<DeckTimeStep>();
                }
            } else if (keyword->name() == "DATES") {
                for( size_t record_iter = 0; record_iter < keyword->size(); ++record_iter ) {
                    m_decktimesteps.push_back(currentTimeStep);
                    currentTimeStep = std::make_shared<DeckTimeStep>();
                }
            } else {
                currentTimeStep->addKeyword(keyword);
            }
        }
        //push last step
        m_decktimesteps.push_back(currentTimeStep);
    }
}
