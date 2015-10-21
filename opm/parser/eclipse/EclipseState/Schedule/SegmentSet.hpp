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

#ifndef SEGMENTSET_HPP_HEADER_INCLUDED
#define SEGMENTSET_HPP_HEADER_INCLUDED

#include <vector>

#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>

namespace Opm {

    struct Segment;

    class SegmentSet {
    public:
        SegmentSet();
        int numberBranch() const;
        int numberSegment() const;
        double depthTopSegment() const;
        double lengthTopSegment() const;
        double volumeTopSegment() const;
        WellSegment::LengthDepthEnum lengthDepthType() const;
        WellSegment::CompPresureDropEnum compPressureDrop() const;
        WellSegment::MultiPhaseModelEnum multiPhaseModel() const;

    private:
        // number of the branches
        int m_number_branch;
        // number of the segments
        int m_number_segment;
        // depth of the nodal point of the top segment
        // it is taken as the BHP reference depth of the well
        // BHP reference depth data from elsewhere will be ignored for multi-segmented wells
        double m_depth_top;
        // length down the tubing to the nodal point of the top segment
        double m_length_top;
        // effective wellbore volume of the top segment
        double m_volume_top;
        // type of the tubing length and depth information
        WellSegment::LengthDepthEnum m_length_depth_type;
        // components of the pressure drop to be included
        WellSegment::CompPresureDropEnum m_comp_pressure_drop;
        // multi-phase flow model
        WellSegment::MultiPhaseModelEnum m_multiphase_model;
        // X and Y cooridnate of the nodal point of the top segment
        int m_x_top;
        int m_y_top;
        // There are other three properties for segment related to thermal conduction,
        // while they are not supported by the keyword at the moment.

        std::vector<SegmentConstPtr> m_segments;
    };

    typedef std::shared_ptr<SegmentSet> SegmentSetPtr;
    typedef std::shared_ptr<const SegmentSet> SegmentSetConstPtr;
}

#endif
