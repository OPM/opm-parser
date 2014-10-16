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


#define BOOST_TEST_MODULE ParserEnumTests
#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>

using namespace Opm;

BOOST_AUTO_TEST_CASE(TestCompletionStateEnumToString) {
    BOOST_CHECK_EQUAL( "AUTO" , CompletionStateEnumToString(AUTO));
    BOOST_CHECK_EQUAL( "OPEN" , CompletionStateEnumToString(OPEN));
    BOOST_CHECK_EQUAL( "SHUT" , CompletionStateEnumToString(SHUT));
}


BOOST_AUTO_TEST_CASE(TestCompletionStateEnumFromString) {
    BOOST_CHECK_THROW( CompletionStateEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( AUTO , CompletionStateEnumFromString("AUTO"));
    BOOST_CHECK_EQUAL( SHUT , CompletionStateEnumFromString("SHUT"));
    BOOST_CHECK_EQUAL( OPEN , CompletionStateEnumFromString("OPEN"));
}



BOOST_AUTO_TEST_CASE(TestCompletionStateEnumLoop) {
    BOOST_CHECK_EQUAL( AUTO , CompletionStateEnumFromString( CompletionStateEnumToString( AUTO ) ));
    BOOST_CHECK_EQUAL( SHUT , CompletionStateEnumFromString( CompletionStateEnumToString( SHUT ) ));
    BOOST_CHECK_EQUAL( OPEN , CompletionStateEnumFromString( CompletionStateEnumToString( OPEN ) ));

    BOOST_CHECK_EQUAL( "AUTO" , CompletionStateEnumToString(CompletionStateEnumFromString(  "AUTO" ) ));
    BOOST_CHECK_EQUAL( "OPEN" , CompletionStateEnumToString(CompletionStateEnumFromString(  "OPEN" ) ));
    BOOST_CHECK_EQUAL( "SHUT" , CompletionStateEnumToString(CompletionStateEnumFromString(  "SHUT" ) ));
}


/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestCompletionDirectionEnumToString)
{
    using namespace CompletionDirection;

    BOOST_CHECK_EQUAL("X", DirectionEnumToString(DirectionEnum::X));
    BOOST_CHECK_EQUAL("Y", DirectionEnumToString(DirectionEnum::Y));
    BOOST_CHECK_EQUAL("Z", DirectionEnumToString(DirectionEnum::Z));
}

BOOST_AUTO_TEST_CASE(TestCompletionDirectionEnumFromString)
{
    using namespace CompletionDirection;

    BOOST_CHECK_THROW(DirectionEnumFromString("XXX"), std::invalid_argument);

    BOOST_CHECK_EQUAL(DirectionEnum::X, DirectionEnumFromString("X"));
    BOOST_CHECK_EQUAL(DirectionEnum::Y, DirectionEnumFromString("Y"));
    BOOST_CHECK_EQUAL(DirectionEnum::Z, DirectionEnumFromString("Z"));
}

BOOST_AUTO_TEST_CASE(TestCompletionDirectionEnumLoop)
{
    using namespace CompletionDirection;

    BOOST_CHECK_EQUAL(DirectionEnum::X, DirectionEnumFromString(DirectionEnumToString(DirectionEnum::X)));
    BOOST_CHECK_EQUAL(DirectionEnum::Y, DirectionEnumFromString(DirectionEnumToString(DirectionEnum::Y)));
    BOOST_CHECK_EQUAL(DirectionEnum::Z, DirectionEnumFromString(DirectionEnumToString(DirectionEnum::Z)));

    BOOST_CHECK_EQUAL("X", DirectionEnumToString(DirectionEnumFromString("X")));
    BOOST_CHECK_EQUAL("Y", DirectionEnumToString(DirectionEnumFromString("Y")));
    BOOST_CHECK_EQUAL("Z", DirectionEnumToString(DirectionEnumFromString("Z")));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestGroupInjectionControlEnumToString) {
    BOOST_CHECK_EQUAL( "NONE" , GroupInjection::ControlEnumToString(GroupInjection::NONE));
    BOOST_CHECK_EQUAL( "RATE" , GroupInjection::ControlEnumToString(GroupInjection::RATE));
    BOOST_CHECK_EQUAL( "RESV" , GroupInjection::ControlEnumToString(GroupInjection::RESV));
    BOOST_CHECK_EQUAL( "REIN" , GroupInjection::ControlEnumToString(GroupInjection::REIN));
    BOOST_CHECK_EQUAL( "VREP" , GroupInjection::ControlEnumToString(GroupInjection::VREP));
    BOOST_CHECK_EQUAL( "FLD"  , GroupInjection::ControlEnumToString(GroupInjection::FLD));
}


