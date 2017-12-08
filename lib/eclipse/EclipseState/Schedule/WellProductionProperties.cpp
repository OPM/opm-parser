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

#include <iostream>
#include <string>
#include <vector>

#include <opm/parser/eclipse/Units/Units.hpp>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/WellProductionProperties.hpp>


namespace Opm {

    WellProductionProperties::
    WellProductionProperties() : predictionMode( true )
    {}

    WellProductionProperties::
    WellProductionProperties( const DeckRecord& record ) :
        OilRate( record.getItem( "ORAT" ).getSIDouble( 0 ) ),
        WaterRate( record.getItem( "WRAT" ).getSIDouble( 0 ) ),
        GasRate( record.getItem( "GRAT" ).getSIDouble( 0 ) )
    {}


    WellProductionProperties WellProductionProperties::history(const WellProductionProperties& prevProperties, const DeckRecord& record,
                                                               const WellProducer::ControlModeEnum controlModeWHISTCL,
                                                               const WellCommon::StatusEnum status)
    {
        WellProductionProperties p(record);

        // for well shut or stopped, the observaion rates will be set to zero
        if (status == WellCommon::SHUT || status == WellCommon::STOP) {
            p.OilRate = 0.;
            p.WaterRate = 0.;
            p.GasRate = 0.;
        }

        p.predictionMode = false;

        /*
          If the control mode specfied by WHISTCL is effective, this control mode will override
          the control mode specified in the keyword WCONHIST.

          There are a few ways to specify the BHPLimit in historical mode.
          If the well is under BHP control mode, then the observed BHP value will
          serve as the BHP limit.

          If the control mode is not BHP, then by default the BHP limit is 1 atm.

          You can reset the BHP limit to a specific value with keyword WELTARG after WCONHIST
          for both of the above situations.

          When the BHP limit is reset by WELTARG, the BHP limit will not be changed by the subsequent
          WCONHIST keyword if not under BHP control.
        */

        if (status != WellCommon::SHUT) {
            namespace wp = WellProducer;
            // control mode will be used
            wp::ControlModeEnum cmode = wp::CMODE_UNDEFINED;

            // when there is an effective control mode specified by WHISTCL, we always use this control mode
            if ( effectiveHistoryProductionControl(controlModeWHISTCL) ) {
                cmode = controlModeWHISTCL;
            } else {
                const auto& cmodeItem = record.getItem("CMODE");
                if ( !cmodeItem.defaultApplied(0) ) {
                    const std::string cmode_string = cmodeItem.getTrimmedString( 0 );
                    cmode = wp::ControlModeFromString(cmode_string);
                }
            }

            // through either WHISTCL or WCONHIST, we get a control mode
            if ( effectiveHistoryProductionControl(cmode) ) {
                p.addProductionControl( cmode );
                p.controlMode = cmode;
            } else {
                const std::string cmode_string = wp::ControlMode2String(cmode);
                const std::string well_name = record.getItem("WELL").getTrimmedString(0);
                const std::string msg = "unsupported control mode " + cmode_string + " for WCONHIST of Well " + well_name;
                throw std::invalid_argument(msg);
            }

            // always have a BHP control/limit, while the limit value needs to be determined
            // the control mode added above can be a BHP control or a type of RATE control
            if ( !p.hasProductionControl( wp::BHP ) )
                p.addProductionControl( wp::BHP );

            // Note, all the BHP limits set here can be reset by WELTARG after WCONHIST
            if (cmode == wp::BHP) {
                if ( record.getItem("BHP").hasValue(0) )
                    p.BHPLimit = record.getItem( "BHP" ).getSIDouble( 0 );

                p.BHPLimitFromWelltag = false;
            } else {
                if (!prevProperties.predictionMode && prevProperties.BHPLimitFromWelltag) {
                    // if the previous property is under history matching mode, and it gets the
                    // BHP limit from the WELTARG, we will use the same BHP limit if not under
                    // BHP control
                    p.BHPLimit = prevProperties.BHPLimit;
                    p.BHPLimitFromWelltag = true;
                } else {
                    // by defualt, the BHP limit is 1 atm
                    p.BHPLimit = 1.* unit::atm;
                    p.BHPLimitFromWelltag = false;
                }
            }
        }

        if ( record.getItem( "BHP" ).hasValue(0) )
            p.BHPH = record.getItem("BHP").getSIDouble(0);
        if ( record.getItem( "THP" ).hasValue(0) )
            p.THPH = record.getItem("THP").getSIDouble(0);

        return p;
    }



