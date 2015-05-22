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
#include <vector>

#include <boost/algorithm/string.hpp>
#include <opm/parser/eclipse/OpmLog/OpmLog.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>

#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/TimeMap.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/WellProductionProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/WellInjectionProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/WellPolymerProperties.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>

namespace Opm {

    Schedule::Schedule(std::shared_ptr<const EclipseGrid> grid , DeckConstPtr deck, IOConfigPtr ioConfig)
        : m_grid(grid)
    {
        initFromDeck(deck, ioConfig);
    }

    void Schedule::initFromDeck(DeckConstPtr deck, IOConfigPtr ioConfig) {
        initializeNOSIM(deck);
        createTimeMap(deck);
        m_tuning.reset(new Tuning(m_timeMap));
        addGroup( "FIELD", 0 );
        initRootGroupTreeNode(getTimeMap());
        iterateScheduleSection(deck, ioConfig);
    }

    void Schedule::initRootGroupTreeNode(TimeMapConstPtr timeMap) {
        m_rootGroupTree.reset(new DynamicState<GroupTreePtr>(timeMap, GroupTreePtr(new GroupTree())));
    }

    void Schedule::initializeNOSIM(DeckConstPtr deck) {
        if (deck->hasKeyword("NOSIM")){
            nosim = true;
        } else {
            nosim = false;
        }
    }

    void Schedule::createTimeMap(DeckConstPtr deck) {
        boost::posix_time::ptime startTime(defaultStartDate);
        if (deck->hasKeyword("START")) {
            DeckKeywordConstPtr startKeyword = deck->getKeyword("START");
            startTime = TimeMap::timeFromEclipse(startKeyword->getRecord(0));
        }

        m_timeMap.reset(new TimeMap(startTime));
    }

    void Schedule::iterateScheduleSection(DeckConstPtr deck, IOConfigPtr ioConfig) {
        size_t currentStep = 0;
        std::vector<std::pair<DeckKeywordConstPtr , size_t> > rftProperties;

        for (size_t keywordIdx = 0; keywordIdx < deck->size(); ++keywordIdx) {
            DeckKeywordConstPtr keyword = deck->getKeyword(keywordIdx);

            if (keyword->name() == "DATES") {
                handleDATES(keyword);
                currentStep += keyword->size();
            }

            if (keyword->name() == "TSTEP") {
                handleTSTEP(keyword);
                currentStep += keyword->getRecord(0)->getItem(0)->size(); // This is a bit weird API.
            }

            if (keyword->name() == "WELSPECS") {
                handleWELSPECS(keyword, currentStep);
            }

            if (keyword->name() == "WCONHIST")
                handleWCONHIST(keyword, currentStep);

            if (keyword->name() == "WCONPROD")
                handleWCONPROD(keyword, currentStep);

            if (keyword->name() == "WCONINJE")
                handleWCONINJE(deck, keyword, currentStep);

            if (keyword->name() == "WPOLYMER")
                handleWPOLYMER(keyword, currentStep);

            if (keyword->name() == "WCONINJH")
                handleWCONINJH(deck, keyword, currentStep);

            if (keyword->name() == "WGRUPCON")
                handleWGRUPCON(keyword, currentStep);

            if (keyword->name() == "COMPDAT")
                handleCOMPDAT(keyword, currentStep);

            if (keyword->name() == "WELOPEN")
                handleWELOPEN(keyword, currentStep , deck->hasKeyword("COMPLUMP"));

            if (keyword->name() == "WELTARG")
                handleWELTARG(deck, keyword, currentStep);

            if (keyword->name() == "GRUPTREE")
                handleGRUPTREE(keyword, currentStep);

            if (keyword->name() == "GCONINJE")
                handleGCONINJE(deck, keyword, currentStep);

            if (keyword->name() == "GCONPROD")
                handleGCONPROD(keyword, currentStep);

            if (keyword->name() == "TUNING")
                handleTUNING(keyword, currentStep);

            if (keyword->name() == "NOSIM")
                handleNOSIM();

            if (keyword->name() == "RPTRST")
                handleRPTRST(keyword, currentStep, ioConfig);

            if (keyword->name() == "WRFT")
                rftProperties.push_back( std::make_pair( keyword , currentStep ));

            if (keyword->name() == "WRFTPLT")
                rftProperties.push_back( std::make_pair( keyword , currentStep ));

        }

        for (auto rftPair = rftProperties.begin(); rftPair != rftProperties.end(); ++rftPair) {
            DeckKeywordConstPtr keyword = rftPair->first;
            size_t timeStep = rftPair->second;
            if (keyword->name() == "WRFT") {
                handleWRFT(keyword,  timeStep);
            }

            if (keyword->name() == "WRFTPLT"){
                handleWRFTPLT(keyword, timeStep);
            }
        }

        checkUnhandledKeywords(deck);
    }

    void Schedule::handleDATES(DeckKeywordConstPtr keyword) {
        m_timeMap->addFromDATESKeyword(keyword);
    }

    void Schedule::handleTSTEP(DeckKeywordConstPtr keyword) {
        m_timeMap->addFromTSTEPKeyword(keyword);
    }

    bool Schedule::handleGroupFromWELSPECS(const std::string& groupName, GroupTreePtr newTree) const {
        bool treeUpdated = false;
        if (!newTree->getNode(groupName)) {
            treeUpdated = true;
            newTree->updateTree(groupName);
        }
        return treeUpdated;
    }