BOOST_AUTO_TEST_CASE(TestGroupInjectionControlEnumFromString) {
    BOOST_CHECK_THROW( GroupInjection::ControlEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( GroupInjection::NONE , GroupInjection::ControlEnumFromString("NONE"));
    BOOST_CHECK_EQUAL( GroupInjection::RATE , GroupInjection::ControlEnumFromString("RATE"));
    BOOST_CHECK_EQUAL( GroupInjection::RESV , GroupInjection::ControlEnumFromString("RESV"));
    BOOST_CHECK_EQUAL( GroupInjection::REIN , GroupInjection::ControlEnumFromString("REIN"));
    BOOST_CHECK_EQUAL( GroupInjection::VREP , GroupInjection::ControlEnumFromString("VREP"));
    BOOST_CHECK_EQUAL( GroupInjection::FLD  , GroupInjection::ControlEnumFromString("FLD"));
}



BOOST_AUTO_TEST_CASE(TestGroupInjectionControlEnumLoop) {
    BOOST_CHECK_EQUAL( GroupInjection::NONE , GroupInjection::ControlEnumFromString( GroupInjection::ControlEnumToString( GroupInjection::NONE ) ));
    BOOST_CHECK_EQUAL( GroupInjection::RATE , GroupInjection::ControlEnumFromString( GroupInjection::ControlEnumToString( GroupInjection::RATE ) ));
    BOOST_CHECK_EQUAL( GroupInjection::RESV , GroupInjection::ControlEnumFromString( GroupInjection::ControlEnumToString( GroupInjection::RESV ) ));
    BOOST_CHECK_EQUAL( GroupInjection::REIN , GroupInjection::ControlEnumFromString( GroupInjection::ControlEnumToString( GroupInjection::REIN ) ));
    BOOST_CHECK_EQUAL( GroupInjection::VREP , GroupInjection::ControlEnumFromString( GroupInjection::ControlEnumToString( GroupInjection::VREP ) ));
    BOOST_CHECK_EQUAL( GroupInjection::FLD  , GroupInjection::ControlEnumFromString( GroupInjection::ControlEnumToString( GroupInjection::FLD ) ));

    BOOST_CHECK_EQUAL( "NONE" , GroupInjection::ControlEnumToString(GroupInjection::ControlEnumFromString( "NONE" ) ));
    BOOST_CHECK_EQUAL( "RATE" , GroupInjection::ControlEnumToString(GroupInjection::ControlEnumFromString( "RATE" ) ));
    BOOST_CHECK_EQUAL( "RESV" , GroupInjection::ControlEnumToString(GroupInjection::ControlEnumFromString( "RESV" ) ));
    BOOST_CHECK_EQUAL( "REIN" , GroupInjection::ControlEnumToString(GroupInjection::ControlEnumFromString( "REIN" ) ));
    BOOST_CHECK_EQUAL( "VREP" , GroupInjection::ControlEnumToString(GroupInjection::ControlEnumFromString( "VREP" ) ));
    BOOST_CHECK_EQUAL( "FLD"  , GroupInjection::ControlEnumToString(GroupInjection::ControlEnumFromString( "FLD"  ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestGroupProductionControlEnumToString) {
    BOOST_CHECK_EQUAL( "NONE" , GroupProduction::ControlEnumToString(GroupProduction::NONE));
    BOOST_CHECK_EQUAL( "ORAT" , GroupProduction::ControlEnumToString(GroupProduction::ORAT));
    BOOST_CHECK_EQUAL( "WRAT" , GroupProduction::ControlEnumToString(GroupProduction::WRAT));
    BOOST_CHECK_EQUAL( "GRAT" , GroupProduction::ControlEnumToString(GroupProduction::GRAT));
    BOOST_CHECK_EQUAL( "LRAT" , GroupProduction::ControlEnumToString(GroupProduction::LRAT));
    BOOST_CHECK_EQUAL( "CRAT" , GroupProduction::ControlEnumToString(GroupProduction::CRAT));
    BOOST_CHECK_EQUAL( "RESV" , GroupProduction::ControlEnumToString(GroupProduction::RESV));
    BOOST_CHECK_EQUAL( "PRBL" , GroupProduction::ControlEnumToString(GroupProduction::PRBL));
}


BOOST_AUTO_TEST_CASE(TestGroupProductionControlEnumFromString) {
    BOOST_CHECK_THROW( GroupProduction::ControlEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL(GroupProduction::NONE  , GroupProduction::ControlEnumFromString("NONE"));
    BOOST_CHECK_EQUAL(GroupProduction::ORAT  , GroupProduction::ControlEnumFromString("ORAT"));
    BOOST_CHECK_EQUAL(GroupProduction::WRAT  , GroupProduction::ControlEnumFromString("WRAT"));
    BOOST_CHECK_EQUAL(GroupProduction::GRAT  , GroupProduction::ControlEnumFromString("GRAT"));
    BOOST_CHECK_EQUAL(GroupProduction::LRAT  , GroupProduction::ControlEnumFromString("LRAT"));
    BOOST_CHECK_EQUAL(GroupProduction::CRAT  , GroupProduction::ControlEnumFromString("CRAT"));
    BOOST_CHECK_EQUAL(GroupProduction::RESV  , GroupProduction::ControlEnumFromString("RESV"));
    BOOST_CHECK_EQUAL(GroupProduction::PRBL  , GroupProduction::ControlEnumFromString("PRBL"));
}



BOOST_AUTO_TEST_CASE(TestGroupProductionControlEnumLoop) {
    BOOST_CHECK_EQUAL( GroupProduction::NONE, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::NONE ) ));
    BOOST_CHECK_EQUAL( GroupProduction::ORAT, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::ORAT ) ));
    BOOST_CHECK_EQUAL( GroupProduction::WRAT, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::WRAT ) ));
    BOOST_CHECK_EQUAL( GroupProduction::GRAT, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::GRAT ) ));
    BOOST_CHECK_EQUAL( GroupProduction::LRAT, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::LRAT ) ));
    BOOST_CHECK_EQUAL( GroupProduction::CRAT, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::CRAT ) ));
    BOOST_CHECK_EQUAL( GroupProduction::RESV, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::RESV ) ));
    BOOST_CHECK_EQUAL( GroupProduction::PRBL, GroupProduction::ControlEnumFromString( GroupProduction::ControlEnumToString( GroupProduction::PRBL ) ));

    BOOST_CHECK_EQUAL( "NONE" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "NONE" ) ));
    BOOST_CHECK_EQUAL( "ORAT" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "ORAT" ) ));
    BOOST_CHECK_EQUAL( "WRAT" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "WRAT" ) ));
    BOOST_CHECK_EQUAL( "GRAT" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "GRAT" ) ));
    BOOST_CHECK_EQUAL( "LRAT" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "LRAT" ) ));
    BOOST_CHECK_EQUAL( "CRAT" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "CRAT" ) ));
    BOOST_CHECK_EQUAL( "RESV" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "RESV" ) ));
    BOOST_CHECK_EQUAL( "PRBL" , GroupProduction::ControlEnumToString(GroupProduction::ControlEnumFromString( "PRBL" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestGroupProductionExceedLimitControlEnumToString) {
    BOOST_CHECK_EQUAL( "NONE"     , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::NONE));
    BOOST_CHECK_EQUAL( "CON"      , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::CON));
    BOOST_CHECK_EQUAL( "+CON"     , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::CON_PLUS));
    BOOST_CHECK_EQUAL( "WELL"     , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::WELL));
    BOOST_CHECK_EQUAL( "PLUG"     , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::PLUG));
    BOOST_CHECK_EQUAL( "RATE"     , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::RATE));
}


