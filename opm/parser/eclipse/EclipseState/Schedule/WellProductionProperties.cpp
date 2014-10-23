#include <opm/parser/eclipse/EclipseState/Schedule/WellProductionProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>

#include <string>
#include <vector>

namespace Opm {
    WellProductionProperties::WellProductionProperties(ParserLogPtr parserLog)
        : m_parserLog(parserLog)
    {
        init();

        predictionMode = true;
    }

    WellProductionProperties::WellProductionProperties(DeckRecordConstPtr record) {
        init();

        waterRate = record->getItem("WRAT")->getSIDouble(0);
        oilRate = record->getItem("ORAT")->getSIDouble(0);
        gasRate = record->getItem("GRAT")->getSIDouble(0);
    }

    WellProductionProperties WellProductionProperties::history(DeckRecordConstPtr record) {
        WellProductionProperties p(record);

        p.predictionMode = false;

        // Modes supported in WCONHIST just from {O,W,G}RAT values
        //
        // Note: The default value of observed {O,W,G}RAT is zero
        // (numerically) whence the following control modes are
        // unconditionally supported.
        const std::vector<std::string> controlModes{
            "ORAT", "WRAT", "GRAT", "LRAT", "RESV"
        };

        auto modeIt = controlModes.begin();
        auto endIt = controlModes.end();
        for (; modeIt != endIt; ++modeIt) {
            const WellProducer::ControlModeEnum cmode =
                WellProducer::ControlModeFromString(*modeIt);

            p.addProductionControl(cmode);
        }

        // BHP control must be explictly provided.
        if (!record->getItem("BHP")->defaultApplied(0))
            p.addProductionControl(WellProducer::BHP);

        const auto cmodeItem = record->getItem("CMODE");
        if (!cmodeItem->defaultApplied(0)) {
            const WellProducer::ControlModeEnum cmode = WellProducer::ControlModeFromString( cmodeItem->getString(0) );

            if (p.hasProductionControl( cmode ))
                p.controlMode = cmode;
            else
                throw std::invalid_argument("Setting CMODE to unspecified control");
        }

        return p;
    }

    WellProductionProperties WellProductionProperties::prediction(DeckRecordConstPtr record) {
        WellProductionProperties p(record);

        p.predictionMode = true;

        p.liquidRate = record->getItem("LRAT")->getSIDouble(0);
        p.resvRate   = record->getItem("RESV")->getSIDouble(0);
        p.BHPLimit   = record->getItem("BHP" )->getSIDouble(0);
        p.THPLimit   = record->getItem("THP" )->getSIDouble(0);

        const std::vector<std::string> controlModes{
            "ORAT", "WRAT", "GRAT", "LRAT",
            "RESV", "BHP" , "THP"
        };

        auto modeIt = controlModes.begin();
        auto endIt = controlModes.end();
        for (; modeIt != endIt; ++modeIt) {
            if (!record->getItem(*modeIt)->defaultApplied(0)) {
                const WellProducer::ControlModeEnum cmode =
                    WellProducer::ControlModeFromString(*modeIt);

                p.addProductionControl(cmode);
            }
        }

        const auto cmodeItem = record->getItem("CMODE");
        if (!cmodeItem->defaultApplied(0)) {
            const WellProducer::ControlModeEnum cmode = WellProducer::ControlModeFromString( cmodeItem->getString(0) );

            if (p.hasProductionControl( cmode ))
                p.controlMode = cmode;
            else
                throw std::invalid_argument("Setting CMODE to unspecified control");
        }

        return p;
    }

    void WellProductionProperties::init() {
        oilRate = 0.0;
        gasRate = 0.0;
        waterRate = 0.0;
        liquidRate = 0.0;
        resvRate = 0.0;
        BHPLimit = 0.0;
        THPLimit = 0.0;
        controlMode = WellProducer::CMODE_UNDEFINED;

        m_productionControls = 0;
    }
} // namespace Opm
