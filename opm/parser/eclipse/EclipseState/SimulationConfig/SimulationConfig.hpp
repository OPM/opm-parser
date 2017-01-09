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

#ifndef OPM_SIMULATION_CONFIG_HPP
#define OPM_SIMULATION_CONFIG_HPP

#include <bitset>

#include <opm/parser/eclipse/EclipseState/SimulationConfig/ThresholdPressure.hpp>

namespace Opm {

    class Deck;
    class Eclipse3DProperties;

    class RelpermOptions {
        public:
            RelpermOptions() = default;
            RelpermOptions( const Deck& );

            bool directional() const noexcept;
            bool nondirectional() const noexcept;
            bool reversible() const noexcept;
            bool irreversible() const noexcept;
            bool hysteresis() const noexcept;
            bool capillaryPressureCanVaryWithSurfaceTension() const noexcept;

            enum Model {
                DEFAULT,
                STONE1,
                STONE2,
                // IKU3P and ODD3P are currently unsupported
                // IKU3P,
                // ODD3P,
            };

            Model model() const noexcept;

        private:
            enum class option {
                direct,
                irrevers,
                hysteresis,
                surface_tension,
            };

            using ue = std::underlying_type< option >::type;
            std::bitset< 4 > options;
            Model rp_model = Model::DEFAULT;
    };

    class SimulationConfig {

    public:

        SimulationConfig(const Deck& deck,
                         const Eclipse3DProperties& gridProperties);

        const ThresholdPressure& getThresholdPressure() const;
        bool hasThresholdPressure() const;
        bool useCPR() const;
        bool hasDISGAS() const;
        bool hasVAPOIL() const;
        const RelpermOptions& relperm() const noexcept;

    private:
        ThresholdPressure m_ThresholdPressure;
        bool m_useCPR;
        bool m_DISGAS;
        bool m_VAPOIL;
        RelpermOptions m_relperm_opts;
    };

} //namespace Opm



#endif