BOOST_AUTO_TEST_CASE(TestGroupProductionExceedLimitActionEnumFromString) {
    BOOST_CHECK_THROW( GroupProductionExceedLimit::ActionEnumFromString("XXX") , std::invalid_argument );
    
    BOOST_CHECK_EQUAL(GroupProductionExceedLimit::NONE     , GroupProductionExceedLimit::ActionEnumFromString("NONE"));
    BOOST_CHECK_EQUAL(GroupProductionExceedLimit::CON      , GroupProductionExceedLimit::ActionEnumFromString("CON" ));
    BOOST_CHECK_EQUAL(GroupProductionExceedLimit::CON_PLUS , GroupProductionExceedLimit::ActionEnumFromString("+CON"));
    BOOST_CHECK_EQUAL(GroupProductionExceedLimit::WELL     , GroupProductionExceedLimit::ActionEnumFromString("WELL"));
    BOOST_CHECK_EQUAL(GroupProductionExceedLimit::PLUG     , GroupProductionExceedLimit::ActionEnumFromString("PLUG"));
    BOOST_CHECK_EQUAL(GroupProductionExceedLimit::RATE     , GroupProductionExceedLimit::ActionEnumFromString("RATE"));
}



BOOST_AUTO_TEST_CASE(TestGroupProductionExceedLimitActionEnumLoop) {
    BOOST_CHECK_EQUAL( GroupProductionExceedLimit::NONE     , GroupProductionExceedLimit::ActionEnumFromString( GroupProductionExceedLimit::ActionEnumToString( GroupProductionExceedLimit::NONE     ) ));
    BOOST_CHECK_EQUAL( GroupProductionExceedLimit::CON      , GroupProductionExceedLimit::ActionEnumFromString( GroupProductionExceedLimit::ActionEnumToString( GroupProductionExceedLimit::CON      ) ));
    BOOST_CHECK_EQUAL( GroupProductionExceedLimit::CON_PLUS , GroupProductionExceedLimit::ActionEnumFromString( GroupProductionExceedLimit::ActionEnumToString( GroupProductionExceedLimit::CON_PLUS ) ));
    BOOST_CHECK_EQUAL( GroupProductionExceedLimit::WELL     , GroupProductionExceedLimit::ActionEnumFromString( GroupProductionExceedLimit::ActionEnumToString( GroupProductionExceedLimit::WELL     ) ));
    BOOST_CHECK_EQUAL( GroupProductionExceedLimit::PLUG     , GroupProductionExceedLimit::ActionEnumFromString( GroupProductionExceedLimit::ActionEnumToString( GroupProductionExceedLimit::PLUG     ) ));
    BOOST_CHECK_EQUAL( GroupProductionExceedLimit::RATE     , GroupProductionExceedLimit::ActionEnumFromString( GroupProductionExceedLimit::ActionEnumToString( GroupProductionExceedLimit::RATE     ) ));

    BOOST_CHECK_EQUAL("NONE" , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::ActionEnumFromString( "NONE" ) ));
    BOOST_CHECK_EQUAL("CON"  , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::ActionEnumFromString( "CON"  ) ));
    BOOST_CHECK_EQUAL("+CON" , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::ActionEnumFromString( "+CON" ) ));
    BOOST_CHECK_EQUAL("WELL" , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::ActionEnumFromString( "WELL" ) ));
    BOOST_CHECK_EQUAL("PLUG" , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::ActionEnumFromString( "PLUG" ) ));
    BOOST_CHECK_EQUAL("RATE" , GroupProductionExceedLimit::ActionEnumToString(GroupProductionExceedLimit::ActionEnumFromString( "RATE" ) ));
}