    WellProductionProperties WellProductionProperties::prediction( const DeckRecord& record, bool addGroupProductionControl)
    {
        WellProductionProperties p(record);
        p.predictionMode = true;

        p.LiquidRate     = record.getItem("LRAT"     ).getSIDouble(0);
        p.ResVRate       = record.getItem("RESV"     ).getSIDouble(0);
        p.BHPLimit       = record.getItem("BHP"      ).getSIDouble(0);
        p.THPLimit       = record.getItem("THP"      ).getSIDouble(0);
        p.ALQValue       = record.getItem("ALQ"      ).get< double >(0); //NOTE: Unit of ALQ is never touched
        p.VFPTableNumber = record.getItem("VFP_TABLE").get< int >(0);

        namespace wp = WellProducer;
        using mode = std::pair< const char*, wp::ControlModeEnum >;
        static const mode modes[] = {
            { "ORAT", wp::ORAT }, { "WRAT", wp::WRAT }, { "GRAT", wp::GRAT },
            { "LRAT", wp::LRAT }, { "RESV", wp::RESV }, { "THP", wp::THP }
        };

        for( const auto& cmode : modes ) {
            if( !record.getItem( cmode.first ).defaultApplied( 0 ) )
                 p.addProductionControl( cmode.second );
        }

        // There is always a BHP constraint, when not specified, will use the default value
        p.addProductionControl( wp::BHP );

        if (addGroupProductionControl) {
            p.addProductionControl(WellProducer::GRUP);
        }


        {
            const auto& cmodeItem = record.getItem("CMODE");
            if (cmodeItem.hasValue(0)) {
                const WellProducer::ControlModeEnum cmode = WellProducer::ControlModeFromString( cmodeItem.getTrimmedString(0) );

                if (p.hasProductionControl( cmode ))
                    p.controlMode = cmode;
                else
                    throw std::invalid_argument("Setting CMODE to unspecified control");
            }
        }
        return p;
    }


    bool WellProductionProperties::operator==(const WellProductionProperties& other) const {
        return OilRate              == other.OilRate
            && WaterRate            == other.WaterRate
            && GasRate              == other.GasRate
            && LiquidRate           == other.LiquidRate
            && ResVRate             == other.ResVRate
            && BHPLimit             == other.BHPLimit
            && THPLimit             == other.THPLimit
            && BHPH                 == other.BHPH
            && THPH                 == other.THPH
            && VFPTableNumber       == other.VFPTableNumber
            && controlMode          == other.controlMode
            && m_productionControls == other.m_productionControls
            && this->predictionMode == other.predictionMode;
    }


    bool WellProductionProperties::operator!=(const WellProductionProperties& other) const {
        return !(*this == other);
    }


    std::ostream& operator<<( std::ostream& stream, const WellProductionProperties& wp ) {
        return stream
            << "WellProductionProperties { "
            << "oil rate: "     << wp.OilRate           << ", "
            << "water rate: "   << wp.WaterRate         << ", "
            << "gas rate: "     << wp.GasRate           << ", "
            << "liquid rate: "  << wp.LiquidRate        << ", "
            << "ResV rate: "    << wp.ResVRate          << ", "
            << "BHP limit: "    << wp.BHPLimit          << ", "
            << "THP limit: "    << wp.THPLimit          << ", "
            << "BHPH: "         << wp.BHPH              << ", "
            << "THPH: "         << wp.THPH              << ", "
            << "VFP table: "    << wp.VFPTableNumber    << ", "
            << "ALQ: "          << wp.ALQValue          << ", "
            << "prediction: "   << wp.predictionMode    << " }";
    }

} // namespace Opm
