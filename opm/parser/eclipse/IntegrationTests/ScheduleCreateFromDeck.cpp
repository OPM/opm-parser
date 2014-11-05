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

#define BOOST_TEST_MODULE ScheduleIntegrationTests
#include <math.h>

#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/CompletionSet.hpp>
#include <opm/parser/eclipse/Units/ConversionFactors.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/GroupTreeNode.hpp>

using namespace Opm;


BOOST_AUTO_TEST_CASE(CreateSchedule) {

    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE1");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));
    TimeMapConstPtr timeMap = sched->getTimeMap();
    BOOST_CHECK_EQUAL(boost::posix_time::ptime(boost::gregorian::date(2007, boost::gregorian::May, 10)), sched->getStartTime());
    BOOST_CHECK_EQUAL(9U, timeMap->size());
    BOOST_CHECK( deck->hasKeyword("NETBALAN") );
}


BOOST_AUTO_TEST_CASE(CreateSchedule_Comments_After_Keywords) {

    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_COMMENTS_AFTER_KEYWORDS");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));
    TimeMapConstPtr timeMap = sched->getTimeMap();
    BOOST_CHECK_EQUAL(boost::posix_time::ptime(boost::gregorian::date(2007, boost::gregorian::May, 10)), sched->getStartTime());
    BOOST_CHECK_EQUAL(9U, timeMap->size());
}


BOOST_AUTO_TEST_CASE(WCONPROD_MissingCmode) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_MISSING_CMODE");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    BOOST_CHECK_NO_THROW( new Schedule(deck) );
}


BOOST_AUTO_TEST_CASE(WCONPROD_Missing_DATA) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_CMODE_MISSING_DATA");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    
    BOOST_CHECK_THROW( new Schedule(deck) , std::invalid_argument );
}