/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestPhaseEnumToString) {
    BOOST_CHECK_EQUAL( "OIL"  ,  Phase::PhaseEnumToString(Phase::OIL));
    BOOST_CHECK_EQUAL( "GAS"  ,  Phase::PhaseEnumToString(Phase::GAS));
    BOOST_CHECK_EQUAL( "WATER" , Phase::PhaseEnumToString(Phase::WATER));
}


BOOST_AUTO_TEST_CASE(TestPhaseEnumFromString) {
    BOOST_CHECK_THROW( Phase::PhaseEnumFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( Phase::OIL   , Phase::PhaseEnumFromString("OIL"));
    BOOST_CHECK_EQUAL( Phase::WATER , Phase::PhaseEnumFromString("WATER"));
    BOOST_CHECK_EQUAL( Phase::GAS   , Phase::PhaseEnumFromString("GAS"));
}



BOOST_AUTO_TEST_CASE(TestPhaseEnumLoop) {
    BOOST_CHECK_EQUAL( Phase::OIL   , Phase::PhaseEnumFromString( Phase::PhaseEnumToString( Phase::OIL ) ));
    BOOST_CHECK_EQUAL( Phase::WATER , Phase::PhaseEnumFromString( Phase::PhaseEnumToString( Phase::WATER ) ));
    BOOST_CHECK_EQUAL( Phase::GAS   , Phase::PhaseEnumFromString( Phase::PhaseEnumToString( Phase::GAS ) ));

    BOOST_CHECK_EQUAL( "OIL"    , Phase::PhaseEnumToString(Phase::PhaseEnumFromString(  "OIL" ) ));
    BOOST_CHECK_EQUAL( "GAS"    , Phase::PhaseEnumToString(Phase::PhaseEnumFromString(  "GAS" ) ));
    BOOST_CHECK_EQUAL( "WATER"  , Phase::PhaseEnumToString(Phase::PhaseEnumFromString(  "WATER" ) ));
}



BOOST_AUTO_TEST_CASE(TestPhaseEnumMask) {
    BOOST_CHECK_EQUAL( 0 , Phase::OIL   & Phase::GAS );
    BOOST_CHECK_EQUAL( 0 , Phase::OIL   & Phase::WATER );
    BOOST_CHECK_EQUAL( 0 , Phase::WATER & Phase::GAS );
}



/*****************************************************************/

BOOST_AUTO_TEST_CASE(TestInjectorEnumToString) {
    BOOST_CHECK_EQUAL( "OIL"  ,  WellInjector::TypeToString(WellInjector::OIL));
    BOOST_CHECK_EQUAL( "GAS"  ,  WellInjector::TypeToString(WellInjector::GAS));
    BOOST_CHECK_EQUAL( "WATER" , WellInjector::TypeToString(WellInjector::WATER));
    BOOST_CHECK_EQUAL( "MULTI" , WellInjector::TypeToString(WellInjector::MULTI));
}


BOOST_AUTO_TEST_CASE(TestInjectorEnumFromString) {
    BOOST_CHECK_THROW( WellInjector::TypeFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( WellInjector::OIL   , WellInjector::TypeFromString("OIL"));
    BOOST_CHECK_EQUAL( WellInjector::WATER , WellInjector::TypeFromString("WATER"));
    BOOST_CHECK_EQUAL( WellInjector::GAS   , WellInjector::TypeFromString("GAS"));
    BOOST_CHECK_EQUAL( WellInjector::MULTI , WellInjector::TypeFromString("MULTI"));
}



BOOST_AUTO_TEST_CASE(TestInjectorEnumLoop) {
    BOOST_CHECK_EQUAL( WellInjector::OIL     , WellInjector::TypeFromString( WellInjector::TypeToString( WellInjector::OIL ) ));
    BOOST_CHECK_EQUAL( WellInjector::WATER   , WellInjector::TypeFromString( WellInjector::TypeToString( WellInjector::WATER ) ));
    BOOST_CHECK_EQUAL( WellInjector::GAS     , WellInjector::TypeFromString( WellInjector::TypeToString( WellInjector::GAS ) ));
    BOOST_CHECK_EQUAL( WellInjector::MULTI   , WellInjector::TypeFromString( WellInjector::TypeToString( WellInjector::MULTI ) ));

    BOOST_CHECK_EQUAL( "MULTI"    , WellInjector::TypeToString(WellInjector::TypeFromString(  "MULTI" ) ));
    BOOST_CHECK_EQUAL( "OIL"      , WellInjector::TypeToString(WellInjector::TypeFromString(  "OIL" ) ));
    BOOST_CHECK_EQUAL( "GAS"      , WellInjector::TypeToString(WellInjector::TypeFromString(  "GAS" ) ));
    BOOST_CHECK_EQUAL( "WATER"    , WellInjector::TypeToString(WellInjector::TypeFromString(  "WATER" ) ));
}

/*****************************************************************/

BOOST_AUTO_TEST_CASE(InjectorCOntrolMopdeEnumToString) {
    BOOST_CHECK_EQUAL( "RATE"  ,  WellInjector::ControlModeToString(WellInjector::RATE));
    BOOST_CHECK_EQUAL( "RESV"  ,  WellInjector::ControlModeToString(WellInjector::RESV));
    BOOST_CHECK_EQUAL( "BHP" , WellInjector::ControlModeToString(WellInjector::BHP));
    BOOST_CHECK_EQUAL( "THP" , WellInjector::ControlModeToString(WellInjector::THP));
    BOOST_CHECK_EQUAL( "GRUP" , WellInjector::ControlModeToString(WellInjector::GRUP));
}


BOOST_AUTO_TEST_CASE(InjectorControlModeEnumFromString) {
    BOOST_CHECK_THROW( WellInjector::ControlModeFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( WellInjector::RATE   , WellInjector::ControlModeFromString("RATE"));
    BOOST_CHECK_EQUAL( WellInjector::BHP , WellInjector::ControlModeFromString("BHP"));
    BOOST_CHECK_EQUAL( WellInjector::RESV   , WellInjector::ControlModeFromString("RESV"));
    BOOST_CHECK_EQUAL( WellInjector::THP , WellInjector::ControlModeFromString("THP"));
    BOOST_CHECK_EQUAL( WellInjector::GRUP , WellInjector::ControlModeFromString("GRUP"));
}



BOOST_AUTO_TEST_CASE(InjectorControlModeEnumLoop) {
    BOOST_CHECK_EQUAL( WellInjector::RATE     , WellInjector::ControlModeFromString( WellInjector::ControlModeToString( WellInjector::RATE ) ));
    BOOST_CHECK_EQUAL( WellInjector::BHP   , WellInjector::ControlModeFromString( WellInjector::ControlModeToString( WellInjector::BHP ) ));
    BOOST_CHECK_EQUAL( WellInjector::RESV     , WellInjector::ControlModeFromString( WellInjector::ControlModeToString( WellInjector::RESV ) ));
    BOOST_CHECK_EQUAL( WellInjector::THP   , WellInjector::ControlModeFromString( WellInjector::ControlModeToString( WellInjector::THP ) ));
    BOOST_CHECK_EQUAL( WellInjector::GRUP   , WellInjector::ControlModeFromString( WellInjector::ControlModeToString( WellInjector::GRUP ) ));

    BOOST_CHECK_EQUAL( "THP"    , WellInjector::ControlModeToString(WellInjector::ControlModeFromString(  "THP" ) ));
    BOOST_CHECK_EQUAL( "RATE"      , WellInjector::ControlModeToString(WellInjector::ControlModeFromString(  "RATE" ) ));
    BOOST_CHECK_EQUAL( "RESV"      , WellInjector::ControlModeToString(WellInjector::ControlModeFromString(  "RESV" ) ));
    BOOST_CHECK_EQUAL( "BHP"    , WellInjector::ControlModeToString(WellInjector::ControlModeFromString(  "BHP" ) ));
    BOOST_CHECK_EQUAL( "GRUP"    , WellInjector::ControlModeToString(WellInjector::ControlModeFromString(  "GRUP" ) ));
}



/*****************************************************************/

BOOST_AUTO_TEST_CASE(InjectorStatusEnumToString) {
    BOOST_CHECK_EQUAL( "OPEN"  ,  WellCommon::StatusToString(WellCommon::OPEN));
    BOOST_CHECK_EQUAL( "SHUT"  ,  WellCommon::StatusToString(WellCommon::SHUT));
    BOOST_CHECK_EQUAL( "AUTO"   ,  WellCommon::StatusToString(WellCommon::AUTO));
    BOOST_CHECK_EQUAL( "STOP"   ,  WellCommon::StatusToString(WellCommon::STOP));
}


BOOST_AUTO_TEST_CASE(InjectorStatusEnumFromString) {
    BOOST_CHECK_THROW( WellCommon::StatusFromString("XXX") , std::invalid_argument );
    BOOST_CHECK_EQUAL( WellCommon::OPEN   , WellCommon::StatusFromString("OPEN"));
    BOOST_CHECK_EQUAL( WellCommon::AUTO , WellCommon::StatusFromString("AUTO"));
    BOOST_CHECK_EQUAL( WellCommon::SHUT   , WellCommon::StatusFromString("SHUT"));
    BOOST_CHECK_EQUAL( WellCommon::STOP , WellCommon::StatusFromString("STOP"));
}



BOOST_AUTO_TEST_CASE(InjectorStatusEnumLoop) {
    BOOST_CHECK_EQUAL( WellCommon::OPEN     , WellCommon::StatusFromString( WellCommon::StatusToString( WellCommon::OPEN ) ));
    BOOST_CHECK_EQUAL( WellCommon::AUTO   , WellCommon::StatusFromString( WellCommon::StatusToString( WellCommon::AUTO ) ));
    BOOST_CHECK_EQUAL( WellCommon::SHUT     , WellCommon::StatusFromString( WellCommon::StatusToString( WellCommon::SHUT ) ));
    BOOST_CHECK_EQUAL( WellCommon::STOP   , WellCommon::StatusFromString( WellCommon::StatusToString( WellCommon::STOP ) ));

    BOOST_CHECK_EQUAL( "STOP"    , WellCommon::StatusToString(WellCommon::StatusFromString(  "STOP" ) ));
    BOOST_CHECK_EQUAL( "OPEN"      , WellCommon::StatusToString(WellCommon::StatusFromString(  "OPEN" ) ));
    BOOST_CHECK_EQUAL( "SHUT"      , WellCommon::StatusToString(WellCommon::StatusFromString(  "SHUT" ) ));
    BOOST_CHECK_EQUAL( "AUTO"    , WellCommon::StatusToString(WellCommon::StatusFromString(  "AUTO" ) ));
}



/*****************************************************************/

BOOST_AUTO_TEST_CASE(ProducerCOntrolMopdeEnumToString) {
    BOOST_CHECK_EQUAL( "ORAT"  ,  WellProducer::ControlModeToString(WellProducer::ORAT));
    BOOST_CHECK_EQUAL( "WRAT"  ,  WellProducer::ControlModeToString(WellProducer::WRAT));
    BOOST_CHECK_EQUAL( "GRAT"  , WellProducer::ControlModeToString(WellProducer::GRAT));
    BOOST_CHECK_EQUAL( "LRAT"  , WellProducer::ControlModeToString(WellProducer::LRAT));
    BOOST_CHECK_EQUAL( "CRAT"  , WellProducer::ControlModeToString(WellProducer::CRAT));
    BOOST_CHECK_EQUAL( "RESV"  ,  WellProducer::ControlModeToString(WellProducer::RESV));
    BOOST_CHECK_EQUAL( "BHP"   , WellProducer::ControlModeToString(WellProducer::BHP));
    BOOST_CHECK_EQUAL( "THP"   , WellProducer::ControlModeToString(WellProducer::THP));
    BOOST_CHECK_EQUAL( "GRUP"  , WellProducer::ControlModeToString(WellProducer::GRUP));
}


BOOST_AUTO_TEST_CASE(ProducerControlModeEnumFromString) {
    BOOST_CHECK_THROW( WellProducer::ControlModeFromString("XRAT") , std::invalid_argument );
    BOOST_CHECK_EQUAL( WellProducer::ORAT   , WellProducer::ControlModeFromString("ORAT"));
    BOOST_CHECK_EQUAL( WellProducer::WRAT   , WellProducer::ControlModeFromString("WRAT"));
    BOOST_CHECK_EQUAL( WellProducer::GRAT   , WellProducer::ControlModeFromString("GRAT"));
    BOOST_CHECK_EQUAL( WellProducer::LRAT   , WellProducer::ControlModeFromString("LRAT"));
    BOOST_CHECK_EQUAL( WellProducer::CRAT   , WellProducer::ControlModeFromString("CRAT"));
    BOOST_CHECK_EQUAL( WellProducer::RESV   , WellProducer::ControlModeFromString("RESV"));
    BOOST_CHECK_EQUAL( WellProducer::BHP    , WellProducer::ControlModeFromString("BHP" ));
    BOOST_CHECK_EQUAL( WellProducer::THP    , WellProducer::ControlModeFromString("THP" ));
    BOOST_CHECK_EQUAL( WellProducer::GRUP   , WellProducer::ControlModeFromString("GRUP"));
}



BOOST_AUTO_TEST_CASE(ProducerControlModeEnumLoop) {
    BOOST_CHECK_EQUAL( WellProducer::ORAT     , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::ORAT ) ));
    BOOST_CHECK_EQUAL( WellProducer::WRAT     , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::WRAT ) ));
    BOOST_CHECK_EQUAL( WellProducer::GRAT     , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::GRAT ) ));
    BOOST_CHECK_EQUAL( WellProducer::LRAT     , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::LRAT ) ));
    BOOST_CHECK_EQUAL( WellProducer::CRAT     , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::CRAT ) ));
    BOOST_CHECK_EQUAL( WellProducer::RESV     , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::RESV ) ));
    BOOST_CHECK_EQUAL( WellProducer::BHP      , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::BHP  ) ));
    BOOST_CHECK_EQUAL( WellProducer::THP      , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::THP  ) ));
    BOOST_CHECK_EQUAL( WellProducer::GRUP     , WellProducer::ControlModeFromString( WellProducer::ControlModeToString( WellProducer::GRUP ) ));

    BOOST_CHECK_EQUAL( "ORAT"      , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "ORAT"  ) ));
    BOOST_CHECK_EQUAL( "WRAT"      , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "WRAT"  ) ));
    BOOST_CHECK_EQUAL( "GRAT"      , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "GRAT"  ) ));
    BOOST_CHECK_EQUAL( "LRAT"      , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "LRAT"  ) ));
    BOOST_CHECK_EQUAL( "CRAT"      , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "CRAT"  ) ));
    BOOST_CHECK_EQUAL( "RESV"      , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "RESV"  ) ));
    BOOST_CHECK_EQUAL( "BHP"       , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "BHP"   ) ));
    BOOST_CHECK_EQUAL( "THP"       , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "THP"   ) ));
    BOOST_CHECK_EQUAL( "GRUP"      , WellProducer::ControlModeToString(WellProducer::ControlModeFromString( "GRUP"  ) ));
}

