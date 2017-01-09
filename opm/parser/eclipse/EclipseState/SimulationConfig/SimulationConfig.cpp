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


#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/EclipseState/Eclipse3DProperties.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/SimulationConfig.hpp>
#include <opm/parser/eclipse/EclipseState/SimulationConfig/ThresholdPressure.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/C.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/D.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/V.hpp>
#include <opm/parser/eclipse/Utility/String.hpp>



/*
  The internalization of the CPR keyword has been temporarily
  disabled, suddenly decks with 'CPR' in the summary section turned
  up. Keywords with section aware keyword semantics is currently not
  handled by the parser.

  When the CPR is added again the following keyword configuration must
  be added:

    {"name" : "CPR" , "sections" : ["RUNSPEC"], "size": 1 }

*/


namespace Opm {

namespace {
RelpermOptions::Model read_relperm_model( const Deck& deck ) {
    /*
     * STONE is an alias for STONE2.
     *
     * Quite iffy since if both STONE and STONE2 are requested, STONE2 will
     * take preference - opm has always behaved that way and will continue to
     * do so for now, but we can/should consider changing behaviour to warn,
     * throw or prefer the one specified last.
     */
    if( deck.hasKeyword( "STONE" ) || deck.hasKeyword( "STONE2" ) )
        return RelpermOptions::Model::STONE2;

    if( deck.hasKeyword( "STONE1" ) )
        return RelpermOptions::Model::STONE1;

    return RelpermOptions::Model::DEFAULT;
}

}

RelpermOptions::RelpermOptions( const Deck& deck ) :
    rp_model( read_relperm_model( deck ) ) {
    if( !deck.hasKeyword( "SATOPTS" ) ) return;

    const auto index = []( option o ) { return static_cast< ue >( o ); };

    for( const auto& k : deck.getKeyword( "SATOPTS" ).getStringData() ) {
        const auto& key = uppercase( k );
        if(      key == "DIRECT" )   this->options.set( index( option::direct ) );
        else if( key == "IRREVERS" ) this->options.set( index( option::irrevers ) );
        else if( key == "HYSTER" )   this->options.set( index( option::hysteresis ) );
        else if( key == "SURFTENS" ) this->options.set( index( option::surface_tension ) );
    }
}

bool RelpermOptions::directional() const noexcept {
    return this->options[ static_cast< ue >( option::direct ) ];
}

bool RelpermOptions::nondirectional() const noexcept {
    return !this->directional();
}

bool RelpermOptions::irreversible() const noexcept {
    return this->options[ static_cast< ue >( option::irrevers ) ];
}

bool RelpermOptions::reversible() const noexcept {
    return !this->irreversible();
}

bool RelpermOptions::hysteresis() const noexcept {
    return this->options[ static_cast< ue >( option::hysteresis ) ];
}

bool RelpermOptions::capillaryPressureCanVaryWithSurfaceTension() const noexcept {
    return this->options[ static_cast< ue >( option::surface_tension ) ];
}

RelpermOptions::Model RelpermOptions::model() const noexcept {
    return this->rp_model;
}

    SimulationConfig::SimulationConfig(const Deck& deck,
                                       const Eclipse3DProperties& eclipseProperties) :
        m_ThresholdPressure( deck, eclipseProperties ),
        m_useCPR(false),
        m_DISGAS(false),
        m_VAPOIL(false),
        m_relperm_opts( deck )
    {
        if (Section::hasRUNSPEC(deck)) {
            const RUNSPECSection runspec(deck);
            if (runspec.hasKeyword<ParserKeywords::CPR>()) {
                const auto& cpr = runspec.getKeyword<ParserKeywords::CPR>();
                if (cpr.size() > 0)
                    throw std::invalid_argument("ERROR: In the RUNSPEC section the CPR keyword should EXACTLY one empty record.");

                m_useCPR = true;
            }
            if (runspec.hasKeyword<ParserKeywords::DISGAS>()) {
                m_DISGAS = true;
            }
            if (runspec.hasKeyword<ParserKeywords::VAPOIL>()) {
                m_VAPOIL = true;
            }
        }
    }

    const ThresholdPressure& SimulationConfig::getThresholdPressure() const {
        return m_ThresholdPressure;
    }

    bool SimulationConfig::hasThresholdPressure() const {
        return m_ThresholdPressure.size() > 0;
    }

    bool SimulationConfig::useCPR() const {
        return m_useCPR;
    }

    bool SimulationConfig::hasDISGAS() const {
        return m_DISGAS;
    }

    bool SimulationConfig::hasVAPOIL() const {
        return m_VAPOIL;
    }

    const RelpermOptions& SimulationConfig::relperm() const noexcept {
        return this->m_relperm_opts;
    }

} //namespace Opm