BOOST_AUTO_TEST_CASE(WellTesting) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELLS2");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));

    BOOST_CHECK_EQUAL(3U, sched->numWells());
    BOOST_CHECK(sched->hasWell("W_1"));
    BOOST_CHECK(sched->hasWell("W_2"));
    BOOST_CHECK(sched->hasWell("W_3"));

    {
        WellPtr well2 = sched->getWell("W_2");
        BOOST_CHECK_EQUAL( 0 , well2->getProductionPropertiesCopy(2).resvRate);
        BOOST_CHECK_CLOSE( 777/Metric::Time , well2->getProductionPropertiesCopy(7).resvRate , 0.0001);
        BOOST_CHECK_EQUAL( 0 , well2->getProductionPropertiesCopy(8).resvRate);

        BOOST_CHECK_EQUAL( WellCommon::SHUT , well2->getStatus(3));

        {
            const WellProductionProperties& prop3 = well2->getProductionProperties(3);
            BOOST_CHECK_EQUAL( WellProducer::ORAT , prop3.controlMode);
            BOOST_CHECK(  prop3.hasProductionControl(WellProducer::ORAT));
            BOOST_CHECK(  prop3.hasProductionControl(WellProducer::GRAT));
            BOOST_CHECK(  prop3.hasProductionControl(WellProducer::WRAT));
        }

        // BOOST_CHECK( !well2->getProductionProperties(8).hasProductionControl(WellProducer::GRAT));
    }


    {
        WellPtr well3 = sched->getWell("W_3");

        BOOST_CHECK_EQUAL( WellCommon::AUTO , well3->getStatus(3));
        BOOST_CHECK_EQUAL( 0 , well3->getProductionPropertiesCopy(2).liquidRate);
        
        {
            const WellProductionProperties& prop7 = well3->getProductionProperties(7);
            BOOST_CHECK_CLOSE( 999/Metric::Time , prop7.liquidRate , 0.001);
            BOOST_CHECK_EQUAL( WellProducer::RESV, prop7.controlMode);
        }
        BOOST_CHECK_EQUAL( 0 , well3->getProductionPropertiesCopy(8).liquidRate);
    }

    {
        WellPtr well1 = sched->getWell("W_1");

        BOOST_CHECK(well1->getProductionPropertiesCopy(0).predictionMode);
        BOOST_CHECK_EQUAL(0, well1->getProductionPropertiesCopy(0).oilRate);

        BOOST_CHECK_EQUAL(0, well1->getProductionPropertiesCopy(1).oilRate);
        BOOST_CHECK_EQUAL(0, well1->getProductionPropertiesCopy(2).oilRate);

        BOOST_CHECK(!well1->getProductionPropertiesCopy(3).predictionMode);
        BOOST_CHECK_CLOSE(4000/Metric::Time , well1->getProductionPropertiesCopy(3).oilRate , 0.001);
        BOOST_CHECK_CLOSE(4000/Metric::Time , well1->getProductionPropertiesCopy(4).oilRate , 0.001);
        BOOST_CHECK_CLOSE(4000/Metric::Time , well1->getProductionPropertiesCopy(5).oilRate , 0.001);
        BOOST_CHECK_CLOSE(4/Metric::Time      , well1->getProductionPropertiesCopy(3).waterRate , 0.001);
        BOOST_CHECK_CLOSE(12345/Metric::Time  , well1->getProductionPropertiesCopy(3).gasRate , 0.001);
        BOOST_CHECK_CLOSE(4/Metric::Time      , well1->getProductionPropertiesCopy(4).waterRate , 0.001);
        BOOST_CHECK_CLOSE(12345/Metric::Time  , well1->getProductionPropertiesCopy(4).gasRate , 0.001);
        BOOST_CHECK_CLOSE(4/Metric::Time      , well1->getProductionPropertiesCopy(5).waterRate , 0.001);
        BOOST_CHECK_CLOSE(12345/Metric::Time  , well1->getProductionPropertiesCopy(5).gasRate , 0.001);


        BOOST_CHECK(!well1->getProductionPropertiesCopy(6).predictionMode);
        BOOST_CHECK_CLOSE(14000/Metric::Time , well1->getProductionPropertiesCopy(6).oilRate , 0.001);

        BOOST_CHECK(well1->getProductionPropertiesCopy(7).predictionMode);
        BOOST_CHECK_CLOSE(11000/Metric::Time , well1->getProductionPropertiesCopy(7).oilRate , 0.001);
        BOOST_CHECK_CLOSE(44/Metric::Time    , well1->getProductionPropertiesCopy(7).waterRate , 0.001);
        BOOST_CHECK_CLOSE(188/Metric::Time   , well1->getProductionPropertiesCopy(7).gasRate , 0.001);

        BOOST_CHECK(!well1->getProductionPropertiesCopy(8).predictionMode);
        BOOST_CHECK_CLOSE(13000/Metric::Time , well1->getProductionPropertiesCopy(8).oilRate , 0.001);
        
        BOOST_CHECK_CLOSE(123.00 * Metric::Pressure , well1->getInjectionPropertiesCopy(10).BHPLimit, 0.001);
        BOOST_CHECK_CLOSE(678.00 * Metric::Pressure , well1->getInjectionPropertiesCopy(10).THPLimit, 0.001);
        
        {
            const WellInjectionProperties& prop11 = well1->getInjectionProperties(11);
            BOOST_CHECK_CLOSE(5000/Metric::Time , prop11.surfaceInjectionRate, 0.001);
            BOOST_CHECK_EQUAL( WellInjector::RATE  , prop11.controlMode);
            BOOST_CHECK_EQUAL( WellCommon::OPEN , well1->getStatus( 11 ));
        }



        BOOST_CHECK( well1->isInjector(9));
        {
            const WellInjectionProperties& prop9 = well1->getInjectionProperties(9);
            BOOST_CHECK_CLOSE(20000/Metric::Time ,  prop9.surfaceInjectionRate  , 0.001);
            BOOST_CHECK_CLOSE(200000/Metric::Time , prop9.reservoirInjectionRate, 0.001);
            BOOST_CHECK_CLOSE(6891 * Metric::Pressure , prop9.BHPLimit, 0.001);
            BOOST_CHECK_CLOSE(0 , prop9.THPLimit , 0.001);
            BOOST_CHECK_EQUAL( WellInjector::RESV  , prop9.controlMode);
            BOOST_CHECK(  prop9.hasInjectionControl(WellInjector::RATE ));
            BOOST_CHECK(  prop9.hasInjectionControl(WellInjector::RESV ));
            BOOST_CHECK( !prop9.hasInjectionControl(WellInjector::THP));
            BOOST_CHECK( !prop9.hasInjectionControl(WellInjector::BHP));
        }

        
        BOOST_CHECK_EQUAL( WellCommon::SHUT , well1->getStatus( 12 ));
        BOOST_CHECK(  well1->getInjectionPropertiesCopy(12).hasInjectionControl(WellInjector::RATE ));
        BOOST_CHECK( !well1->getInjectionPropertiesCopy(12).hasInjectionControl(WellInjector::RESV));
        BOOST_CHECK(  well1->getInjectionPropertiesCopy(12).hasInjectionControl(WellInjector::THP ));
        BOOST_CHECK(  well1->getInjectionPropertiesCopy(12).hasInjectionControl(WellInjector::BHP ));

    }
}

