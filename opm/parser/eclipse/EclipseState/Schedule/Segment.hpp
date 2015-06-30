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

#ifndef SEGMENT_HPP_HEADER_INCLUDED
#define SEGMENT_HPP_HEADER_INCLUDED

namespace Opm {

    struct Segment {

        Segment();

        // branch number
        int m_branch;
        // the outlet junction segment
        int m_outlet_segment;
        // length of the nodes
        // depending on item 5 in the record 1
        // 'INC' or 'ABS'
        double m_length;
        // depth of the nodes
        // depending on item 5 in the record 1
        // 'INC' or 'ABS'
        double m_depth;
        // tubing internal diameter
        // or the equivalent diameter for annular cross-sections
        double m_internal_dimaeter;
        // effective roughness of the tubing
        // used to calculate the Fanning friction factor
        double m_roughness;
        // cross-sectional area for fluid flow
        double m_cross_area;
        // valume of the segment;
        double m_volume;
        // length of segment projected onto the X-axis and Y-axis
        // depending on item 5 in the record 1
        // 'INC' or 'ABS'
        // they are only used for plotting purpose
        double m_length_x;
        double m_length_y;
        // There are other three properties for segment related to thermal conduction,
        // while they are not supported by the keyword at the moment.

    };
}

#endif
