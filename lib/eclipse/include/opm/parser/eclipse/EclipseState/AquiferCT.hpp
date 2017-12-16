/*
  Copyright (C) 2015 Statoil ASA

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

#ifndef OPM_AQUIFERCT_HPP
#define OPM_AQUIFERCT_HPP

/*
  The AquiferCT which stands for AquiferCarterTracy is a data container object meant to hold the data for the aquifer carter tracy model.
  This includes the logic for parsing as well as the associated tables. It is meant to be used by opm-grid and opm-simulators in order to
  implement the Carter Tracy analytical aquifer model in OPM Flow.
*/

#include <opm/parser/eclipse/EclipseState/EclipseState.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/A.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckItem.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

#include <opm/parser/eclipse/EclipseState/Tables/Aqudims.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/TableContainer.hpp>
#include <opm/parser/eclipse/EclipseState/Tables/AqutabTable.hpp>
#include <boost/concept_check.hpp>

namespace Opm {

    class AquiferCT {
        public:

            struct AQUCT_data{

                    // Aquifer ID
                    int aquiferID;
                    // Table IDs
                    int inftableID, pvttableID;
                    // Perforation cell id
                    std::vector<int> cell_id;
                    // Variables constants
                    double  phi_aq , //aquifer porosity
                            d0,   //aquifer datum depth
                            C_t , //total compressibility
                            r_o , //aquifer inner radius
                            k_a , //aquifer permeability
                            c1, // 0.008527 (METRIC, PVT-M); 0.006328 (FIELD); 3.6 (LAB)
                            h , //aquifer thickness
                            theta , //angle subtended by the aquifer boundary
                            c2 ; //6.283 (METRIC, PVT-M); 1.1191 (FIELD); 6.283 (LAB).
                    std::vector<double> td, pi;
            };

            AquiferCT(const EclipseState& eclState, const Deck& deck);

            std::vector<AquiferCT::AQUCT_data> getAquifers();

            const double getAqPorosity(size_t aquiferIndex);
      
    private:
        
        Deck m_deck;
        
        

        std::vector<AquiferCT::AQUCT_data> m_aquifers;

        // Initialize function
        inline std::vector<AquiferCT::AQUCT_data>
        init_object(const EclipseState& eclState, const Deck& deck)
        {
            if (!deck.hasKeyword("AQUCT")){
                std::cout << "Nothing is shown! Where is AQUCT!????" << std::endl;
            }

            const auto& aquctKeyword = deck.getKeyword("AQUCT");

            std::vector<AquiferCT::AQUCT_data> aquctParams;
            // Resize the parameter vector container based on row entries in aquct
            // We do the same for aquifers too because number of aquifers is assumed to be for each entry in aquct
            aquctParams.resize(aquctKeyword.size());
            const int tableID = 0;

            std::cout << "Parsing AQUCT stuff" << std::endl;
            for (size_t aquctRecordIdx = 0; aquctRecordIdx < aquctKeyword.size(); ++ aquctRecordIdx) 
            {
                const auto& aquctRecord = aquctKeyword.getRecord(aquctRecordIdx);

                aquctParams.at(aquctRecordIdx).aquiferID = aquctRecord.getItem("AQUIFER_ID").template get<int>(0);
                aquctParams.at(aquctRecordIdx).h = aquctRecord.getItem("THICKNESS_AQ").getSIDouble(0);
                aquctParams.at(aquctRecordIdx).phi_aq = aquctRecord.getItem("PORO_AQ").getSIDouble(0);
                aquctParams.at(aquctRecordIdx).d0 = aquctRecord.getItem("DAT_DEPTH").getSIDouble(0);
                aquctParams.at(aquctRecordIdx).C_t = aquctRecord.getItem("C_T").getSIDouble(0);
                aquctParams.at(aquctRecordIdx).r_o = aquctRecord.getItem("RAD").getSIDouble(0);
                aquctParams.at(aquctRecordIdx).k_a = aquctRecord.getItem("PERM_AQ").getSIDouble(0);
                aquctParams.at(aquctRecordIdx).theta = aquctRecord.getItem("INFLUENCE_ANGLE").getSIDouble(0);
                aquctParams.at(aquctRecordIdx).c1 = 0.008527; // We are using SI
                aquctParams.at(aquctRecordIdx).c2 = 6.283;
                aquctParams.at(aquctRecordIdx).inftableID = aquctRecord.getItem("TABLE_NUM_INFLUENCE_FN").template get<int>(0) - 1;
                aquctParams.at(aquctRecordIdx).pvttableID = aquctRecord.getItem("TABLE_NUM_WATER_PRESS").template get<int>(0) - 1;

                std::cout << aquctParams.at(aquctRecordIdx).inftableID << std::endl;
                // Get the correct influence table values
                const auto& aqutabTable = eclState.getTableManager().getAqutabTables().getTable(aquctParams.at(aquctRecordIdx).inftableID);
                const auto& aqutab_tdColumn = aqutabTable.getColumn(0);
                const auto& aqutab_piColumn = aqutabTable.getColumn(1);
                aquctParams.at(aquctRecordIdx).td = aqutab_tdColumn.vectorCopy();
                aquctParams.at(aquctRecordIdx).pi = aqutab_piColumn.vectorCopy();

                // We determine the cell perforation here. (We must provide OPM-grid and simulator 
                // with the relevant information of the global cell id). This is a hack for now to test functionality
                int cellID = 10 + aquctRecordIdx;

                aquctParams.at(aquctRecordIdx).cell_id.push_back(cellID);

                std::cout << "Aquifer CT #" << aquctParams.at(aquctRecordIdx).aquiferID << std::endl;
                auto ita = aquctParams.at(aquctRecordIdx).td.cbegin();
                auto f_lambda = [&ita] (double i) {std::cout << *ita++ << "    " << i << std::endl;};
                std::for_each( aquctParams.at(aquctRecordIdx).pi.cbegin(), 
                               aquctParams.at(aquctRecordIdx).pi.cend(), f_lambda );
            }

            return aquctParams;
        }

        inline void set_default_tables(std::vector<double>& td, std::vector<double>& pi)
        {
            std::vector<double> default_pressure_ = { 0.112, 0.229, 0.315, 0.376, 0.424, 0.469, 0.503, 0.564, 0.616, 0.659, 0.702, 0.735, 
                                                      0.772, 0.802, 0.927, 1.02, 1.101, 1.169, 1.275, 1.362, 1.436, 1.5, 1.556, 1.604, 
                                                      1.651, 1.829, 1.96, 2.067, 2.147, 2.282, 2.388, 2.476, 2.55, 2.615, 2.672, 2.723, 
                                                      2.921, 3.064, 3.173, 3.263, 3.406, 3.516, 3.608, 3.684, 3.75, 3.809, 3.86           };

            std::vector<double> default_time_ = { 0.01, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 
                                                    1.5, 2, 2.5, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 30, 40, 50, 60, 70, 
                                                    80, 90, 100, 150, 200, 250, 300, 400, 500, 600, 700, 800, 900, 1000    };

            td = default_time_;
            pi = default_pressure_;
        }

    };
}


#endif