BOOST_AUTO_TEST_CASE(WellTestCOMPDAT) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELLS2");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));

    BOOST_CHECK_EQUAL(3U, sched->numWells());
    BOOST_CHECK(sched->hasWell("W_1"));
    BOOST_CHECK(sched->hasWell("W_2"));
    BOOST_CHECK(sched->hasWell("W_3"));
    {
        WellPtr well1 = sched->getWell("W_1");
        BOOST_CHECK_CLOSE(13000/Metric::Time , well1->getProductionPropertiesCopy(8).oilRate , 0.0001);
        CompletionSetConstPtr completions = well1->getCompletions(0);
        BOOST_CHECK_EQUAL(0U, completions->size());

        completions = well1->getCompletions(3);
        BOOST_CHECK_EQUAL(4U, completions->size());

        BOOST_CHECK_EQUAL(OPEN, completions->get(3)->getState());
        BOOST_CHECK_EQUAL(2.2836805555555556e-12 , completions->get(3)->getConnectionTransmissibilityFactor());
        BOOST_CHECK_EQUAL(0.311/Metric::Length, completions->get(3)->getDiameter());
        BOOST_CHECK_EQUAL(3.3, completions->get(3)->getSkinFactor());

        completions = well1->getCompletions(7);
        BOOST_CHECK_EQUAL(4U, completions->size());
        BOOST_CHECK_EQUAL(SHUT, completions->get(3)->getState());
    }
}

BOOST_AUTO_TEST_CASE(GroupTreeTest_GRUPTREE_with_explicit_L0_parenting) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_GRUPTREE_EXPLICIT_PARENTING");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));

    GroupTreeNodePtr rootNode = sched->getGroupTree(0)->getNode("FIELD");


    BOOST_REQUIRE_EQUAL("FIELD", rootNode->name());

    BOOST_CHECK(rootNode->hasChildGroup("FIRST_LEVEL1"));
    GroupTreeNodePtr FIRST_LEVEL1 = rootNode->getChildGroup("FIRST_LEVEL1");
    BOOST_CHECK(rootNode->hasChildGroup("FIRST_LEVEL2"));
    GroupTreeNodePtr FIRST_LEVEL2 = rootNode->getChildGroup("FIRST_LEVEL2");

    BOOST_CHECK(FIRST_LEVEL1->hasChildGroup("SECOND_LEVEL1"));
    GroupTreeNodePtr SECOND_LEVEL1 = FIRST_LEVEL1->getChildGroup("SECOND_LEVEL1");

    BOOST_CHECK(FIRST_LEVEL2->hasChildGroup("SECOND_LEVEL2"));
    GroupTreeNodePtr SECOND_LEVEL2 = FIRST_LEVEL2->getChildGroup("SECOND_LEVEL2");

    BOOST_CHECK(SECOND_LEVEL1->hasChildGroup("THIRD_LEVEL1"));
    GroupTreeNodePtr THIRD_LEVEL1 = SECOND_LEVEL1->getChildGroup("THIRD_LEVEL1");
}


BOOST_AUTO_TEST_CASE(GroupTreeTest_GRUPTREE_correct) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELSPECS_GRUPTREE");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr schedule(new Schedule(deck));
    
    BOOST_CHECK( schedule->hasGroup( "FIELD" ));
    BOOST_CHECK( schedule->hasGroup( "PROD" ));
    BOOST_CHECK( schedule->hasGroup( "INJE" ));
    BOOST_CHECK( schedule->hasGroup( "MANI-PROD" ));
    BOOST_CHECK( schedule->hasGroup( "MANI-INJ" ));
    BOOST_CHECK( schedule->hasGroup( "DUMMY-PROD" ));
    BOOST_CHECK( schedule->hasGroup( "DUMMY-INJ" ));
}



