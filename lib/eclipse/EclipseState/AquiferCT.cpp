/*
  Copyright (C) 2017 TNO

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

#include <opm/parser/eclipse/EclipseState/AquiferCT.hpp>

namespace Opm {

    AquiferCT::AquiferCT(const EclipseState& eclState, const Deck& deck)
    {
        m_aquifers = init_aquct_aqutab(eclState, deck);
        m_aquancon = init_aquancon(eclState, deck);
    }

    std::vector<AquiferCT::AQUCT_data> AquiferCT::getAquifers()
    {
        return m_aquifers;
    }

    std::vector<AquiferCT::AQUANCON_data> AquiferCT::getAquancon()
    {
        return m_aquancon;
    }

    const int AquiferCT::getAqInflTabID(size_t aquiferIndex)
    {
        return m_aquifers.at(aquiferIndex).inftableID;
    }

    const int AquiferCT::getAqPvtTabID(size_t aquiferIndex)
    {
        return m_aquifers.at(aquiferIndex).pvttableID;
    }

    const double AquiferCT::getAqInfluxCoeff(size_t aquanconRecord)
    {
        return m_aquancon.at(aquanconRecord).influx_coeff;
    }

    const double AquiferCT::getAqInfluxMult(size_t aquanconRecord)
    {
        return m_aquancon.at(aquanconRecord).influx_mult;
    }

}