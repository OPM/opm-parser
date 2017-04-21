#ifndef OPM_HYSTERESIS_OPTIONS_HPP
#define OPM_HYSTERESIS_OPTIONS_HPP

/*
  Copyright 2016 Statoil ASA.

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

#include <iosfwd>
#include <opm/parser/eclipse/EclipseState/Runspec.hpp>

namespace Opm {

class Deck;

class HysteresisOptions {
    public:
        HysteresisOptions() = default;
        HysteresisOptions( const Deck& );

        enum class CapillaryPressureScanningCurveShape { RETR, NEW };
        enum class Model {
            Equilibration,
            CarlsonHysteresisNonWettingDrainageWetting,
            CarlsonHysteresisNonWettingImbibitionNonWetting,
            KilloughHysteresisNonWettingDrainageWetting,
            KilloughHysteresisNonWettingImbibitionWetting,
            KilloughHysteresisNonWettingKilloughHysteresisWetting,
            CarlsonNonWettingGasWaterDrainageOil,
            KilloughNonWettingGasWaterDrainageOil,
            KilloughHysteresisNonWettingGasWaterWettingOil,
            JargonHysteresisNonWettingDrainageWetting,
            JargonHysteresisNonWettingImbibitionWetting,
        };

        operator bool() const noexcept;

        double curvatureCapillaryPressure() const noexcept;
        Model model() const noexcept;
        double curvatureForKilloughWettingPhase() const noexcept;
        double modificationForTrappedNonWettingKillough() const noexcept;
        bool applyToCapillaryPressure() const noexcept;
        bool applyToRelativePermeability() const noexcept;
        bool applyToBothCapillaryPressureAndRelativePermeability() const noexcept;
        CapillaryPressureScanningCurveShape curveShapesOnSecondaryReversal() const noexcept;
        bool initialMobilityCorrectionAppliesToDrainageCurve() const noexcept;
        bool initialMobilityCorrectionAppliesToImbibitionCurve() const noexcept;
        bool initialMobilityCorrectionAppliesToBothCurves() const noexcept;
        Phase wettingPhase() const noexcept;
        bool oilPhaseUseBaker() const noexcept;
        bool gasPhaseUseBaker() const noexcept;
        bool waterPhaseUseBaker() const noexcept;
        double thresholdSaturationKillough() const noexcept;
        int wettingPhaseRelpermModification() const noexcept;

    private:
        bool enabled = false;
        double curvature = 0.1;
        Model hys_model = Model::CarlsonHysteresisNonWettingDrainageWetting;
        double curvature_killough = 1.0;
        double modification_killough = 0.1;
        bool apply_capillary_pressure = true;
        bool apply_relative_permeability = true;
        CapillaryPressureScanningCurveShape curve_shape = CapillaryPressureScanningCurveShape::RETR;
        bool apply_imbibition = false;
        Phase wetting_phase = Phase::OIL;
        bool baker_oil = false;
        bool baker_gas = false;
        bool baker_water = false;
        double threshold_saturation = 0.0;
        int modify_wetting_relperm = 0;
};

std::ostream& operator<<( std::ostream&, const HysteresisOptions::Model& );
std::ostream& operator<<( std::ostream&, const HysteresisOptions::CapillaryPressureScanningCurveShape& );

}

#endif //OPM_HYSTERESIS_OPTIONS_HPP
