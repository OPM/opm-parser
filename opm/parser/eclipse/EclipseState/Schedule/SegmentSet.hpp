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
#include <map>

#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Segment.hpp>

namespace Opm {

    class SegmentSet {
    public:
        SegmentSet();

        std::string wellName() const;
        int numberBranch() const;
        int numberSegment() const;
        double depthTopSegment() const;
        double lengthTopSegment() const;
        double volumeTopSegment() const;
        double xTop() const;
        double yTop() const;

        WellSegment::LengthDepthEnum lengthDepthType() const;
        WellSegment::CompPresureDropEnum compPressureDrop() const;
        WellSegment::MultiPhaseModelEnum multiPhaseModel() const;
        std::vector<SegmentPtr>& Segments();

        bool numberToLocation(const int segment_number, int& location) const;

        int numberToLocation(const int segment_number) const;

        void addSegment(SegmentPtr new_segment);

        void addSegmentINC(SegmentPtr new_segment);

        void addSegmentABS(SegmentPtr new_segment);

        void segmentsFromWELSEGSKeyword(DeckKeywordConstPtr welsegsKeyword, const int nsegmx);

        SegmentSet* shallowCopy() const;

        SegmentPtr& operator[](size_t idx);

        SegmentPtr operator[](size_t idx) const;

    private:

        // name of the well
        std::string m_well_name;
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
        double m_x_top;
        double m_y_top;
        // There are other three properties for segment related to thermal conduction,
        // while they are not supported by the keyword at the moment.

        // std::vector<SegmentConstPtr> m_segments;
        std::vector<SegmentPtr> m_segments;
        // the mapping from the segment number to the
        // storage location in the vector
        std::map<int, int> m_number_to_location;
    };

    typedef std::shared_ptr<SegmentSet> SegmentSetPtr;
    typedef std::shared_ptr<const SegmentSet> SegmentSetConstPtr;
}

#endif
