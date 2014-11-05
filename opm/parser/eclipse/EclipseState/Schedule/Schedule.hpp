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
#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <opm/parser/eclipse/EclipseState/Schedule/TimeMap.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Well.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/GroupTree.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/DynamicState.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Group.hpp>
#include <opm/parser/eclipse/EclipseState/Util/OrderedMap.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Parser/ParserLog.hpp>

#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <map>

namespace Opm 
{
    
    const boost::gregorian::date defaultStartDate( 1983 , boost::gregorian::Jan , 1);

    class Schedule {
    public:
        Schedule(DeckConstPtr deck, ParserLogPtr parserLog=std::make_shared<ParserLog>());
        boost::posix_time::ptime getStartTime() const
        { return m_timeMap->getStartTime(/*timeStepIdx=*/0); }
        TimeMapConstPtr getTimeMap() const;

        size_t numWells() const;
        bool hasWell(const std::string& wellName) const;
        WellPtr getWell(const std::string& wellName) const;
        std::vector<WellConstPtr> getWells() const;
        std::vector<WellConstPtr> getWells(size_t timeStep) const;
        std::vector<WellPtr> getWells(const std::string& wellNamePattern) const;

        GroupTreePtr getGroupTree(size_t t) const;
        size_t numGroups() const;
        bool hasGroup(const std::string& groupName) const;
        GroupPtr getGroup(const std::string& groupName) const;
  
    private:
        TimeMapPtr m_timeMap;
        OrderedMap<WellPtr> m_wells;
        std::map<std::string , GroupPtr> m_groups;
        std::shared_ptr<DynamicState<GroupTreePtr> > m_rootGroupTree;

        void addWellToGroup( GroupPtr newGroup , WellPtr well , size_t timeStep);
        void initFromDeck(DeckConstPtr deck, ParserLogPtr parserLog);
        void createTimeMap(DeckConstPtr deck, ParserLogPtr parserLog);
        void initRootGroupTreeNode(TimeMapConstPtr timeMap);
        void iterateScheduleSection(DeckConstPtr deck, ParserLogPtr parserLog);
        bool handleGroupFromWELSPECS(const std::string& groupName, GroupTreePtr newTree) const;
        void addGroup(const std::string& groupName , size_t timeStep, ParserLogPtr parserLog);
        void addWell(const std::string& wellName, DeckRecordConstPtr record, size_t timeStep, ParserLogPtr parserLog);
        bool checkWELSPECSConsistency(WellConstPtr well, DeckKeywordConstPtr keyword, size_t recordIdx, ParserLogPtr parserLog) const;
        void handleWELSPECS(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleWCONProducer(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep, bool isPredictionMode);
        void handleWCONHIST(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleWCONPROD(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleWGRUPCON(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleCOMPDAT(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleWCONINJE(DeckConstPtr deck, DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleWCONINJH(DeckConstPtr deck, DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleWELOPEN(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleGCONINJE(DeckConstPtr deck, DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleGCONPROD(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);
        void handleDATES(DeckKeywordConstPtr keyword, ParserLogPtr parserLog);
        void handleTSTEP(DeckKeywordConstPtr keyword, ParserLogPtr parserLog);
        void handleGRUPTREE(DeckKeywordConstPtr keyword, ParserLogPtr parserLog, size_t currentStep);

        double convertInjectionRateToSI(double rawRate, WellInjector::TypeEnum wellType, const Opm::UnitSystem &unitSystem) const;
        double convertInjectionRateToSI(double rawRate, Phase::PhaseEnum wellPhase, const Opm::UnitSystem &unitSystem) const;

        static bool convertEclipseStringToBool(const std::string& eclipseString, ParserLogPtr parserLog, const std::string& fileName, int lineNumber);
    };
    typedef std::shared_ptr<Schedule> SchedulePtr;
    typedef std::shared_ptr<const Schedule> ScheduleConstPtr;
}

#endif