/*******************************************************************/
/*****************************************************************/

BOOST_AUTO_TEST_CASE(GuideRatePhaseEnumToString) {
    BOOST_CHECK_EQUAL( "OIL"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::OIL));
    BOOST_CHECK_EQUAL( "WAT"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::WAT));
    BOOST_CHECK_EQUAL( "GAS"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::GAS));
    BOOST_CHECK_EQUAL( "LIQ"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::LIQ));
    BOOST_CHECK_EQUAL( "COMB"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::COMB));
    BOOST_CHECK_EQUAL( "WGA"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::WGA));
    BOOST_CHECK_EQUAL( "CVAL"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::CVAL));
    BOOST_CHECK_EQUAL( "RAT"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::RAT));
    BOOST_CHECK_EQUAL( "RES"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::RES));
    BOOST_CHECK_EQUAL( "UNDEFINED"  ,  GuideRate::GuideRatePhaseEnumToString(GuideRate::UNDEFINED));
}


BOOST_AUTO_TEST_CASE(GuideRatePhaseEnumFromString) {
    BOOST_CHECK_THROW( GuideRate::GuideRatePhaseEnumFromString("XRAT") , std::invalid_argument );
    BOOST_CHECK_EQUAL( GuideRate::OIL   , GuideRate::GuideRatePhaseEnumFromString("OIL"));
    BOOST_CHECK_EQUAL( GuideRate::WAT   , GuideRate::GuideRatePhaseEnumFromString("WAT"));
    BOOST_CHECK_EQUAL( GuideRate::GAS   , GuideRate::GuideRatePhaseEnumFromString("GAS"));
    BOOST_CHECK_EQUAL( GuideRate::LIQ   , GuideRate::GuideRatePhaseEnumFromString("LIQ"));
    BOOST_CHECK_EQUAL( GuideRate::COMB   , GuideRate::GuideRatePhaseEnumFromString("COMB"));
    BOOST_CHECK_EQUAL( GuideRate::WGA   , GuideRate::GuideRatePhaseEnumFromString("WGA"));
    BOOST_CHECK_EQUAL( GuideRate::CVAL   , GuideRate::GuideRatePhaseEnumFromString("CVAL"));
    BOOST_CHECK_EQUAL( GuideRate::RAT   , GuideRate::GuideRatePhaseEnumFromString("RAT"));
    BOOST_CHECK_EQUAL( GuideRate::RES   , GuideRate::GuideRatePhaseEnumFromString("RES"));
    BOOST_CHECK_EQUAL( GuideRate::UNDEFINED, GuideRate::GuideRatePhaseEnumFromString("UNDEFINED"));
}