    void Schedule::handleWELSPECS(DeckKeywordConstPtr keyword, size_t currentStep) {
        bool needNewTree = false;
        GroupTreePtr newTree = m_rootGroupTree->get(currentStep)->deepCopy();

        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& wellName = record->getItem("WELL")->getTrimmedString(0);
            const std::string& groupName = record->getItem("GROUP")->getTrimmedString(0);

            if (!hasGroup(groupName)) {
                addGroup(groupName , currentStep);
            }

            if (!hasWell(wellName)) {
                addWell(wellName, record, currentStep);
            }

            WellConstPtr currentWell = getWell(wellName);
            checkWELSPECSConsistency(currentWell, keyword, recordNr);

            addWellToGroup( getGroup(groupName) , getWell(wellName) , currentStep);
            bool treeChanged = handleGroupFromWELSPECS(groupName, newTree);
            needNewTree = needNewTree || treeChanged;
        }
        if (needNewTree) {
            m_rootGroupTree->add(currentStep, newTree);
        }
    }
    
    void Schedule::checkWELSPECSConsistency(WellConstPtr well, DeckKeywordConstPtr keyword, size_t recordIdx) const {
        DeckRecordConstPtr record = keyword->getRecord(recordIdx);
        if (well->getHeadI() != record->getItem("HEAD_I")->getInt(0) - 1) {
            std::string msg =
                "Unable process WELSPECS for well " + well->name() + ", HEAD_I deviates from existing value";
            OpmLog::addMessage(Log::MessageType::Error , Log::fileMessage( keyword->getFileName(), keyword->getLineNumber(), msg));
            throw std::invalid_argument(msg);
        }
        if (well->getHeadJ() != record->getItem("HEAD_J")->getInt(0) - 1) {
            std::string msg =
                "Unable process WELSPECS for well " + well->name() + ", HEAD_J deviates from existing value";
            OpmLog::addMessage(Log::MessageType::Error , Log::fileMessage( keyword->getFileName(), keyword->getLineNumber(), msg));
            throw std::invalid_argument(msg);
        }
    }

    void Schedule::handleWCONProducer(DeckKeywordConstPtr keyword, size_t currentStep, bool isPredictionMode) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);

            const std::string& wellNamePattern =
                record->getItem("WELL")->getTrimmedString(0);

            const WellCommon::StatusEnum status =
                WellCommon::StatusFromString(record->getItem("STATUS")->getTrimmedString(0));

            WellProductionProperties properties =
                ((isPredictionMode)
                 ? WellProductionProperties::prediction(record)
                 : WellProductionProperties::history   (record));

            const std::vector<WellPtr> wells = getWells(wellNamePattern);

            for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
                WellPtr well = *wellIter;

                if (well->isAvailableForGroupControl(currentStep)) {
                    properties.addProductionControl(WellProducer::GRUP);
                }
                else {
                    properties.dropProductionControl(WellProducer::GRUP);
                }

                if (status != WellCommon::SHUT) {
                    const std::string& cmodeString =
                        record->getItem("CMODE")->getTrimmedString(0);

                    WellProducer::ControlModeEnum control =
                        WellProducer::ControlModeFromString(cmodeString);

                    if (properties.hasProductionControl(control)) {
                        properties.controlMode = control;
                    }
                    else {
                        std::string msg =
                            "Tried to set invalid control: " +
                            cmodeString + " for well: " + well->name();
                        OpmLog::addMessage(Log::MessageType::Error , Log::fileMessage( keyword->getFileName(), keyword->getLineNumber(), msg));
                        throw std::invalid_argument(msg);
                    }
                }

                well->setStatus( currentStep , status );
                well->setProductionProperties(currentStep, properties);
            }
        }
    }

    void Schedule::handleWCONHIST(DeckKeywordConstPtr keyword, size_t currentStep) {
        handleWCONProducer(keyword, currentStep, false);
    }

    void Schedule::handleWCONPROD(DeckKeywordConstPtr keyword, size_t currentStep) {
        handleWCONProducer(keyword, currentStep, true);
    }

    void Schedule::handleWCONINJE(DeckConstPtr deck, DeckKeywordConstPtr keyword, size_t currentStep) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& wellNamePattern = record->getItem("WELL")->getTrimmedString(0);
            std::vector<WellPtr> wells = getWells(wellNamePattern);

            for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
                WellPtr well = *wellIter;
                WellInjector::TypeEnum injectorType = WellInjector::TypeFromString( record->getItem("TYPE")->getTrimmedString(0) );
                WellCommon::StatusEnum status = WellCommon::StatusFromString( record->getItem("STATUS")->getTrimmedString(0));

                well->setStatus( currentStep , status );
                WellInjectionProperties properties(well->getInjectionPropertiesCopy(currentStep));

                properties.injectorType = injectorType;
                properties.predictionMode = true;

                if (!record->getItem("RATE")->defaultApplied(0)) {
                    properties.surfaceInjectionRate = convertInjectionRateToSI(record->getItem("RATE")->getRawDouble(0) , injectorType, *deck->getActiveUnitSystem());
                    properties.addInjectionControl(WellInjector::RATE);
                } else
                    properties.dropInjectionControl(WellInjector::RATE);


                if (!record->getItem("RESV")->defaultApplied(0)) {
                    properties.reservoirInjectionRate = record->getItem("RESV")->getSIDouble(0);
                    properties.addInjectionControl(WellInjector::RESV);
                } else
                    properties.dropInjectionControl(WellInjector::RESV);


                if (!record->getItem("THP")->defaultApplied(0)) {
                    properties.THPLimit = record->getItem("THP")->getSIDouble(0);
                    properties.addInjectionControl(WellInjector::THP);
                } else
                    properties.dropInjectionControl(WellInjector::THP);

                /*
                  What a mess; there is a sensible default BHP limit
                  defined, so the BHPLimit can be safely set
                  unconditionally - but should we make BHP control
                  available based on that default value - currently we
                  do not do that.
                */
                properties.BHPLimit = record->getItem("BHP")->getSIDouble(0);
                if (!record->getItem("BHP")->defaultApplied(0)) {
                    properties.addInjectionControl(WellInjector::BHP);
                } else
                    properties.dropInjectionControl(WellInjector::BHP);

                if (well->isAvailableForGroupControl(currentStep))
                    properties.addInjectionControl(WellInjector::GRUP);
                else
                    properties.dropInjectionControl(WellInjector::GRUP);
                {
                    const std::string& cmodeString = record->getItem("CMODE")->getTrimmedString(0);
                    WellInjector::ControlModeEnum controlMode = WellInjector::ControlModeFromString( cmodeString );
                    if (properties.hasInjectionControl( controlMode))
                        properties.controlMode = controlMode;
                    else {
                        throw std::invalid_argument("Tried to set invalid control: " + cmodeString + " for well: " + wellNamePattern);
                    }
                }
                well->setInjectionProperties(currentStep, properties);
            }
        }
    }


    void Schedule::handleWPOLYMER(DeckKeywordConstPtr keyword, size_t currentStep) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& wellNamePattern = record->getItem("WELL")->getTrimmedString(0);
            std::vector<WellPtr> wells = getWells(wellNamePattern);

            for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
                WellPtr well = *wellIter;

                WellPolymerProperties properties(well->getPolymerPropertiesCopy(currentStep));

                properties.m_polymerConcentration = record->getItem("POLYMER_CONCENTRATION")->getSIDouble(0);
                properties.m_saltConcentration = record->getItem("SALT_CONCENTRATION")->getSIDouble(0);

                auto group_polymer_item = record->getItem("GROUP_POLYMER_CONCENTRATION");
                auto group_salt_item = record->getItem("GROUP_SALT_CONCENTRATION");

                if (!group_polymer_item->defaultApplied(0)) {
                    throw std::logic_error("Sorry explicit setting of \'GROUP_POLYMER_CONCENTRATION\' is not supported!");
                }

                if (!group_salt_item->defaultApplied(0)) {
                    throw std::logic_error("Sorry explicit setting of \'GROUP_SALT_CONCENTRATION\' is not supported!");
                }
                well->setPolymerProperties(currentStep, properties);
            }
        }
    }

    void Schedule::handleWCONINJH(DeckConstPtr deck, DeckKeywordConstPtr keyword, size_t currentStep) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& wellName = record->getItem("WELL")->getTrimmedString(0);
            WellPtr well = getWell(wellName);

            // convert injection rates to SI
            WellInjector::TypeEnum injectorType = WellInjector::TypeFromString( record->getItem("TYPE")->getTrimmedString(0));
            double injectionRate = record->getItem("RATE")->getRawDouble(0);
            injectionRate = convertInjectionRateToSI(injectionRate, injectorType, *deck->getActiveUnitSystem());

            WellCommon::StatusEnum status = WellCommon::StatusFromString( record->getItem("STATUS")->getTrimmedString(0));

            well->setStatus( currentStep , status );
            WellInjectionProperties properties(well->getInjectionPropertiesCopy(currentStep));

            properties.injectorType = injectorType;

            const std::string& cmodeString = record->getItem("CMODE")->getTrimmedString(0);
            WellInjector::ControlModeEnum controlMode = WellInjector::ControlModeFromString( cmodeString );
            if (!record->getItem("RATE")->defaultApplied(0)) {
                properties.surfaceInjectionRate = injectionRate;
                properties.addInjectionControl(controlMode);
                properties.controlMode = controlMode;
            }
            properties.predictionMode = false;

            well->setInjectionProperties(currentStep, properties);
        }
    }

    static Opm::Value<int> getValueItem(DeckItemPtr item){
        Opm::Value<int> data(item->name());
        if(item->hasValue(0)) {
            int tempValue = item->getInt(0);
            if( tempValue >0){
                data.setValue(tempValue-1);
            }
        }
        return data;
    }

    void Schedule::handleWELOPEN(DeckKeywordConstPtr keyword, size_t currentStep , bool hascomplump) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);

            bool haveCompletionData = false;
            for (size_t i=2; i<7; i++) {
                if (record->getItem(i)->hasValue(0)) {
                    haveCompletionData = true;
                    break;
                }
            }

            const std::string& wellNamePattern = record->getItem("WELL")->getTrimmedString(0);
            const std::vector<WellPtr>& wells = getWells(wellNamePattern);

            for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
                WellPtr well = *wellIter;

                if(haveCompletionData){
                    CompletionSetConstPtr currentCompletionSet = well->getCompletions(currentStep);

                    CompletionSetPtr newCompletionSet(new CompletionSet( ));

                    Opm::Value<int> I  = getValueItem(record->getItem("I"));
                    Opm::Value<int> J  = getValueItem(record->getItem("J"));
                    Opm::Value<int> K  = getValueItem(record->getItem("K"));
                    Opm::Value<int> C1 = getValueItem(record->getItem("C1"));
                    Opm::Value<int> C2 = getValueItem(record->getItem("C2"));

                    if(hascomplump && (C2.hasValue() || C1.hasValue())){
                        std::cerr << "ERROR the keyword COMPLUMP is not supported used when C1 or C2 in WELOPEN have values" << std::endl;
                        throw std::exception();
                    }

                    size_t completionSize = currentCompletionSet->size();

                    for(size_t i = 0; i < completionSize;i++) {

                        CompletionConstPtr currentCompletion = currentCompletionSet->get(i);

                        if (C1.hasValue()) {
                            if (i < (size_t) C1.getValue()) {
                                newCompletionSet->add(currentCompletion);
                                continue;
                            }
                        }
                        if (C2.hasValue()) {
                            if (i > (size_t) C2.getValue()) {
                                newCompletionSet->add(currentCompletion);
                                continue;
                            }
                        }

                        int ci = currentCompletion->getI();
                        int cj = currentCompletion->getJ();
                        int ck = currentCompletion->getK();

                        if (I.hasValue() && (!(I.getValue() == ci) )) {
                            newCompletionSet->add(currentCompletion);
                            continue;
                        }

                        if (J.hasValue() && (!(J.getValue() == cj) )) {
                            newCompletionSet->add(currentCompletion);
                            continue;
                        }

                        if (K.hasValue() && (!(K.getValue() == ck) )) {
                            newCompletionSet->add(currentCompletion);
                            continue;
                        }
                        WellCompletion::StateEnum completionStatus = WellCompletion::StateEnumFromString(record->getItem("STATUS")->getTrimmedString(0));
                        CompletionPtr newCompletion = std::make_shared<Completion>(currentCompletion, completionStatus);
                        newCompletionSet->add(newCompletion);
                    }
                    well->addCompletionSet(currentStep, newCompletionSet);

                    if (newCompletionSet->allCompletionsShut()) {
                      well->setStatus(currentStep, WellCommon::StatusEnum::SHUT);
                    }
                }
                else if(!haveCompletionData) {
                    WellCommon::StatusEnum status = WellCommon::StatusFromString( record->getItem("STATUS")->getTrimmedString(0));
                    well->setStatus(currentStep, status);
                }
            }
        }
    }

    /*
    The documentation for the WELTARG keyword says that the well must have been fully specified and initialized using one of the WCONxxxx keywords prior to
    modifying the well using the WELTARG keyword. The following implementation of handling the WELTARG keyword does not check or enforce in any way that
    this is done (i.e. it is not checked or verified that the well is initialized with any WCONxxxx keyword).
    */
    void Schedule::handleWELTARG(DeckConstPtr deck, DeckKeywordConstPtr keyword, size_t currentStep) {
        Opm::UnitSystem unitSystem = *deck->getActiveUnitSystem();
        double siFactorL = unitSystem.parse("LiquidSurfaceVolume/Time")->getSIScaling();
        double siFactorG = unitSystem.parse("GasSurfaceVolume/Time")->getSIScaling();
        double siFactorP = unitSystem.parse("Pressure")->getSIScaling();

        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);

            const std::string& wellNamePattern = record->getItem("WELL")->getTrimmedString(0);
            const std::string& cMode = record->getItem("CMODE")->getTrimmedString(0);
            double newValue = record->getItem("NEW_VALUE")->getRawDouble(0);
            const std::vector<WellPtr>& wells = getWells(wellNamePattern);

            for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
                WellPtr well = *wellIter;
                WellProductionProperties prop = well->getProductionPropertiesCopy(currentStep);

                if (cMode == "ORAT"){
                    prop.OilRate = newValue * siFactorL;
                }
                else if (cMode == "WRAT"){
                    prop.WaterRate = newValue * siFactorL;
                }
                else if (cMode == "GRAT"){
                    prop.GasRate = newValue * siFactorG;
                }
                else if (cMode == "LRAT"){
                    prop.LiquidRate = newValue * siFactorL;
                }
                else if (cMode == "RESV"){
                    prop.ResVRate = newValue * siFactorL;
                }
                else if (cMode == "BHP"){
                    prop.BHPLimit = newValue * siFactorP;
                }
                else if (cMode == "THP"){
                    prop.THPLimit = newValue * siFactorP;
                }
                else if (cMode == "VFP"){
                    prop.VFPTableNumber = static_cast<int> (newValue);
                }
                else if (cMode == "GUID"){
                    well->setGuideRate(currentStep, newValue);
                }
                else{
                    throw std::invalid_argument("Invalid keyword (MODE) supplied");
                }

                well->setProductionProperties(currentStep, prop);
            }
        }
    }

    void Schedule::handleGCONINJE(DeckConstPtr deck, DeckKeywordConstPtr keyword, size_t currentStep) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& groupName = record->getItem("GROUP")->getTrimmedString(0);
            GroupPtr group = getGroup(groupName);

            {
                Phase::PhaseEnum phase = Phase::PhaseEnumFromString( record->getItem("PHASE")->getTrimmedString(0) );
                group->setInjectionPhase( currentStep , phase );
            }
            {
                GroupInjection::ControlEnum controlMode = GroupInjection::ControlEnumFromString( record->getItem("CONTROL_MODE")->getTrimmedString(0) );
                group->setInjectionControlMode( currentStep , controlMode );
            }

            Phase::PhaseEnum wellPhase = Phase::PhaseEnumFromString( record->getItem("PHASE")->getTrimmedString(0));

            // calculate SI injection rates for the group
            double surfaceInjectionRate = record->getItem("SURFACE_TARGET")->getRawDouble(0);
            surfaceInjectionRate = convertInjectionRateToSI(surfaceInjectionRate, wellPhase, *deck->getActiveUnitSystem());
            double reservoirInjectionRate = record->getItem("RESV_TARGET")->getSIDouble(0);

            group->setSurfaceMaxRate( currentStep , surfaceInjectionRate);
            group->setReservoirMaxRate( currentStep , reservoirInjectionRate);
            group->setTargetReinjectFraction( currentStep , record->getItem("REINJ_TARGET")->getSIDouble(0));
            group->setTargetVoidReplacementFraction( currentStep , record->getItem("VOIDAGE_TARGET")->getSIDouble(0));

            group->setProductionGroup(currentStep, false);
        }
    }

    void Schedule::handleGCONPROD(DeckKeywordConstPtr keyword, size_t currentStep) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& groupName = record->getItem("GROUP")->getTrimmedString(0);
            GroupPtr group = getGroup(groupName);
            {
                GroupProduction::ControlEnum controlMode = GroupProduction::ControlEnumFromString( record->getItem("CONTROL_MODE")->getTrimmedString(0) );
                group->setProductionControlMode( currentStep , controlMode );
            }
            group->setOilTargetRate( currentStep , record->getItem("OIL_TARGET")->getSIDouble(0));
            group->setGasTargetRate( currentStep , record->getItem("GAS_TARGET")->getSIDouble(0));
            group->setWaterTargetRate( currentStep , record->getItem("WATER_TARGET")->getSIDouble(0));
            group->setLiquidTargetRate( currentStep , record->getItem("LIQUID_TARGET")->getSIDouble(0));
            {
                GroupProductionExceedLimit::ActionEnum exceedAction = GroupProductionExceedLimit::ActionEnumFromString(record->getItem("EXCEED_PROC")->getTrimmedString(0) );
                group->setProductionExceedLimitAction( currentStep , exceedAction );
            }

            group->setProductionGroup(currentStep, true);
        }
    }

    void Schedule::handleTUNING(DeckKeywordConstPtr keyword, size_t currentStep) {

        int numrecords = keyword->size();

        if (numrecords > 0) {
            DeckRecordConstPtr record1 = keyword->getRecord(0);

            double TSINIT = record1->getItem("TSINIT")->getSIDouble(0);
            m_tuning->setTSINIT(currentStep, TSINIT);

            double TSMAXZ = record1->getItem("TSMAXZ")->getSIDouble(0);
            m_tuning->setTSMAXZ(currentStep, TSMAXZ);

            double TSMINZ = record1->getItem("TSMINZ")->getSIDouble(0);
            m_tuning->setTSMINZ(currentStep, TSMINZ);

            double TSMCHP = record1->getItem("TSMCHP")->getSIDouble(0);
            m_tuning->setTSMCHP(currentStep, TSMCHP);

            double TSFMAX = record1->getItem("TSFMAX")->getRawDouble(0);
            m_tuning->setTSFMAX(currentStep, TSFMAX);

            double TSFMIN = record1->getItem("TSFMIN")->getRawDouble(0);
            m_tuning->setTSFMIN(currentStep, TSFMIN);

            double TSFCNV = record1->getItem("TSFCNV")->getRawDouble(0);
            m_tuning->setTSFCNV(currentStep, TSFCNV);

            double TFDIFF = record1->getItem("TFDIFF")->getRawDouble(0);
            m_tuning->setTFDIFF(currentStep, TFDIFF);

            double THRUPT = record1->getItem("THRUPT")->getRawDouble(0);
            m_tuning->setTHRUPT(currentStep, THRUPT);

            DeckItemConstPtr TMAXWCdeckItem = record1->getItem("TMAXWC");
            if (TMAXWCdeckItem->hasValue(0)) {
                double TMAXWC = TMAXWCdeckItem->getSIDouble(0);
                m_tuning->setTMAXWC(currentStep, TMAXWC);
            }
        }


        if (numrecords > 1) {
            DeckRecordConstPtr record2 = keyword->getRecord(1);

            double TRGTTE = record2->getItem("TRGTTE")->getRawDouble(0);
            m_tuning->setTRGTTE(currentStep, TRGTTE);

            double TRGCNV = record2->getItem("TRGCNV")->getRawDouble(0);
            m_tuning->setTRGCNV(currentStep, TRGCNV);

            double TRGMBE = record2->getItem("TRGMBE")->getRawDouble(0);
            m_tuning->setTRGMBE(currentStep, TRGMBE);

            double TRGLCV = record2->getItem("TRGLCV")->getRawDouble(0);
            m_tuning->setTRGLCV(currentStep, TRGLCV);

            double XXXTTE = record2->getItem("XXXTTE")->getRawDouble(0);
            m_tuning->setXXXTTE(currentStep, XXXTTE);

            double XXXCNV = record2->getItem("XXXCNV")->getRawDouble(0);
            m_tuning->setXXXCNV(currentStep, XXXCNV);

            double XXXMBE = record2->getItem("XXXMBE")->getRawDouble(0);
            m_tuning->setXXXMBE(currentStep, XXXMBE);

            double XXXLCV = record2->getItem("XXXLCV")->getRawDouble(0);
            m_tuning->setXXXLCV(currentStep, XXXLCV);

            double XXXWFL = record2->getItem("XXXWFL")->getRawDouble(0);
            m_tuning->setXXXWFL(currentStep, XXXWFL);

            double TRGFIP = record2->getItem("TRGFIP")->getRawDouble(0);
            m_tuning->setTRGFIP(currentStep, TRGFIP);

            DeckItemConstPtr TRGSFTdeckItem = record2->getItem("TRGSFT");
            if (TRGSFTdeckItem->hasValue(0)) {
                double TRGSFT = TRGSFTdeckItem->getRawDouble(0);
                m_tuning->setTRGSFT(currentStep, TRGSFT);
            }

            double THIONX = record2->getItem("THIONX")->getRawDouble(0);
            m_tuning->setTHIONX(currentStep, THIONX);

            int TRWGHT = record2->getItem("TRWGHT")->getInt(0);
            m_tuning->setTRWGHT(currentStep, TRWGHT);
        }


        if (numrecords > 2) {
            DeckRecordConstPtr record3 = keyword->getRecord(2);

            int NEWTMX = record3->getItem("NEWTMX")->getInt(0);
            m_tuning->setNEWTMX(currentStep, NEWTMX);

            int NEWTMN = record3->getItem("NEWTMN")->getInt(0);
            m_tuning->setNEWTMN(currentStep, NEWTMN);

            int LITMAX = record3->getItem("LITMAX")->getInt(0);
            m_tuning->setLITMAX(currentStep, LITMAX);

            int LITMIN = record3->getItem("LITMIN")->getInt(0);
            m_tuning->setLITMIN(currentStep, LITMIN);

            int MXWSIT = record3->getItem("MXWSIT")->getInt(0);
            m_tuning->setMXWSIT(currentStep, MXWSIT);

            int MXWPIT = record3->getItem("MXWPIT")->getInt(0);
            m_tuning->setMXWPIT(currentStep, MXWPIT);

            double DDPLIM = record3->getItem("DDPLIM")->getSIDouble(0);
            m_tuning->setDDPLIM(currentStep, DDPLIM);

            double DDSLIM = record3->getItem("DDSLIM")->getRawDouble(0);
            m_tuning->setDDSLIM(currentStep, DDSLIM);

            double TRGDPR = record3->getItem("TRGDPR")->getSIDouble(0);
            m_tuning->setTRGDPR(currentStep, TRGDPR);

            DeckItemConstPtr XXXDPRdeckItem = record3->getItem("XXXDPR");
            if (XXXDPRdeckItem->hasValue(0)) {
                double XXXDPR = XXXDPRdeckItem->getSIDouble(0);
                m_tuning->setXXXDPR(currentStep, XXXDPR);
            }
        }
    }


    void Schedule::handleNOSIM() {
        nosim = true;
    }

    void Schedule::handleRPTRST(DeckKeywordConstPtr keyword, size_t currentStep, IOConfigPtr ioConfig) {
        DeckRecordConstPtr record = keyword->getRecord(0);

        size_t basic = 1;
        size_t freq  = 0;

        DeckItemConstPtr item = record->getItem(0);

        for (size_t index = 0; index < item->size(); ++index) {

            if (item->hasValue(index)) {
                std::string mnemonics = item->getString(index);
                size_t found_basic = mnemonics.find("BASIC=");
                if (found_basic != std::string::npos) {
                    std::string basic_no = mnemonics.substr(found_basic+6, mnemonics.size());
                    basic = boost::lexical_cast<size_t>(basic_no);
                }

                size_t found_freq = mnemonics.find("FREQ=");
                if (found_freq != std::string::npos) {
                    std::string freq_no = mnemonics.substr(found_freq+5, mnemonics.size());
                    freq = boost::lexical_cast<size_t>(freq_no);
                }
            }
        }

        ioConfig->handleRPTRSTBasic(m_timeMap, currentStep, basic, freq);
    }


    void Schedule::handleCOMPDAT(DeckKeywordConstPtr keyword, size_t currentStep) {
        std::map<std::string , std::vector< CompletionPtr> > completionMapList = Completion::completionsFromCOMPDATKeyword( keyword );
        std::map<std::string , std::vector< CompletionPtr> >::iterator iter;

        for( iter= completionMapList.begin(); iter != completionMapList.end(); iter++) {
            const std::string wellName = iter->first;
            WellPtr well = getWell(wellName);
            well->addCompletions(currentStep, iter->second);
        }
    }

    void Schedule::handleWGRUPCON(DeckKeywordConstPtr keyword, size_t currentStep) {
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& wellName = record->getItem("WELL")->getTrimmedString(0);
            WellPtr well = getWell(wellName);

            bool availableForGroupControl = convertEclipseStringToBool(record->getItem("GROUP_CONTROLLED")->getTrimmedString(0));
            well->setAvailableForGroupControl(currentStep, availableForGroupControl);

            well->setGuideRate(currentStep, record->getItem("GUIDE_RATE")->getRawDouble(0));

            if (!record->getItem("PHASE")->defaultApplied(0)) {
                std::string guideRatePhase = record->getItem("PHASE")->getTrimmedString(0);
                well->setGuideRatePhase(currentStep, GuideRate::GuideRatePhaseEnumFromString(guideRatePhase));
            } else
                well->setGuideRatePhase(currentStep, GuideRate::UNDEFINED);

            well->setGuideRateScalingFactor(currentStep, record->getItem("SCALING_FACTOR")->getRawDouble(0));
        }
    }

    void Schedule::handleGRUPTREE(DeckKeywordConstPtr keyword, size_t currentStep) {
        GroupTreePtr currentTree = m_rootGroupTree->get(currentStep);
        GroupTreePtr newTree = currentTree->deepCopy();
        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);
            const std::string& childName = record->getItem("CHILD_GROUP")->getTrimmedString(0);
            const std::string& parentName = record->getItem("PARENT_GROUP")->getTrimmedString(0);
            newTree->updateTree(childName, parentName);

            if (!hasGroup(parentName))
                addGroup( parentName , currentStep );

            if (!hasGroup(childName))
                addGroup( childName , currentStep );
        }
        m_rootGroupTree->add(currentStep, newTree);
    }

    void Schedule::handleWRFT(DeckKeywordConstPtr keyword, size_t currentStep) {

        /* Rule for handling RFT: Request current RFT data output for specified wells, plus output when
         * any well is subsequently opened
         */

        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);

            const std::string& wellNamePattern = record->getItem("WELL")->getTrimmedString(0);
            const std::vector<WellPtr> wells = getWells(wellNamePattern);

            for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
                WellPtr well = *wellIter;
                well->setRFTActive(currentStep, true);
                size_t numStep = m_timeMap->numTimesteps();
                if(currentStep<numStep){
                    well->setRFTActive(currentStep+1, false);
                }
            }
        }

        for (auto iter = m_wells.begin(); iter != m_wells.end(); ++iter) {
            WellPtr well = *iter;
            well->setRFTForWellWhenFirstOpen(m_timeMap->numTimesteps(), currentStep);
        }
    }

    void Schedule::handleWRFTPLT(DeckKeywordConstPtr keyword,  size_t currentStep) {

        for (size_t recordNr = 0; recordNr < keyword->size(); recordNr++) {
            DeckRecordConstPtr record = keyword->getRecord(recordNr);

            const std::string& wellNamePattern = record->getItem("WELL")->getTrimmedString(0);
            const std::vector<WellPtr> wells = getWells(wellNamePattern);

            RFTConnections::RFTEnum RFTKey = RFTConnections::RFTEnumFromString(record->getItem("OUTPUT_RFT")->getTrimmedString(0));

            PLTConnections::PLTEnum PLTKey = PLTConnections::PLTEnumFromString(record->getItem("OUTPUT_PLT")->getTrimmedString(0));

            for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
                WellPtr well = *wellIter;
                switch(RFTKey){
                    case RFTConnections::RFTEnum::YES:
                        well->setRFTActive(currentStep, true);
                        break;
                    case RFTConnections::RFTEnum::REPT:
                        well->setRFTActive(currentStep, true);
                        break;
                    case RFTConnections::RFTEnum::TIMESTEP:
                        well->setRFTActive(currentStep, true);
                        break;
                    case RFTConnections::RFTEnum::FOPN:
                        well->setRFTForWellWhenFirstOpen(m_timeMap->numTimesteps(),currentStep);
                        break;
                    case RFTConnections::RFTEnum::NO:
                        well->setRFTActive(currentStep, false);
                        break;
                }

                switch(PLTKey){
                    case PLTConnections::PLTEnum::YES:
                        well->setPLTActive(currentStep, true);
                        break;
                    case PLTConnections::PLTEnum::REPT:
                        well->setPLTActive(currentStep, true);
                        break;
                    case PLTConnections::PLTEnum::TIMESTEP:
                        well->setPLTActive(currentStep, true);
                        break;
                    case PLTConnections::PLTEnum::NO:
                        well->setPLTActive(currentStep, false);
                        break;
                }
            }
        }
    }

    TimeMapConstPtr Schedule::getTimeMap() const {
        return m_timeMap;
    }

    GroupTreePtr Schedule::getGroupTree(size_t timeStep) const {
        return m_rootGroupTree->get(timeStep);
    }

    void Schedule::addWell(const std::string& wellName, DeckRecordConstPtr record, size_t timeStep) {
        // We change from eclipse's 1 - n, to a 0 - n-1 solution
        int headI = record->getItem("HEAD_I")->getInt(0) - 1;
        int headJ = record->getItem("HEAD_J")->getInt(0) - 1;
        Phase::PhaseEnum preferredPhase = Phase::PhaseEnumFromString(record->getItem("PHASE")->getTrimmedString(0));
        Value<double> refDepth("REF_DEPTH");
        WellPtr well;
        auto refDepthItem = record->getItem("REF_DEPTH");

        if (refDepthItem->hasValue(0))
            refDepth.setValue( refDepthItem->getSIDouble(0));

        well = std::make_shared<Well>(wellName, m_grid , headI, headJ, refDepth, preferredPhase, m_timeMap , timeStep);
        m_wells.insert( wellName  , well);
    }

    size_t Schedule::numWells() const {
        return m_wells.size();
    }

    size_t Schedule::numWells(size_t timestep) const {
      std::vector<WellConstPtr> wells = getWells(timestep);
      return wells.size();
    }

    bool Schedule::hasWell(const std::string& wellName) const {
        return m_wells.hasKey( wellName );
    }

    std::vector<WellConstPtr> Schedule::getWells() const {
        return getWells(m_timeMap->size()-1);
    }

    std::vector<WellConstPtr> Schedule::getWells(size_t timeStep) const {
        if (timeStep >= m_timeMap->size()) {
            throw std::invalid_argument("Timestep to large");
        }

        std::vector<WellConstPtr> wells;
        for (auto iter = m_wells.begin(); iter != m_wells.end(); ++iter) {
            WellConstPtr well = *iter;
            if (well->hasBeenDefined(timeStep)) {
                wells.push_back(well);
            }
        }
        return wells;
    }

    WellPtr Schedule::getWell(const std::string& wellName) const {
        return m_wells.get( wellName );
    }

    std::vector<WellPtr> Schedule::getWells(const std::string& wellNamePattern) const {
        std::vector<WellPtr> wells;
        size_t wildcard_pos = wellNamePattern.find("*");
        if (wildcard_pos == wellNamePattern.length()-1) {
            for (auto wellIter = m_wells.begin(); wellIter != m_wells.end(); ++wellIter) {
                WellPtr well = *wellIter;
                if (wellNamePattern.compare (0, wildcard_pos, well->name(), 0, wildcard_pos) == 0) {
                    wells.push_back (well);
                }
            }
        }
        else {
            wells.push_back(getWell(wellNamePattern));
        }
        return wells;
    }

    void Schedule::addGroup(const std::string& groupName, size_t timeStep) {
        if (!m_timeMap) {
            throw std::invalid_argument("TimeMap is null, can't add group named: " + groupName);
        }
        GroupPtr group(new Group(groupName, m_timeMap , timeStep));
        m_groups[ groupName ] = group;
    }

    size_t Schedule::numGroups() const {
        return m_groups.size();
    }

    bool Schedule::hasGroup(const std::string& groupName) const {
        return m_groups.find(groupName) != m_groups.end();
    }

    bool Schedule::initOnly() const {
        return nosim;
    }

    GroupPtr Schedule::getGroup(const std::string& groupName) const {
        if (hasGroup(groupName)) {
            return m_groups.at(groupName);
        } else
            throw std::invalid_argument("Group: " + groupName + " does not exist");
    }


    void Schedule::addWellToGroup( GroupPtr newGroup , WellPtr well , size_t timeStep) {
        const std::string currentGroupName = well->getGroupName(timeStep);
        if (currentGroupName != "") {
            GroupPtr currentGroup = getGroup( currentGroupName );
            currentGroup->delWell( timeStep , well->name());
        }
        well->setGroupName(timeStep , newGroup->name());
        newGroup->addWell(timeStep , well);
    }

    void Schedule::checkUnhandledKeywords(DeckConstPtr deck) const {
        if (deck->hasKeyword("COMPORD")) {
            auto compordKeyword = deck->getKeyword("COMPORD");
            for (auto record = compordKeyword->begin(); record != compordKeyword->end(); ++record) {
                auto methodItem = (*record)->getItem(1);
                if (methodItem->getString(0) != "TRACK")
                    throw std::invalid_argument("Can only handle TRACK in well order");
            }
        }
    }

    double Schedule::convertInjectionRateToSI(double rawRate, WellInjector::TypeEnum wellType, const Opm::UnitSystem &unitSystem) {
        switch (wellType) {
        case WellInjector::MULTI:
            // multi-phase controlled injectors are a really funny
            // construct in Eclipse: the quantity controlled for is
            // not physically meaningful, i.e. Eclipse adds up
            // MCFT/day and STB/day.
            throw std::logic_error("There is no generic way to handle multi-phase injectors at this level!");

        case WellInjector::OIL:
        case WellInjector::WATER:
            return rawRate * unitSystem.parse("LiquidSurfaceVolume/Time")->getSIScaling();

        case WellInjector::GAS:
            return rawRate * unitSystem.parse("GasSurfaceVolume/Time")->getSIScaling();

        default:
            throw std::logic_error("Unknown injector type");
        }
    }

    double Schedule::convertInjectionRateToSI(double rawRate, Phase::PhaseEnum wellPhase, const Opm::UnitSystem &unitSystem) {
        switch (wellPhase) {
        case Phase::OIL:
        case Phase::WATER:
            return rawRate * unitSystem.parse("LiquidSurfaceVolume/Time")->getSIScaling();

        case Phase::GAS:
            return rawRate * unitSystem.parse("GasSurfaceVolume/Time")->getSIScaling();

        default:
            throw std::logic_error("Unknown injection phase");
        }
    }

    bool Schedule::convertEclipseStringToBool(const std::string& eclipseString) {
        std::string lowerTrimmed = boost::algorithm::to_lower_copy(eclipseString);
        boost::algorithm::trim(lowerTrimmed);

        if (lowerTrimmed == "y" || lowerTrimmed == "yes") {
            return true;
        }
        else if (lowerTrimmed == "n" || lowerTrimmed == "no") {
            return false;
        }
        else throw std::invalid_argument("String " + eclipseString + " not recognized as a boolean-convertible string.");
    }

    size_t Schedule::getMaxNumCompletionsForWells(size_t timestep) const {
      size_t ncwmax = 0;
      const std::vector<WellConstPtr>& wells = getWells();
      for (auto wellIter=wells.begin(); wellIter != wells.end(); ++wellIter) {
        WellConstPtr wellPtr = *wellIter;
        CompletionSetConstPtr completionsSetPtr = wellPtr->getCompletions(timestep);

        if (completionsSetPtr->size() > ncwmax )
          ncwmax = completionsSetPtr->size();

      }
      return ncwmax;
    }

    TuningPtr Schedule::getTuning() const {
      return m_tuning;
    }
}
