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

#include <opm/parser/eclipse/EclipseState/SimulationConfig/ThresholdPressure.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/Log/Logger.hpp>


namespace Opm {

    ThresholdPressure::ThresholdPressure(DeckConstPtr deck,int maxEqlnum) {

        if (Section::hasRUNSPEC(deck) && Section::hasSOLUTION(deck)) {
            std::shared_ptr<const RUNSPECSection> runspecSection = std::make_shared<const RUNSPECSection>(deck);
            std::shared_ptr<const SOLUTIONSection> solutionSection = std::make_shared<const SOLUTIONSection>(deck);
            initThresholdPressure(runspecSection, solutionSection, maxEqlnum);
        }
    }


    void ThresholdPressure::initThresholdPressure(std::shared_ptr<const RUNSPECSection> runspecSection,
                                                  std::shared_ptr<const SOLUTIONSection> solutionSection,
                                                  int maxEqlnum) {

        bool thpresOption = false;

        if (runspecSection->hasKeyword("EQLOPTS")) {
            auto eqlopts = runspecSection->getKeyword("EQLOPTS");
            auto rec = eqlopts->getRecord(0);
            for (size_t i = 0; i < rec->size(); ++i) {
                auto item = rec->getItem(i);
                if (item->hasValue(0)) {
                    if (item->getString(0) == "THPRES") {
                        thpresOption = true;
                    } else if (item->getString(0) == "IRREVERS") {
                        throw std::runtime_error("Cannot use IRREVERS version of THPRES option, not implemented");
                    }
                }
            }
        }


        // Do we have the THPRES keyword?
        // Check for consistency.
        const bool thpresKeyword = solutionSection->hasKeyword("THPRES");
        if (thpresKeyword != thpresOption) {
            throw std::runtime_error("Invalid solution section, the THPRES keyword must be present and the THPRES "
                                      "option of EQLOPTS must be used for the threshold pressure feature.");
        }


        // Create threshold pressure table.
        auto thpres = solutionSection->getKeyword("THPRES");

        mThresholdPressureTable.resize(maxEqlnum * maxEqlnum, 0.0);

        const int numRecords = thpres->size();
        for (int rec_ix = 0; rec_ix < numRecords; ++rec_ix) {
            auto rec = thpres->getRecord(rec_ix);
            auto region1Item = rec->getItem("REGION1");
            auto region2Item = rec->getItem("REGION2");
            auto thpressItem = rec->getItem("THPRES");

            if (region1Item->hasValue(0) && region2Item->hasValue(0) && thpressItem->hasValue(0)) {
                const int r1 = region1Item->getInt(0) - 1;
                const int r2 = region2Item->getInt(0) - 1;
                const double p = thpressItem->getSIDouble(0);

                if (r1 >= maxEqlnum || r2 >= maxEqlnum) {
                    throw std::runtime_error("Too high region numbers in THPRES keyword");
                }
                mThresholdPressureTable[r1 + maxEqlnum*r2] = p;
                mThresholdPressureTable[r2 + maxEqlnum*r1] = p;
            } else {
                throw std::runtime_error("Missing data for use of the THPRES keyword");
            }
        }
    }


    const std::vector<double>& ThresholdPressure::getThresholdPressureTable() const {
        return mThresholdPressureTable;
    }

} //namespace Opm





