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

#include <set>

#include <boost/algorithm/string/join.hpp>

#include <opm/parser/eclipse/EclipseState/Eclipse3DProperties.hpp>
#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/Box.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/BoxManager.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/FaultCollection.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/Fault.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/GridDims.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/MULTREGTScanner.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/NNC.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/SatfuncPropertyInitializers.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/TransMult.hpp>
#include <opm/parser/eclipse/EclipseState/InitConfig/InitConfig.hpp>
#include <opm/parser/eclipse/EclipseState/IOConfig/IOConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/SimulationConfig.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableManager.hpp>
#include <opm/parser/eclipse/Parser/Deck.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/M.hpp>
#include <opm/parser/eclipse/Units/Dimension.hpp>
#include <opm/parser/eclipse/Units/UnitSystem.hpp>
#include <opm/parser/eclipse/Parser/MessageContainer.hpp>


namespace Opm {

    EclipseState::EclipseState(const Deck& deck, ParseContext parseContext) :
        m_parseContext(      parseContext ),
        m_tables(            deck ),
        m_runspec(           deck ),
        m_gridDims(          deck ),
        m_inputGrid(         deck, nullptr ),
        m_schedule(          m_parseContext, m_inputGrid, deck, m_runspec.phases() ),
        m_eclipseProperties( deck, m_tables, m_inputGrid ),
        m_eclipseConfig(     deck, m_eclipseProperties, m_gridDims, m_schedule, parseContext ),
        m_transMult(         m_inputGrid.getNX(), m_inputGrid.getNY(), m_inputGrid.getNZ(),
                             m_eclipseProperties, deck.getKeywordList( "MULTREGT" ) ),
        m_inputNnc(          deck, m_gridDims ),
        m_deckUnitSystem(    deck.getActiveUnitSystem() )
    {
        m_inputGrid.resetACTNUM(m_eclipseProperties.getIntGridProperty("ACTNUM").getData().data());

        if( this->runspec().phases().size() < 3 )
            m_messageContainer.info("Only " + std::to_string( this->runspec().phases().size() )
                                    + " fluid phases are enabled" );

        if (deck.hasKeyword( "TITLE" )) {
            const auto& titleKeyword = deck.getKeyword( "TITLE" );
            const auto& item = titleKeyword.getRecord( 0 ).getItem( 0 );
            std::vector<std::string> itemValue = item.getData<std::string>();
            m_title = boost::algorithm::join( itemValue, " " );
        }

        initTransMult();
        initFaults(deck);

        m_messageContainer.appendMessages(m_tables.getMessageContainer());
        m_messageContainer.appendMessages(m_schedule.getMessageContainer());
        m_messageContainer.appendMessages(m_inputGrid.getMessageContainer());
        m_messageContainer.appendMessages(m_eclipseProperties.getMessageContainer());
    }

    const UnitSystem& EclipseState::getDeckUnitSystem() const {
        return m_deckUnitSystem;
    }

    const UnitSystem& EclipseState::getUnits() const {
        return m_deckUnitSystem;
    }

    const EclipseGrid& EclipseState::getInputGrid() const {
        return m_inputGrid;
    }

    const SummaryConfig& EclipseState::getSummaryConfig() const {
        return m_eclipseConfig.getSummaryConfig();
    }

    const RestartConfig& EclipseState::getRestartConfig() const {
        return m_eclipseConfig.getRestartConfig();
    }

    RestartConfig& EclipseState::getRestartConfig() {
        return const_cast< RestartConfig& >( m_eclipseConfig.getRestartConfig() );
    }

    const Eclipse3DProperties& EclipseState::get3DProperties() const {
        return m_eclipseProperties;
    }

    const MessageContainer& EclipseState::getMessageContainer() const {
        return m_messageContainer;
    }


    MessageContainer& EclipseState::getMessageContainer() {
        return m_messageContainer;
    }


    const TableManager& EclipseState::getTableManager() const {
        return m_tables;
    }

    const ParseContext& EclipseState::getParseContext() const {
        return m_parseContext;
    }

    const Schedule& EclipseState::getSchedule() const {
        return this->m_schedule;
    }

    /// [[deprecated]] --- use cfg().io()
    const IOConfig& EclipseState::getIOConfig() const {
        return m_eclipseConfig.io();
    }

    /// [[deprecated]] --- use cfg().io()
    IOConfig& EclipseState::getIOConfig() {
        return m_eclipseConfig.io();
    }

    /// [[deprecated]] --- use cfg().init()
    const InitConfig& EclipseState::getInitConfig() const {
        return m_eclipseConfig.getInitConfig();
    }

    /// [[deprecated]] --- use cfg()
    const EclipseConfig& EclipseState::getEclipseConfig() const {
        return cfg();
    }

    const EclipseConfig& EclipseState::cfg() const {
        return m_eclipseConfig;
    }

    const Runspec& EclipseState::runspec() const {
        return this->m_runspec;
    }

