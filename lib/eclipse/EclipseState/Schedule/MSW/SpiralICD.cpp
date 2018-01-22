/*
  Copyright 2017 SINTEF Digital, Mathematics and Cybernetics.

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

#include <opm/parser/eclipse/EclipseState/Schedule/MSW/SpiralICD.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>

#include <limits>


namespace Opm {

    SpiralICD::SpiralICD(const DeckRecord& record)
        : strength( record.getItem("STRENGTH").getSIDouble(0) ),
          length(record.getItem("LENGTH").getSIDouble(0) ),
          density_calibration(record.getItem("DENSITY_CALI").getSIDouble(0) ),
          viscosity_calibration(record.getItem("VISCOSITY_CALI").getSIDouble(0) ),
          critical_value(record.getItem("CRITICAL_VALUE").getSIDouble(0) ),
          width_transition_region(record.getItem("WIDTH_TRANS").get< double >(0) ),
          max_viscosity_ratio(record.getItem("MAX_VISC_RATIO").get< double >(0) ),
          method_flow_scaling(record.getItem("METHOD_SCALING_FACTOR").get < int >(0) ),
          max_absolute_rate(record.getItem("MAX_ABS_RATE").hasValue(0)
                            ? record.getItem("MAX_ABS_RATE").getSIDouble(0)
                            : std::numeric_limits<double>::max() ),
          status(record.getItem("STATUS").getTrimmedString(0) )
    {
    }

    std::map<std::string, std::vector<std::pair<int, SpiralICD> > >
    SpiralICD::fromWSEGSICD(const DeckKeyword& wsegsicd)
    {
        std::map<std::string, std::vector<std::pair<int, SpiralICD> > > res;

        for (const DeckRecord& record : wsegsicd) {
            const std::string well_name = record.getItem("WELL").getTrimmedString(0);

            const int start_segment = record.getItem("SEG1").get< int >(0);
            const int end_segment = record.getItem("SEG2").get< int >(0);

            if (start_segment < 2 || end_segment < 2 || end_segment < start_segment) {
                const std::string message = "Segment numbers " + std::to_string(start_segment) + " and "
                                            + std::to_string(end_segment) + " specified in WSEGSICD for well " + well_name
                                            + " are illegal ";
                throw std::invalid_argument(message);
            }

            const SpiralICD spiral_icd(record);
            for (int seg = start_segment; seg <= end_segment; seg++) {
                res[well_name].push_back(std::make_pair(seg, spiral_icd));
            }
        }

        return res;
    }
}