BOOST_AUTO_TEST_CASE(GroupTreeTest_WELSPECS_AND_GRUPTREE_correct_iter_function) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELSPECS_GROUPS");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr schedule(new Schedule(deck));

    // Time 0, only from WELSPECS
    GroupTreeNodeConstPtr root = schedule->getGroupTree(0)->getNode("FIELD");

    int iter_counted = 0;

    for (auto iter=root->begin(); iter != root->end(); ++iter) 
        iter_counted++;
    
    BOOST_CHECK_EQUAL(2, iter_counted);
    // Time 1, a new group added in tree
    iter_counted = 0;

    root = schedule->getGroupTree(1)->getNode("FIELD");
    for (auto iter=root->begin(); iter != root->end(); ++iter) {
       iter_counted++;
    }
    BOOST_CHECK_EQUAL(3, iter_counted);
}

BOOST_AUTO_TEST_CASE(GroupTreeTest_WELSPECS_AND_GRUPTREE_correct_tree) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELSPECS_GROUPS");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr schedule(new Schedule(deck));

    // Time 0, only from WELSPECS
    GroupTreeNodePtr root0 = schedule->getGroupTree(0)->getNode("FIELD");
    BOOST_REQUIRE_EQUAL("FIELD", root0->name());
    BOOST_CHECK(root0->hasChildGroup("GROUP_BJARNE"));
    GroupTreeNodePtr GROUP_BJARNE = root0->getChildGroup("GROUP_BJARNE");
    BOOST_CHECK_EQUAL("GROUP_BJARNE", GROUP_BJARNE->name());

    BOOST_CHECK(root0->hasChildGroup("GROUP_ODD"));
    GroupTreeNodePtr GROUP_ODD = root0->getChildGroup("GROUP_ODD");
    BOOST_CHECK_EQUAL("GROUP_ODD", GROUP_ODD->name());

    // Time 1, now also from GRUPTREE
    GroupTreeNodePtr root1 = schedule->getGroupTree(1)->getNode("FIELD");
    BOOST_REQUIRE_EQUAL("FIELD", root1->name());
    BOOST_CHECK(root1->hasChildGroup("GROUP_BJARNE"));
    GroupTreeNodePtr GROUP_BJARNE1 = root1->getChildGroup("GROUP_BJARNE");
    BOOST_CHECK_EQUAL("GROUP_BJARNE", GROUP_BJARNE1->name());

    BOOST_CHECK(root1->hasChildGroup("GROUP_ODD"));
    GroupTreeNodePtr GROUP_ODD1 = root1->getChildGroup("GROUP_ODD");
    BOOST_CHECK_EQUAL("GROUP_ODD", GROUP_ODD1->name());

    // - from GRUPTREE
    
    BOOST_CHECK(GROUP_BJARNE1->hasChildGroup("GROUP_BIRGER"));
    GroupTreeNodePtr GROUP_BIRGER = GROUP_BJARNE1->getChildGroup("GROUP_BIRGER");
    BOOST_CHECK_EQUAL("GROUP_BIRGER", GROUP_BIRGER->name());

    BOOST_CHECK(root1->hasChildGroup("GROUP_NEW"));
    GroupTreeNodePtr GROUP_NEW = root1->getChildGroup("GROUP_NEW");
    BOOST_CHECK_EQUAL("GROUP_NEW", GROUP_NEW->name());
    
    BOOST_CHECK(GROUP_NEW->hasChildGroup("GROUP_NILS"));
    GroupTreeNodePtr GROUP_NILS = GROUP_NEW->getChildGroup("GROUP_NILS");
    BOOST_CHECK_EQUAL("GROUP_NILS", GROUP_NILS->name());
}

