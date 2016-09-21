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

#include <map>
#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/Util/OrderedMap.hpp>
#include <opm/parser/eclipse/Parser/MessageContainer.hpp>

namespace Opm
{

    template< typename > class DynamicState;
    template< typename > class DynamicVector;

    class Deck;
    class DeckKeyword;
    class DeckRecord;
    class EclipseGrid;
    class Events;
    class Group;
    class GroupTree;
    class OilVaporizationProperties;
    class ParseContext;
    class SCHEDULESection;
    class TimeMap;
    class Tuning;
    class UnitSystem;
    class Well;

    class Schedule {
    public:
        Schedule(const ParseContext& parseContext, std::shared_ptr<const EclipseGrid> grid,
                 const Deck& deck );
        /// [deprecated]
        Schedule(const ParseContext& parseContext, std::shared_ptr<const EclipseGrid> grid,
                 std::shared_ptr<const Deck> deck );

        /*
         * If the input deck does not specify a start time, Eclipse's 1. Jan
         * 1983 is defaulted
         */
        boost::posix_time::ptime getStartTime() const;
        time_t posixStartTime() const;

        std::shared_ptr< const TimeMap > getTimeMap() const;

        size_t numWells() const;
        size_t numWells(size_t timestep) const;
        size_t numOpenWells(size_t timestep) const;
        size_t getMaxNumCompletionsForWells(size_t timestep) const;
        bool hasWell(const std::string& wellName) const;
        const Well* getWell(const std::string& wellName) const;
        std::vector< const Well* > getOpenWells(size_t timeStep) const;
        std::vector< const Well* > getWells() const;
        std::vector< const Well* > getWells(size_t timeStep) const;
        std::vector< const Well* > getWellsMatching( const std::string& ) const;
        std::shared_ptr< const OilVaporizationProperties > getOilVaporizationProperties(size_t timestep);

        std::shared_ptr< GroupTree > getGroupTree(size_t t) const;
        size_t numGroups() const;
        bool hasGroup(const std::string& groupName) const;
        const Group* getGroup(const std::string& groupName) const;
        std::vector< const Group* > getGroups() const;
        std::shared_ptr< Tuning > getTuning() const;

        const Events& getEvents() const;
        bool hasOilVaporizationProperties();
        std::shared_ptr<const Deck> getModifierDeck(size_t timeStep) const;
        const MessageContainer& getMessageContainer() const;
        MessageContainer& getMessageContainer();


    private:
        std::shared_ptr< TimeMap > m_timeMap;
        OrderedMap<std::shared_ptr< Well >> m_wells;
        std::shared_ptr<const EclipseGrid> m_grid;
        std::map<std::string , std::shared_ptr< Group >> m_groups;
        std::shared_ptr<DynamicState<std::shared_ptr< GroupTree >> > m_rootGroupTree;
        std::shared_ptr<DynamicState<std::shared_ptr< OilVaporizationProperties > > > m_oilvaporizationproperties;
        std::shared_ptr<Events> m_events;
        std::shared_ptr<DynamicVector<std::shared_ptr<Deck> > > m_modifierDeck;
        std::shared_ptr< Tuning > m_tuning;
        MessageContainer m_messages;


        std::vector< Well* > getWells(const std::string& wellNamePattern);
        void updateWellStatus( Well& well, size_t reportStep , WellCommon::StatusEnum status);
        void addWellToGroup( Group& newGroup , Well& well , size_t timeStep);
        void initRootGroupTreeNode(std::shared_ptr< const TimeMap > timeMap);
        void initOilVaporization(std::shared_ptr< const TimeMap > timeMap);
        void iterateScheduleSection(const ParseContext& parseContext ,  const SCHEDULESection& );
        bool handleGroupFromWELSPECS(const std::string& groupName, std::shared_ptr< GroupTree > newTree) const;
        void addGroup(const std::string& groupName , size_t timeStep);
        void addWell(const std::string& wellName, const DeckRecord& record, size_t timeStep, WellCompletion::CompletionOrderEnum wellCompletionOrder);
        void handleCOMPORD(const ParseContext& parseContext, const DeckKeyword& compordKeyword, size_t currentStep);
        void checkWELSPECSConsistency( const Well&, const DeckKeyword& keyword, size_t recordIdx);
        void handleWELSPECS( const SCHEDULESection&, size_t, size_t  );
        void handleWCONProducer( const DeckKeyword& keyword, size_t currentStep, bool isPredictionMode);
        void handleWCONHIST( const DeckKeyword& keyword, size_t currentStep);
        void handleWCONPROD( const DeckKeyword& keyword, size_t currentStep);
        void handleWGRUPCON( const DeckKeyword& keyword, size_t currentStep);
        void handleCOMPDAT( const DeckKeyword& keyword,  size_t currentStep);
        void handleWELSEGS( const DeckKeyword& keyword, size_t currentStep);
        void handleCOMPSEGS( const DeckKeyword& keyword, size_t currentStep);
        void handleWCONINJE( const SCHEDULESection&,  const DeckKeyword& keyword, size_t currentStep);
        void handleWPOLYMER( const DeckKeyword& keyword, size_t currentStep);
        void handleWSOLVENT( const DeckKeyword& keyword, size_t currentStep);
        void handleWCONINJH( const SCHEDULESection&,  const DeckKeyword& keyword, size_t currentStep);
        void handleWELOPEN( const DeckKeyword& keyword, size_t currentStep, bool hascomplump);
        void handleWELTARG( const SCHEDULESection&,  const DeckKeyword& keyword, size_t currentStep);
        void handleGCONINJE( const SCHEDULESection&,  const DeckKeyword& keyword, size_t currentStep);
        void handleGCONPROD( const DeckKeyword& keyword, size_t currentStep);
        void handleGEFAC( const DeckKeyword& keyword, size_t currentStep);
        void handleTUNING( const DeckKeyword& keyword, size_t currentStep);
        void handleGRUPTREE( const DeckKeyword& keyword, size_t currentStep);
        void handleWRFT( const DeckKeyword& keyword, size_t currentStep);
        void handleWRFTPLT( const DeckKeyword& keyword, size_t currentStep);
        void handleWPIMULT( const DeckKeyword& keyword, size_t currentStep);
        void handleDRSDT( const DeckKeyword& keyword, size_t currentStep);
        void handleDRVDT( const DeckKeyword& keyword, size_t currentStep);
        void handleVAPPARS( const DeckKeyword& keyword, size_t currentStep);
        void handleWECON( const DeckKeyword& keyword, size_t currentStep);

        void checkUnhandledKeywords( const SCHEDULESection& ) const;

        static double convertInjectionRateToSI(double rawRate, WellInjector::TypeEnum wellType, const Opm::UnitSystem &unitSystem);
        static double convertInjectionRateToSI(double rawRate, Phase::PhaseEnum wellPhase, const Opm::UnitSystem &unitSystem);
        static bool convertEclipseStringToBool(const std::string& eclipseString);


        void setOilVaporizationProperties(const std::shared_ptr< OilVaporizationProperties > vapor, size_t timestep);

    };
    typedef std::shared_ptr<Schedule> SchedulePtr;
    typedef std::shared_ptr<const Schedule> ScheduleConstPtr;
}

#endif
