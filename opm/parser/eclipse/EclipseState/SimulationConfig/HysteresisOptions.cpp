#include <opm/parser/eclipse/EclipseState/SimulationConfig/HysteresisOptions.hpp>
#include <opm/parser/eclipse/Utility/String.hpp>

namespace Opm {

namespace {

HysteresisOptions::Model readHysteresisModel( int x ) {
    using Model = HysteresisOptions::Model;
    switch( x ) {
        case -1: return Model::Equilibration;
        case  0: return Model::CarlsonHysteresisNonWettingDrainageWetting;
        case  1: return Model::CarlsonHysteresisNonWettingImbibitionNonWetting;
        case  2: return Model::KilloughHysteresisNonWettingDrainageWetting;
        case  3: return Model::KilloughHysteresisNonWettingImbibitionWetting;
        case  4: return Model::KilloughHysteresisNonWettingKilloughHysteresisWetting;
        case  5: return Model::CarlsonNonWettingGasWaterDrainageOil;
        case  6: return Model::KilloughNonWettingGasWaterDrainageOil;
        case  7: return Model::KilloughHysteresisNonWettingGasWaterWettingOil;
        case  8: return Model::JargonHysteresisNonWettingDrainageWetting;
        case  9: return Model::JargonHysteresisNonWettingImbibitionWetting;
        default:
            throw std::invalid_argument( "Hysteresis model must be an integer [-1, 9]" );
    }         
}

int readHysteresisApply( const DeckItem& item ) {
    if( item.defaultApplied( 0 ) ) return 0;

    const auto& key = uppercase( item.getTrimmedString( 0 ) );

    if( key == "BOTH" ) return 0;
    if( key == "PC" ) return 1;
    if( key == "KR" ) return 2;

    throw std::invalid_argument( "Limiting hysteresis flag must be 'PC', 'KR' or 'BOTH'" );
}

HysteresisOptions::CapillaryPressureScanningCurveShape readHysteresisCurveShape( const DeckItem& item ) {
    if( item.defaultApplied( 0 ) )
        return HysteresisOptions::CapillaryPressureScanningCurveShape::RETR;

    const auto& key = uppercase( item.getTrimmedString( 0 ) );
    if( key == "RETR" )
        return HysteresisOptions::CapillaryPressureScanningCurveShape::RETR;

    if( key == "NEW" )
        return HysteresisOptions::CapillaryPressureScanningCurveShape::RETR;

    throw std::invalid_argument( "Flag controlling the shape of capillary pressure curves on retraversal must be 'RETR' or 'NEW'" );
}

bool readHysteresisCorrectImbibition( const DeckItem& item ) {
    if( item.defaultApplied( 0 ) ) return false;

    const auto& key = uppercase( item.getTrimmedString( 0 ) );
    if( key == "DRAIN" ) return false;
    if( key == "BOTH" ) return true;

    throw std::invalid_argument( "Flag controlling fluid mobility correction application on imbibition curves must be 'DRAIN' or 'BOTH'" );
}

Phase readHysteresisWettingPhase( const DeckItem& item ) {
    if( item.defaultApplied( 0 ) ) return Phase::OIL;

    const auto& key = uppercase( item.getTrimmedString( 0 ) );
    if( key == "OIL" ) return Phase::OIL;
    if( key == "GAS" ) return Phase::GAS;

    throw std::invalid_argument( "Hysteresis Wetting phase must be 'OIL' or 'GAS'" );
}

bool yesno( const DeckItem& item ) {
    if( item.defaultApplied( 0 ) ) return false;

    const auto& key = item.get< std::string >( 0 );
    if( key.front() == 'Y' || key.front() == 'y' ) return true;
    if( key.front() == 'N' || key.front() == 'n' ) return false;

    throw std::invalid_argument( "Hysteresis boolean flag must be 'YES' or 'NO'" );
}

}

HysteresisOptions::HysteresisOptions( const Deck& deck ) {
    if( !deck.hasKeyword( "EHYSTR" ) ) return;

    const auto& record = deck.getKeyword( "EHYSTR" ).getRecord( 0 );

    this->enabled = true;
    this->curvature = record.getItem( 0 ).get< double >( 0 );
    this->hys_model = readHysteresisModel( record.getItem( 1 ).get< int >( 0 ) );
    this->curvature_killough = record.getItem( 2 ).get< double >( 0 );
    this->modification_killough = record.getItem( 3 ).get< double >( 0 );

    const int apply_to = readHysteresisApply( record.getItem( 4 ) );
    this->apply_capillary_pressure = apply_to == 0 || apply_to == 1;
    this->apply_relative_permeability = apply_to == 0 || apply_to == 2;

    this->curve_shape = readHysteresisCurveShape( record.getItem( 5 ) );
    this->apply_imbibition = readHysteresisCorrectImbibition( record.getItem( 6 ) );
    this->wetting_phase = readHysteresisWettingPhase( record.getItem( 7 ) );
    this->baker_oil = yesno( record.getItem( 8 ) );
    this->baker_gas = yesno( record.getItem( 9 ) );
    this->baker_water = yesno( record.getItem( 10 ) );
    this->threshold_saturation = record.getItem( 11 ).get< double >( 0 );
    this->modify_wetting_relperm = record.getItem( 12 ).get< int >( 0 );

    if( this->curvature < 0 )
        throw std::invalid_argument(
                "EHYSTR curvature parameter for capillary pressure curves "
                "must be larger than zero" );
}

HysteresisOptions::operator bool() const noexcept {
    return this->enabled;
}

double HysteresisOptions::curvatureCapillaryPressure() const noexcept {
    return this->curvature;
}

HysteresisOptions::Model HysteresisOptions::model() const noexcept {
    return this->hys_model;
}

double HysteresisOptions::curvatureForKilloughWettingPhase() const noexcept {
    return this->curvature_killough;
}

double HysteresisOptions::modificationForTrappedNonWettingKillough() const noexcept {
    return this->modification_killough;
}

bool HysteresisOptions::applyToCapillaryPressure() const noexcept {
    return this->apply_capillary_pressure;
}

bool HysteresisOptions::applyToRelativePermeability() const noexcept {
    return this->apply_relative_permeability;
}

bool HysteresisOptions::applyToBothCapillaryPressureAndRelativePermeability() const noexcept {
    return this->applyToCapillaryPressure() && this->applyToRelativePermeability();
}

HysteresisOptions::CapillaryPressureScanningCurveShape HysteresisOptions::curveShapesOnSecondaryReversal() const noexcept {
    return this->curve_shape;
}

bool HysteresisOptions::initialMobilityCorrectionAppliesToDrainageCurve() const noexcept {
    return true;
}

bool HysteresisOptions::initialMobilityCorrectionAppliesToImbibitionCurve() const noexcept {
    return this->apply_imbibition;
}

bool HysteresisOptions::initialMobilityCorrectionAppliesToBothCurves() const noexcept {
    return this->initialMobilityCorrectionAppliesToDrainageCurve()
        && this->initialMobilityCorrectionAppliesToImbibitionCurve();
}

Phase HysteresisOptions::wettingPhase() const noexcept {
    return this->wetting_phase;
}

bool HysteresisOptions::oilPhaseUseBaker() const noexcept {
    return this->baker_oil;
}

bool HysteresisOptions::gasPhaseUseBaker() const noexcept {
    return this->baker_gas;
}

bool HysteresisOptions::waterPhaseUseBaker() const noexcept {
    return this->baker_gas;
}

double HysteresisOptions::thresholdSaturationKillough() const noexcept {
    return this->threshold_saturation;
}

int HysteresisOptions::wettingPhaseRelpermModification() const noexcept {
    return this->modify_wetting_relperm;
}

std::ostream& operator<<( std::ostream& stream, const HysteresisOptions::Model& model ) {
    using Model = HysteresisOptions::Model;
    switch( model ) {
        case Model::Equilibration:
            return stream << "Equilibration";
        case Model::CarlsonHysteresisNonWettingDrainageWetting:
            return stream << "Carlson's hysteresis for non-wetting phase(s), drainage curve for wetting phase";
        case Model::CarlsonHysteresisNonWettingImbibitionNonWetting:
            return stream << "Carlson's hysteresis for non-wetting phase(s), imbibition curve for wetting phase";
        case Model::KilloughHysteresisNonWettingDrainageWetting:
            return stream << "Killough's hysteresis for non-wetting phase(s), drainage curve for wetting phase";
        case Model::KilloughHysteresisNonWettingImbibitionWetting:
            return stream << "Killough's hysteresis for non-wetting phase(s), imbibition curve for wetting phase";
        case Model::KilloughHysteresisNonWettingKilloughHysteresisWetting:
            return stream << "Killough's hysteresis for both wetting and non-wetting phases.";
        case Model::CarlsonNonWettingGasWaterDrainageOil:
            return stream << "Carlson's non-wetting model for gas and water phases, drainage curve for oil phase";
        case Model::KilloughNonWettingGasWaterDrainageOil:
            return stream << "Killough's non-wetting model for gas and water phases, drainage curve for oil phase";
        case Model::KilloughHysteresisNonWettingGasWaterWettingOil:
            return stream << "Killough's hysteresis model for non-wetting gas and water phases and the wetting oil phase";
        case Model::JargonHysteresisNonWettingDrainageWetting:
            return stream << "Jargon's hystersis model for non-wetting phase(s), drainge curve for wetting phase.";
        case Model::JargonHysteresisNonWettingImbibitionWetting:
            return stream << "Jargon's hystersis model for non-wetting phase(s), imbibition curve for wetting phase.";

        default:
            throw std::logic_error( "Illegal state." );
    }
}

std::ostream& operator<<( std::ostream& stream, const HysteresisOptions::CapillaryPressureScanningCurveShape& shape ) {
    using Shape = HysteresisOptions::CapillaryPressureScanningCurveShape;
    switch( shape ) {
        case Shape::RETR: return stream << "re-traverse";
        case Shape::NEW: return stream << "new";

        default:
            throw std::logic_error( "Illegal state." );
    }
}

}
