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

#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>

#include <iostream>
#include <boost/algorithm/string/join.hpp>

namespace Opm {
    
    EclipseState::EclipseState(DeckConstPtr deck) 
    {
        initPhases(deck);
        initEclipseGrid(deck);
        initSchedule(deck);
        initTitle(deck);
        initProperties(deck);
    }
    

    EclipseGridConstPtr EclipseState::getEclipseGrid() const {
        return m_eclipseGrid;
    }


    ScheduleConstPtr EclipseState::getSchedule() const {
        return schedule;
    }


    std::string EclipseState::getTitle() const {
        return m_title;
    }

    void EclipseState::initSchedule(DeckConstPtr deck) {
        schedule = ScheduleConstPtr( new Schedule(deck) );
    }

    void EclipseState::initEclipseGrid(DeckConstPtr deck) {
        std::shared_ptr<Opm::GRIDSection> gridSection(new Opm::GRIDSection(deck) );
        std::shared_ptr<Opm::RUNSPECSection> runspecSection(new Opm::RUNSPECSection(deck) );
        
        m_eclipseGrid = EclipseGridConstPtr( new EclipseGrid( runspecSection , gridSection ));
    }


    void EclipseState::initPhases(DeckConstPtr deck) {
        if (deck->hasKeyword("OIL"))
            phases.insert(Phase::PhaseEnum::OIL);

        if (deck->hasKeyword("GAS"))
            phases.insert(Phase::PhaseEnum::GAS);

        if (deck->hasKeyword("WATER"))
            phases.insert(Phase::PhaseEnum::WATER);
    }


    bool EclipseState::hasPhase(enum Phase::PhaseEnum phase) const {
         return (phases.count(phase) == 1);
    }

    void EclipseState::initTitle(DeckConstPtr deck){
        if (deck->hasKeyword("TITLE")) {
            DeckKeywordConstPtr titleKeyword = deck->getKeyword("TITLE");
            DeckRecordConstPtr record = titleKeyword->getRecord(0);
            DeckItemPtr item = record->getItem(0);
            std::vector<std::string> itemValue = item->getStringData();
            m_title = boost::algorithm::join(itemValue, " ");
        }
    }

    bool EclipseState::supportsGridProperty(const std::string& keyword) const {
         return m_intGridProperties->supportsKeyword( keyword );
    }   

    bool EclipseState::hasIntGridProperty(const std::string& keyword) const {
         return m_intGridProperties->hasKeyword( keyword );
    }   

    /*
      Observe that this will autocreate a property if it has not been explicitly added. 
    */
    std::shared_ptr<GridProperty<int> > EclipseState::getIntProperty( const std::string& keyword ) {
        return m_intGridProperties->getKeyword( keyword );
    }

    
    void EclipseState::loadGridPropertyFromDeckKeyword(DeckKeywordConstPtr deckKeyword) {            
        const std::string& keyword = deckKeyword->name();
        auto gridProperty = m_intGridProperties->getKeyword( keyword );
        gridProperty->loadFromDeckKeyword( deckKeyword );
    }
    
    
    void EclipseState::initProperties(DeckConstPtr deck) {
         size_t volume = m_eclipseGrid->getCartesianSize();
         std::vector<std::pair<std::string , int> > supportedKeywords = {{ "SATNUM" , 0 }};
         m_intGridProperties = std::make_shared<GridProperties<int> >(m_eclipseGrid->getNX() , m_eclipseGrid->getNY() , m_eclipseGrid->getNZ() , supportedIntKeywords); 

         if (Section::hasREGIONS(deck)) {
             std::shared_ptr<Opm::REGIONSSection> regionsSection(new Opm::REGIONSSection(deck) );
             
             for (auto iter = regionsSection->begin(); iter != regionsSection->end(); ++iter) {
                 DeckKeywordConstPtr deckKeyword = *iter;
                 
                 if (supportsGridProperty( deckKeyword->name()))
                     loadGridPropertyFromDeckKeyword( deckKeyword );
                 
             }
         }
    }
}