BOOST_AUTO_TEST_CASE(GroupTreeTest_GRUPTREE_WITH_REPARENT_correct_tree) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_GROUPS_REPARENT");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr schedule(new Schedule(deck));


    // Time , from  first GRUPTREE
    GroupTreeNodePtr root0 = schedule->getGroupTree(0)->getNode("FIELD");
    BOOST_REQUIRE_EQUAL("FIELD", root0->name());
    BOOST_CHECK(root0->hasChildGroup("GROUP_BJARNE"));
    GroupTreeNodePtr GROUP_BJARNE0 = root0->getChildGroup("GROUP_BJARNE");
    BOOST_CHECK_EQUAL("GROUP_BJARNE", GROUP_BJARNE0->name());

    BOOST_CHECK(root0->hasChildGroup("GROUP_NEW"));
    GroupTreeNodePtr GROUP_NEW0 = root0->getChildGroup("GROUP_NEW");
    BOOST_CHECK_EQUAL("GROUP_NEW", GROUP_NEW0->name());

    
    BOOST_CHECK(GROUP_BJARNE0->hasChildGroup("GROUP_BIRGER"));
    GroupTreeNodePtr GROUP_BIRGER0 = GROUP_BJARNE0->getChildGroup("GROUP_BIRGER");
    BOOST_CHECK_EQUAL("GROUP_BIRGER", GROUP_BIRGER0->name());

    BOOST_CHECK(GROUP_NEW0->hasChildGroup("GROUP_NILS"));
    GroupTreeNodePtr GROUP_NILS0 = GROUP_NEW0->getChildGroup("GROUP_NILS");
    BOOST_CHECK_EQUAL("GROUP_NILS", GROUP_NILS0->name());
    
    // SÅ den nye strukturen med et barneflytt
}


BOOST_AUTO_TEST_CASE(GroupTreeTest_PrintGrouptree) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELSPECS_GROUPS");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));

    GroupTreePtr rootNode = sched->getGroupTree(0);
    rootNode->printTree(std::cout);

}


BOOST_AUTO_TEST_CASE( WellTestGroups ) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_GROUPS");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched( new Schedule(deck));
    
    BOOST_CHECK_EQUAL( 3U , sched->numGroups() );
    BOOST_CHECK( sched->hasGroup( "INJ" ));
    BOOST_CHECK( sched->hasGroup( "OP" ));

    {
        GroupPtr group = sched->getGroup("INJ");
        BOOST_CHECK_EQUAL( Phase::WATER , group->getInjectionPhase( 3 ));
        BOOST_CHECK_EQUAL( GroupInjection::VREP , group->getInjectionControlMode( 3 ));
        BOOST_CHECK_CLOSE( 10/Metric::Time , group->getSurfaceMaxRate( 3 ) , 0.001);
        BOOST_CHECK_CLOSE( 20/Metric::Time , group->getReservoirMaxRate( 3 ) , 0.001);
        BOOST_CHECK_EQUAL( 0.75 , group->getTargetReinjectFraction( 3 ));
        BOOST_CHECK_EQUAL( 0.95 , group->getTargetVoidReplacementFraction( 3 ));
    
        BOOST_CHECK_EQUAL( Phase::OIL , group->getInjectionPhase( 6 ));
        BOOST_CHECK_EQUAL( GroupInjection::RATE , group->getInjectionControlMode( 6 ));
        BOOST_CHECK_CLOSE( 1000/Metric::Time , group->getSurfaceMaxRate( 6 ) , 0.0001);

        BOOST_CHECK(group->isInjectionGroup(3));
    }
    
    {
        GroupPtr group = sched->getGroup("OP");
        BOOST_CHECK_EQUAL( GroupProduction::ORAT , group->getProductionControlMode(3));
        BOOST_CHECK_CLOSE( 10/Metric::Time , group->getOilTargetRate(3) , 0.001);
        BOOST_CHECK_CLOSE( 20/Metric::Time , group->getWaterTargetRate(3) , 0.001);
        BOOST_CHECK_CLOSE( 30/Metric::Time , group->getGasTargetRate(3) , 0.001);
        BOOST_CHECK_CLOSE( 40/Metric::Time , group->getLiquidTargetRate(3) , 0.001);

        BOOST_CHECK(group->isProductionGroup(3));
    }

}


