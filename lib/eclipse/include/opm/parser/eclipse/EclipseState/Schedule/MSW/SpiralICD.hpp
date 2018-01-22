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

#ifndef SPIRALICD_HPP_HEADER_INCLUDED
#define SPIRALICD_HPP_HEADER_INCLUDED

#include <map>
#include <utility>
#include <vector>


namespace Opm {

    class DeckRecord;
    class DeckKeyword;

    class SpiralICD {
    public:
        double strength;
        double length;
        double density_calibration;
        double viscosity_calibration;
        double critical_value;
        double width_transition_region;
        double max_viscosity_ratio;
        int method_flow_scaling;
        // negative value means no limit for max_absolute_rate
        double max_absolute_rate;
        std::string status;

        explicit SpiralICD(const DeckRecord& record);

        // the function will return a map
        // [
        //     "WELL1" : [<seg1, sicd1>, <seg2, sicd2> ...]
        //     ....
        static std::map<std::string, std::vector<std::pair<int, SpiralICD> > >
        fromWSEGSICD(const DeckKeyword& wsegsicd);
};

}

#endif
