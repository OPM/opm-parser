/*
  Copyright 2015 SINTEF ICT, Applied Mathematics.

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


#ifndef COMPSEGS_HPP_
#define COMPSEGS_HPP_

#include <memory>
#include <string>

#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>

namespace Opm {

    struct Compsegs {
        int m_i;
        int m_j;
        int m_k;
        int m_branch_number;
        double m_distance_start;
        double m_distance_end;

        WellCompletion::DirectionEnum m_dir;
        double m_center_depth;
        double m_thermal_length;
        int m_segment_number;

        Compsegs(int i_in, int j_in, int k_in, int branch_number_in, double distance_start_in, double distance_end_in,
                 WellCompletion::DirectionEnum dir_in, double center_depth_in, double thermal_length_in, int segment_number_in);

        static std::vector<std::shared_ptr<Compsegs>> compsegsFromCOMPSEGSKeyword(DeckKeywordConstPtr compsegsKeyword,
                                                                                        EclipseGridConstPtr grid);

    };

    typedef std::shared_ptr<Compsegs> CompsegsPtr;
    typedef std::shared_ptr<const Compsegs> CompsegsConstPtr;
}



#endif /* COMPSEGS_HPP_ */
