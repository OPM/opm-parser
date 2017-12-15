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
  The Aqudims class is a small utility class designed to hold on to
  the values from the AQUCT and AQUTAB keywords.
*/

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
    
    // class Deck;

    class AquiferCT {
        public:

            AquiferCT() : m_aquId(0),
                          m_datDepth(0.0),
                          m_aqPerm(0.0),
                          m_aqPoro(0.0),
                          m_aqCt(0.0),
                          m_aqRad(0.0),
                          m_aqThickness(0.0),
                          m_aqInfluenceAngle(0.0),
                          m_waterPressTableNum(0 ),
                          m_influenceFnTableNum( 0 )
            {
                Deck init_deck = Deck();
                deck_ = &init_deck;
            }

            explicit AquiferCT(const Deck& deck) 
            //: Aquifer() // I don't see why we should delegate the constructor? This is because deck_ is given deck!
            {
                deck_ = deck;
                if (deck.hasKeyword("AQUCT")) {
                    const auto& record = deck.getKeyword( "AQUCT" , 0 ).getRecord( 0 );
                    m_aquId  = record.getItem("AQUIFER_ID").get<int>(0);
                    m_datDepth  = record.getItem("DAT_DEPTH").get<double>(0);
                    m_aqPerm  = record.getItem("PERM_AQ").get<double>(0);
                    m_aqPoro  = record.getItem("PORO_AQ").get<double>(0);
                    m_aqCt  = record.getItem("C_T").get<double>(0);
                    m_aqRad  = record.getItem("RAD").get<double>(0);
                    m_aqThickness  = record.getItem("THICKNESS_AQ").get<double>(0);
                    m_aqInfluenceAngle  = record.getItem("INFLUENCE_ANGLE").get<double>(0);
                    m_waterPressTableNum  = record.getItem("TABLE_NUM_WATER_PRESS").get<int>(0);
                    m_influenceFnTableNum  = record.getItem("TABLE_NUM_INFLUENCE_FN").get<int>(0);
                }

                int number_tables = m_aqudims.NIFTBL;
            }

            size_t getAquId() const
            {
                return m_aquId;
            }

            size_t getAqDatumDepth() const
            {
                return m_datDepth;
            }

            size_t getAqPermeability() const
            {
                return m_aqPerm;
            }

            size_t getAqPorosity() const
            {
                return m_aqPoro;
            }

            size_t getAqTotalCompressibility() const
            {
                return m_aqCt;
            }

            size_t getAqRadius() const
            {
                return m_aqRad;
            }
            
            size_t getAqThickness() const
            {
                return m_aqThickness;
            }

            size_t getAqInfluenceAngle() const
            {
                return m_aqInfluenceAngle;
            }

            size_t getAqWaterPressureTableNumber() const
            {
                return m_waterPressTableNum;
            }        

            size_t getAqInfluenceFunctionTableNumber() const
            {
                return m_influenceFnTableNum;
            }        
      
            std::vector<double> getTimeColumn() const
            {
                if (deck_.hasKeyword("AQUTAB")) 
                {
                    if (m_influenceFnTableNum == 1)
                    {
                        return default_time_;
                    }
                    else 
                    {
                        return Aqutab_tables_.at(m_influenceFnTableNum-1).getTimeColumn();
                    }
                }
            }
	  
            std::vector<double> getPressureColumn() const
            {
                if (m_influenceFnTableNum == 1) 
                {
                    return default_pressure_;
                }
                else 
                {
                    return Aqutab_tables_.at(m_influenceFnTableNum-1).getPressureColumn();
                }
            }

    private:
        size_t m_aquId , m_datDepth , m_aqPerm , m_aqPoro , m_aqCt , m_aqRad , m_aqThickness , m_aqInfluenceAngle, m_waterPressTableNum, m_influenceFnTableNum;
        
        Deck& deck_;
        
        std::vector<AqutabTable> Aqutab_tables_;
        
        std::vector<double> default_pressure_ = {0.112, 0.229, 0.315, 0.376, 0.424, 0.469, 0.503, 0.564, 0.616, 0.659, 0.702, 0.735, 0.772, 0.802, 0.927, 1.02,\
          1.101, 1.169, 1.275, 1.362, 1.436, 1.5, 1.556, 1.604, 1.651, 1.829, 1.96, 2.067, 2.147, 2.282, 2.388, 2.476, 2.55, 2.615, 2.672, 2.723, 2.921, 3.064,\
          3.173, 3.263, 3.406, 3.516, 3.608, 3.684, 3.75, 3.809, 3.86};
        std::vector<double> default_time_ = {0.01, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.5, 2, 2.5, 3, 4, 5, 6, 7, 8, 9, 10, 15,\
          20, 25, 30, 40, 50, 60, 70, 80, 90, 100, 150, 200, 250, 300, 400, 500, 600, 700, 800, 900, 1000};

        // Initialize function
        inline void init_object()
        {

        }

    };
}


#endif
