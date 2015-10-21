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

#include <memory>

#include <opm/parser/eclipse/Deck/DeckKeyword.hpp>
#include <opm/parser/eclipse/Deck/DeckRecord.hpp>


namespace Opm {

    class Segment {
    public:
        Segment();

        Segment(int segment_number, int branch, int outlet_segment, double length, double depth,
                double internal_diameter, double roughness, double cross_area,
                double volume, double length_x, double length_y, bool data_ready);

        int& segmentNumber();
        int& branchNumber();
        int& outletSegment();
        double& length();
        double& depth();
        double& internalDiameter();
        double& roughness();
        double& crossArea();
        double& volume();
        double& lengthX();
        double& lengthY();
        bool& dataReady();

    private:
        // segment number
        // it should work as a ID.
        int m_segment_number;
        // branch number
        // for top segment, it should always be 1
        int m_branch;
        // the outlet junction segment
        // for top segment, it should be -1
        int m_outlet_segment;
        // length of the nodes
        // depending on item 5 in the record 1
        // 'INC' or 'ABS'
        // if it is 'INC', for top segment, it will be 0
        // in the class Well, it always stores the 'ABS' value.
        double m_length;
        // depth of the nodes
        // depending on item 5 in the record 1
        // 'INC' or 'ABS'
        // if it is 'INC', for top segment, it will be 0
        // TODO: to check if it is good to use 'ABS' always.
        // since it is easy to compute the 'INC' value with the 'ABS' value
        // while not easy the other way.
        // in the class Well, it always stores the 'ABS' value.
        double m_depth;
        // tubing internal diameter
        // or the equivalent diameter for annular cross-sections
        // for top segment, it is UNDEFINED
        // we use -1e100 for the top segment
        double m_internal_diameter;
        // effective roughness of the tubing
        // used to calculate the Fanning friction factor
        // for top segment, it is UNDEFINED
        // we use -1e100 for the top segment
        double m_roughness;
        // cross-sectional area for fluid flow
        // not defined for the top segment,
        // we use -1e100 for the top segment.
        double m_cross_area;
        // valume of the segment;
        // it is defined for top segment.
        // TODO: to check if the definition is the same with other segments.
        double m_volume;
        // length of segment projected onto the X-axis and Y-axis
        // depending on item 5 in the record 1
        // 'INC' or 'ABS'
        // they are only used for plotting purpose
        // it is is 'INC', for top segment, they will be 0
        // in the class Well, it always stores the 'ABS' value.
        double m_length_x;
        double m_length_y;
        // indicate if the data related to 'INC' or 'ABS' is ready
        // the volume will be updated at a final step.
        bool m_data_ready;
        // There are other three properties for segment related to thermal conduction,
        // while they are not supported by the keyword at the moment.

    };

    typedef std::shared_ptr<Segment> SegmentPtr;
    typedef std::shared_ptr<const Segment> SegmentConstPtr;

}

#endif
