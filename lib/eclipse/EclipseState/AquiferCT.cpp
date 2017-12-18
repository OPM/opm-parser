#include <opm/parser/eclipse/EclipseState/AquiferCT.hpp>

namespace Opm {

    AquiferCT::AquiferCT(const EclipseState& eclState, const Deck& deck)
    {
        m_aquifers = init_object(eclState, deck);
    }

    std::vector<AquiferCT::AQUCT_data> AquiferCT::getAquifers()
    {
        return m_aquifers;
    }

    const double AquiferCT::getAqPorosity(size_t aquiferIndex)
    {
        return m_aquifers.at(aquiferIndex).phi_aq;
    }

}