BOOST_AUTO_TEST_CASE( WellTestGroupAndWellRelation ) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELLS_AND_GROUPS");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched( new Schedule(deck));

    GroupPtr group1 = sched->getGroup("GROUP1");
    GroupPtr group2 = sched->getGroup("GROUP2");
    
    BOOST_CHECK( group1->hasBeenDefined(0) );
    BOOST_CHECK_EQUAL(false , group2->hasBeenDefined(0));
    BOOST_CHECK( group2->hasBeenDefined(1));

    BOOST_CHECK_EQUAL( true , group1->hasWell("W_1" , 0));
    BOOST_CHECK_EQUAL( true , group1->hasWell("W_2" , 0));
    BOOST_CHECK_EQUAL( false, group2->hasWell("W_1" , 0));
    BOOST_CHECK_EQUAL( false, group2->hasWell("W_2" , 0));
    


    BOOST_CHECK_EQUAL( true  , group1->hasWell("W_1" , 1));
    BOOST_CHECK_EQUAL( false , group1->hasWell("W_2" , 1));
    BOOST_CHECK_EQUAL( false , group2->hasWell("W_1" , 1));
    BOOST_CHECK_EQUAL( true  , group2->hasWell("W_2" , 1));
}


BOOST_AUTO_TEST_CASE(WellTestWELSPECSDataLoaded) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELLS2");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));

    BOOST_CHECK_EQUAL(3U, sched->numWells());
    BOOST_CHECK(sched->hasWell("W_1"));
    BOOST_CHECK(sched->hasWell("W_2"));
    BOOST_CHECK(sched->hasWell("W_3"));
    {
        WellConstPtr well1 = sched->getWell("W_1");
        BOOST_CHECK(!well1->hasBeenDefined(2));
        BOOST_CHECK(well1->hasBeenDefined(3));
        BOOST_CHECK_EQUAL(29, well1->getHeadI());
        BOOST_CHECK_EQUAL(36, well1->getHeadJ());
        BOOST_CHECK(!well1->getRefDepthDefaulted());
        BOOST_CHECK_EQUAL(3.33, well1->getRefDepth());

        WellConstPtr well2 = sched->getWell("W_2");
        BOOST_CHECK(!well2->hasBeenDefined(2));
        BOOST_CHECK(well2->hasBeenDefined(3));
        BOOST_CHECK_EQUAL(19, well2->getHeadI());
        BOOST_CHECK_EQUAL(50, well2->getHeadJ());
        BOOST_CHECK(!well2->getRefDepthDefaulted());
        BOOST_CHECK_EQUAL(3.92, well2->getRefDepth());

        WellConstPtr well3 = sched->getWell("W_3");
        BOOST_CHECK(!well3->hasBeenDefined(2));
        BOOST_CHECK(well3->hasBeenDefined(3));
        BOOST_CHECK_EQUAL(30, well3->getHeadI());
        BOOST_CHECK_EQUAL(17, well3->getHeadJ());
        BOOST_CHECK(well3->getRefDepthDefaulted());
    }
}

BOOST_AUTO_TEST_CASE(WellTestWELSPECS_InvalidConfig_Throws) {
    ParserPtr parser(new Parser());
    ParserLogPtr parserLog = parser->getParserLog();

    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELL_INVALID_WELSPECS");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());

    parserLog->clear();
    BOOST_CHECK_NO_THROW(new Schedule(deck, parserLog));
    BOOST_CHECK_EQUAL(parserLog->numErrors(), 1);
}

BOOST_AUTO_TEST_CASE(WellTestWELOPEN_ConfigWithIndexes_Throws) {
    ParserPtr parser(new Parser());
    ParserLogPtr parserLog = parser->getParserLog();

    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELOPEN_INVALID");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());

    parserLog->clear();
    BOOST_CHECK_NO_THROW(new Schedule(deck, parserLog));
    BOOST_CHECK_EQUAL(parserLog->numErrors(), 1);
}


BOOST_AUTO_TEST_CASE(WellTestWELOPENControlsSet) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WELOPEN");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));

    WellConstPtr well1 = sched->getWell("W_1");
    BOOST_CHECK_EQUAL(WellCommon::StatusEnum::OPEN, sched->getWell("W_1")->getStatus(0));
    BOOST_CHECK_EQUAL(WellCommon::StatusEnum::SHUT, sched->getWell("W_1")->getStatus(1));
    BOOST_CHECK_EQUAL(WellCommon::StatusEnum::OPEN, sched->getWell("W_1")->getStatus(2));
    BOOST_CHECK_EQUAL(WellCommon::StatusEnum::STOP, sched->getWell("W_1")->getStatus(3));
    BOOST_CHECK_EQUAL(WellCommon::StatusEnum::AUTO, sched->getWell("W_1")->getStatus(4));
    BOOST_CHECK_EQUAL(WellCommon::StatusEnum::STOP, sched->getWell("W_1")->getStatus(5));
}