    /// [[deprecated]] --- use cfg().simulation()
    const SimulationConfig& EclipseState::getSimulationConfig() const {
        return m_eclipseConfig.getSimulationConfig();
    }

    const FaultCollection& EclipseState::getFaults() const {
        return m_faults;
    }

    const TransMult& EclipseState::getTransMult() const {
        return m_transMult;
    }

    const NNC& EclipseState::getInputNNC() const {
        return m_inputNnc;
    }

    bool EclipseState::hasInputNNC() const {
        return m_inputNnc.hasNNC();
    }

    std::string EclipseState::getTitle() const {
        return m_title;
    }

    void EclipseState::initTransMult() {
        const auto& p = m_eclipseProperties;
        if (m_eclipseProperties.hasDeckDoubleGridProperty("MULTX"))
            m_transMult.applyMULT(p.getDoubleGridProperty("MULTX"), FaceDir::XPlus);
        if (m_eclipseProperties.hasDeckDoubleGridProperty("MULTX-"))
            m_transMult.applyMULT(p.getDoubleGridProperty("MULTX-"), FaceDir::XMinus);

        if (m_eclipseProperties.hasDeckDoubleGridProperty("MULTY"))
            m_transMult.applyMULT(p.getDoubleGridProperty("MULTY"), FaceDir::YPlus);
        if (m_eclipseProperties.hasDeckDoubleGridProperty("MULTY-"))
            m_transMult.applyMULT(p.getDoubleGridProperty("MULTY-"), FaceDir::YMinus);

        if (m_eclipseProperties.hasDeckDoubleGridProperty("MULTZ"))
            m_transMult.applyMULT(p.getDoubleGridProperty("MULTZ"), FaceDir::ZPlus);
        if (m_eclipseProperties.hasDeckDoubleGridProperty("MULTZ-"))
            m_transMult.applyMULT(p.getDoubleGridProperty("MULTZ-"), FaceDir::ZMinus);
    }

    void EclipseState::initFaults(const Deck& deck) {
        const GRIDSection gridSection ( deck );

        m_faults = FaultCollection(gridSection, m_inputGrid);
        setMULTFLT(gridSection);

        if (Section::hasEDIT(deck)) {
            setMULTFLT(EDITSection ( deck ));
        }

        m_transMult.applyMULTFLT( m_faults );
    }



    void EclipseState::setMULTFLT(const Section& section) {
        for (size_t index=0; index < section.count("MULTFLT"); index++) {
            const auto& faultsKeyword = section.getKeyword("MULTFLT" , index);
            for (auto iter = faultsKeyword.begin(); iter != faultsKeyword.end(); ++iter) {

                const auto& faultRecord = *iter;
                const std::string& faultName = faultRecord.getItem(0).get< std::string >(0);
                double multFlt = faultRecord.getItem(1).get< double >(0);

                m_faults.setTransMult( faultName , multFlt );
            }
        }
    }

    void EclipseState::complainAboutAmbiguousKeyword(const Deck& deck, const std::string& keywordName) {
        m_messageContainer.error("The " + keywordName + " keyword must be unique in the deck. Ignoring all!");
        auto keywords = deck.getKeywordList(keywordName);
        for (size_t i = 0; i < keywords.size(); ++i) {
            std::string msg = "Ambiguous keyword "+keywordName+" defined here";
            m_messageContainer.error(keywords[i]->getFileName(), msg, keywords[i]->getLineNumber());
        }
    }

    void EclipseState::applyModifierDeck(const Deck& deck) {
        using namespace ParserKeywords;
        for (const auto& keyword : deck) {

            if (keyword.isKeyword<MULTFLT>()) {
                for (const auto& record : keyword) {
                    const std::string& faultName = record.getItem<MULTFLT::fault>().get< std::string >(0);
                    auto& fault = m_faults.getFault( faultName );
                    double tmpMultFlt = record.getItem<MULTFLT::factor>().get< double >(0);
                    double oldMultFlt = fault.getTransMult( );
                    double newMultFlt = oldMultFlt * tmpMultFlt;

                    /*
                      This extremely contrived way of doing it is because of difference in
                      behavior and section awareness between the Fault object and the
                      Transmult object:

                      1. MULTFLT keywords found in the SCHEDULE section should apply the
                         transmissibility modifiers cumulatively - i.e. the current
                         transmissibility across the fault should be *multiplied* with the
                         newly entered MULTFLT value, and the resulting transmissibility
                         multplier for this fault should be the product of the newly
                         entered value and the current value.

                      2. The TransMult::applyMULTFLT() implementation will *multiply* the
                         transmissibility across a face with the value in the fault
                         object. Hence the current value has already been multiplied in;
                         we therefor first *set* the MULTFLT value to the new value, then
                         apply it to the TransMult object and then eventually update the
                         MULTFLT value in the fault instance.

                    */
                    fault.setTransMult( tmpMultFlt );
                    m_transMult.applyMULTFLT( fault );
                    fault.setTransMult( newMultFlt );
                }
            }
        }
    }
}
