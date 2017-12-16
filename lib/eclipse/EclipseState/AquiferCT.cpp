#include <opm/parser/eclipse/EclipseState/AquiferCT.hpp>

namespace Opm {

    AquiferCT::AquiferCT(const EclipseState& eclState, const Deck& deck)
    {
        m_deck = deck;
        m_aquifers = init_object(eclState, deck);
    }

}