BOOST_AUTO_TEST_CASE(WellTestWGRUPCONWellPropertiesSet) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_WGRUPCON");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));
    WellConstPtr well1 = sched->getWell("W_1");
    BOOST_CHECK(well1->isAvailableForGroupControl(0));
    BOOST_CHECK_EQUAL(-1, well1->getGuideRate(0));
    BOOST_CHECK_EQUAL(GuideRate::OIL, well1->getGuideRatePhase(0));
    BOOST_CHECK_EQUAL(1.0, well1->getGuideRateScalingFactor(0));

    WellConstPtr well2 = sched->getWell("W_2");
    BOOST_CHECK(!well2->isAvailableForGroupControl(0));
    BOOST_CHECK_EQUAL(-1, well2->getGuideRate(0));
    BOOST_CHECK_EQUAL(GuideRate::UNDEFINED, well2->getGuideRatePhase(0));
    BOOST_CHECK_EQUAL(1.0, well2->getGuideRateScalingFactor(0));

    WellConstPtr well3 = sched->getWell("W_3");
    BOOST_CHECK(well3->isAvailableForGroupControl(0));
    BOOST_CHECK_EQUAL(100, well3->getGuideRate(0));
    BOOST_CHECK_EQUAL(GuideRate::RAT, well3->getGuideRatePhase(0));
    BOOST_CHECK_EQUAL(0.5, well3->getGuideRateScalingFactor(0));
}


BOOST_AUTO_TEST_CASE(TestDefaultedCOMPDATIJ) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_COMPDAT_DEFAULT_IJ");
    const char * deckString = "\n\
START\n\
\n\
10 MAI 2007 /\n\
\n\
SCHEDULE\n\
WELSPECS \n\
     'W1'        'OP'   11   21  3.33       'OIL'  7* /   \n\
/\n\
COMPDAT \n\
     'W1'   2*    1    1      'OPEN'  1*     32.948      0.311   3047.839  2*         'X'     22.100 /\n\
/\n";
    DeckPtr deck =  parser->parseString(deckString);
    ScheduleConstPtr sched(new Schedule(deck));
    WellConstPtr well = sched->getWell("W1");
    CompletionSetConstPtr completions = well->getCompletions(0);
    BOOST_CHECK_EQUAL( 10 , completions->get(0)->getI() );
    BOOST_CHECK_EQUAL( 20 , completions->get(0)->getJ() );
}


/**
   This is a deck used in the opm-core wellsManager testing; just be
   certain we can parse it.
*/
BOOST_AUTO_TEST_CASE(OpmCode) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/wells_group.data");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    BOOST_CHECK_NO_THROW( new Schedule(deck) );
}



BOOST_AUTO_TEST_CASE(WELLS_SHUT) {
    ParserPtr parser(new Parser());
    boost::filesystem::path scheduleFile("testdata/integration_tests/SCHEDULE/SCHEDULE_SHUT_WELL");
    DeckPtr deck =  parser->parseFile(scheduleFile.string());
    ScheduleConstPtr sched(new Schedule(deck));
    
    WellConstPtr well1 = sched->getWell("W1");
    WellConstPtr well2 = sched->getWell("W2");
    WellConstPtr well3 = sched->getWell("W3");

    BOOST_CHECK_EQUAL( WellCommon::StatusEnum::OPEN , well1->getStatus(1));
    BOOST_CHECK_EQUAL( WellCommon::StatusEnum::OPEN , well2->getStatus(1));
    BOOST_CHECK_EQUAL( WellCommon::StatusEnum::OPEN , well3->getStatus(1));

    
    BOOST_CHECK_EQUAL( WellCommon::StatusEnum::SHUT , well1->getStatus(2));
    BOOST_CHECK_EQUAL( WellCommon::StatusEnum::SHUT , well2->getStatus(2));
    BOOST_CHECK_EQUAL( WellCommon::StatusEnum::SHUT , well3->getStatus(2));
}