BOOST_AUTO_TEST_CASE(GuideRatePhaseEnum2Loop) {
    BOOST_CHECK_EQUAL( GuideRate::OIL     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::OIL ) ));
    BOOST_CHECK_EQUAL( GuideRate::WAT     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::WAT ) ));
    BOOST_CHECK_EQUAL( GuideRate::GAS     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::GAS ) ));
    BOOST_CHECK_EQUAL( GuideRate::LIQ     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::LIQ ) ));
    BOOST_CHECK_EQUAL( GuideRate::COMB     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::COMB ) ));
    BOOST_CHECK_EQUAL( GuideRate::WGA     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::WGA ) ));
    BOOST_CHECK_EQUAL( GuideRate::CVAL     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::CVAL ) ));
    BOOST_CHECK_EQUAL( GuideRate::RAT     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::RAT ) ));
    BOOST_CHECK_EQUAL( GuideRate::RES     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::RES ) ));
    BOOST_CHECK_EQUAL( GuideRate::UNDEFINED     , GuideRate::GuideRatePhaseEnumFromString( GuideRate::GuideRatePhaseEnumToString( GuideRate::UNDEFINED ) ));

    BOOST_CHECK_EQUAL( "OIL"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "OIL"  ) ));
    BOOST_CHECK_EQUAL( "WAT"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "WAT"  ) ));
    BOOST_CHECK_EQUAL( "GAS"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "GAS"  ) ));
    BOOST_CHECK_EQUAL( "LIQ"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "LIQ"  ) ));
    BOOST_CHECK_EQUAL( "COMB"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "COMB"  ) ));
    BOOST_CHECK_EQUAL( "WGA"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "WGA"  ) ));
    BOOST_CHECK_EQUAL( "CVAL"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "CVAL"  ) ));
    BOOST_CHECK_EQUAL( "RAT"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "RAT"  ) ));
    BOOST_CHECK_EQUAL( "RES"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "RES"  ) ));
    BOOST_CHECK_EQUAL( "UNDEFINED"      , GuideRate::GuideRatePhaseEnumToString(GuideRate::GuideRatePhaseEnumFromString( "UNDEFINED"  ) ));

}
