#include <iostream>
#include <cassert>
#include <cmath>
#include <map>

#include <opm/parser/eclipse/EclipseState/Schedule/Segment.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/SegmentSet.hpp>


namespace Opm {

    SegmentSet::SegmentSet() {
    }

    std::string SegmentSet::wellName() const {
        return m_well_name;
    }

    int SegmentSet::numberBranch() const {
        return m_number_branch;
    }

    int SegmentSet::numberSegment() const {
        return m_number_segment;
    }

    double SegmentSet::depthTopSegment() const {
        return m_depth_top;
    }

    double SegmentSet::lengthTopSegment() const {
        return m_length_top;
    }

    double SegmentSet::volumeTopSegment() const {
        return m_volume_top;
    }

    double SegmentSet::xTop() const {
        return m_x_top;
    }

    double SegmentSet::yTop() const {
        return m_y_top;
    }

    WellSegment::LengthDepthEnum SegmentSet::lengthDepthType() const {
        return m_length_depth_type;
    }

    WellSegment::CompPresureDropEnum SegmentSet::compPressureDrop() const {
        return m_comp_pressure_drop;
    }

    WellSegment::MultiPhaseModelEnum SegmentSet::multiPhaseModel() const {
        return m_multiphase_model;
    }

    std::vector<SegmentPtr>& SegmentSet::Segments() {
        return m_segments;
    }

    SegmentPtr& SegmentSet::operator[](size_t idx) {
        return m_segments[idx];
    }

    SegmentPtr SegmentSet::operator[](size_t idx) const {
        return m_segments[idx];
    }

    bool SegmentSet::numberToLocation(const int segment_number, int& location) const {
         // std::map<int, int>::iterator it;
         auto it = m_number_to_location.find(segment_number);
         if (it != m_number_to_location.end()) {
             location = it->second;
             return true;
         } else {
             return false;
         }
    }

    int SegmentSet::numberToLocation(const int segment_number) const {
        auto it = m_number_to_location.find(segment_number);
        if (it != m_number_to_location.end()) {
            return it->second;
        } else {
            return -1;
        }
    }

    void SegmentSet::addSegment(SegmentPtr new_segment) {
        // decide whether to push_back or insert
        bool insert = false;
        int segment_number = new_segment->segmentNumber();

        int segment_location; // storage location
        bool find = numberToLocation(segment_number, segment_location);

        if (!find) { // it is a new segment
            m_segments.push_back(new_segment);
            m_number_to_location[segment_number] = m_number_segment;
            ++ m_number_segment;
        } else { // the segment already exists
            m_segments[segment_location] = new_segment;
        }
    }

    void SegmentSet::addSegmentINC(SegmentPtr new_segment) {

    }

    void SegmentSet::addSegmentABS(SegmentPtr new_segment) {

    }

    SegmentSet* SegmentSet::shallowCopy() const {
        SegmentSet* copy = new SegmentSet();
        copy->m_well_name = m_well_name;
        copy->m_number_branch = m_number_branch;
        copy->m_number_segment = m_number_segment;
        copy->m_depth_top = m_depth_top;
        copy->m_length_top = m_length_top;
        copy->m_volume_top = m_volume_top;
        copy->m_length_depth_type = m_length_depth_type;
        copy->m_comp_pressure_drop = m_comp_pressure_drop;
        copy->m_multiphase_model = m_multiphase_model;
        copy->m_x_top = m_x_top;
        copy->m_y_top = m_y_top;
        copy->m_number_to_location = m_number_to_location;
        copy->m_segments.resize(m_segments.size());
        for (size_t i = 0; i < m_segments.size(); ++i) {
            copy->m_segments[i] = m_segments[i];
        }
        return copy;
    }

    void SegmentSet::segmentsFromWELSEGSKeyword(DeckKeywordConstPtr welsegsKeyword, const int nsegmx) {
        std::map<std::string, std::vector<SegmentPtr>> segmentMapList;
        // for the first record, which provides the information for the top segment
        // and information for the whole segment set
        DeckRecordConstPtr record1 = welsegsKeyword->getRecord(0);
        m_well_name = record1->getItem("WELL")->getTrimmedString(0);

        // create a temporary vector to store all the segments information in a sparse way
        // then compress the vector in a orderly way to m_segments
        std::vector<SegmentPtr> new_segments;
        new_segments.resize(nsegmx);

        for (size_t i = 0; i < new_segments.size(); ++i) {
            new_segments[i] = NULL;
        }

        const double meaningless_value = -1.e100; // meaningless value to indicate unspecified values

        m_depth_top = record1->getItem("DEPTH")->getRawDouble(0);
        m_length_top = record1->getItem("LENGTH")->getRawDouble(0);
        m_length_depth_type = WellSegment::LengthDepthEnumFromString(record1->getItem("INFO_TYPE")->getTrimmedString(0));
        m_volume_top = record1->getItem("WELLBORE_VOLUME")->getRawDouble(0);
        m_comp_pressure_drop = WellSegment::CompPressureDropEnumFromString(record1->getItem("PRESSURE_COMPONENTS")->getTrimmedString(0));
        m_multiphase_model = WellSegment::MultiPhaseModelEnumFromString(record1->getItem("FLOW_MODEL")->getTrimmedString(0));
        m_x_top = record1->getItem("TOP_X")->getRawDouble(0);
        m_y_top = record1->getItem("TOP_Y")->getRawDouble(0);

        // the main branch is 1 instead of 0.
        if (m_length_depth_type == WellSegment::INC) {
            SegmentPtr top_segment(new Segment(1, 1, 0, 0., 0., meaningless_value, meaningless_value, meaningless_value,
                                               m_volume_top, 0., 0., false));
            new_segments[0] = top_segment;
        } else if (m_length_depth_type == WellSegment::ABS) {
            SegmentPtr top_segment(new Segment(1, 1, 0, m_length_top, m_depth_top, meaningless_value, meaningless_value,
                                               meaningless_value, m_volume_top, m_x_top, m_y_top, true));
            new_segments[0] = top_segment;
        }

        // TODO: This assume all the segments are given with a continuous segment number
        // which means, if N segments are specified, the segment number will be 1...N.
        // which is not necessarily true. theoretically, it can be any number from 1 to NSEGMX.
        // m_segments.resize(number_segments);

        // read all the information out from the DECK first then process to get all the required information
        for (size_t recordIndex = 1; recordIndex < welsegsKeyword->size(); ++recordIndex) {
            DeckRecordConstPtr record = welsegsKeyword->getRecord(recordIndex);
            int K1 = record->getItem("SEGMENT1")->getInt(0);
            int K2 = record->getItem("SEGMENT2")->getInt(0);
            assert((K1 >= 2) && (K2 >= K1) && (K2 <= nsegmx));
            int branch = record->getItem("BRANCH")->getInt(0);
            int outlet_segment = record->getItem("JOIN_SEGMENT")->getInt(0);
            double diameter = record->getItem("DIAMETER")->getRawDouble(0);
            DeckItemConstPtr itemArea = record->getItem("AREA");
            double area;
            if (itemArea->hasValue(0)) {
                area = itemArea->getRawDouble(0);
            } else {
                area = M_PI * diameter * diameter / 4.0;
            }

            double segment_length;
            double depth_change;
            // if the values are incremental values, then we can just use the values
            // if the values are absolute values, then we need to calculate them during the next process
            // only the value for the last segment in the range is recorded
            segment_length = record->getItem("SEGMENT_LENGTH")->getRawDouble(0);
            depth_change = record->getItem("DEPTH_CHANGE")->getRawDouble(0);

            DeckItemConstPtr itemVolume = record->getItem("VOLUME");
            double volume;
            if (itemVolume->hasValue(0)) {
                volume = itemVolume->getRawDouble(0);
            } else if (m_length_depth_type == WellSegment::INC) {
                volume = area * segment_length;
            } else {
                volume = meaningless_value; // A * L, while L is not determined yet
            }

            double roughness = record->getItem("ROUGHNESS")->getRawDouble(0);

            double length_x;
            double length_y;

            length_x = record->getItem("LENGTH_X")->getRawDouble(0);
            length_y = record->getItem("LENGTH_Y")->getRawDouble(0);

            for (int i = K1; i <= K2; ++i) {
                // from the second segment in the range, the outlet segment is the previous segment in the segment
                if (i != K1) {
                    outlet_segment = i - 1;
                }

                if (m_length_depth_type == WellSegment::INC) {
                    new_segments[i - 1].reset(new Segment(i, branch, outlet_segment, segment_length, depth_change,
                                                        diameter, roughness, area, volume, length_x, length_y, false));
                } else if (i == K2) {
                    new_segments[i - 1].reset(new Segment(i, branch, outlet_segment, segment_length, depth_change,
                                                        diameter, roughness, area, volume, length_x, length_y, true));
                } else {
                    new_segments[i - 1].reset(new Segment(i, branch, outlet_segment, meaningless_value, meaningless_value,
                                                        diameter, roughness, area, volume, meaningless_value, meaningless_value, false));
                }
            }
        }

        // process to make all the values ready.
        // only required when the m_length_depth_type is ABS
        // TODO: VERIFYING THIS PART OF CODE.
        // TODO: IT IS ONLY WORKING FOR THE FIRST TIME HAVE WELSEGS FOR THE WELL
        // TODO: IT WILL BE MOVED TO Schedule.cpp or SegmentSet.cpp AS A PROCESSING PROCESS
        /* if (m_length_depth_type == WellSegment::ABS) {
            bool all_ready;
            do {
                all_ready = true;
                // skip the top segment
                for (size_t i = 1; i < new_segments.size(); ++i) {
                    if (new_segments[i]->m_length < 0.5 * meaningless_value) {
                        all_ready = false;
                        // check the outlet_segment value
                        size_t index_begin = i;
                        size_t outlet_segment = new_segments[i]->m_outlet_segment;
                        while ( new_segments[outlet_segment]->m_length < 0.5 * meaningless_value) {
                            index_begin = outlet_segment;
                            outlet_segment = new_segments[outlet_segment]->m_outlet_segment;
                        }

                        // the values from the outlet segment
                        double length_outlet = new_segments[outlet_segment]->m_length;
                        double depth_outlet = new_segments[outlet_segment]->m_depth;
                        double length_x_outlet = new_segments[outlet_segment]->m_length_x;
                        double length_y_outlet = new_segments[outlet_segment]->m_length_y;

                        // look for the last segment in the range
                        int index_end;
                        for (size_t j = index_begin+1; j < new_segments.size(); ++j) {
                            if (new_segments[j]->m_length > 0.5 * meaningless_value) {
                                index_end = j;
                                break;
                            }
                        }

                        // set the values for the segments in the range
                        int number_segments = index_end - index_begin + 1;

                        double length_last = new_segments[index_end]->m_length;
                        double depth_last = new_segments[index_end]->m_depth;
                        double length_x_last = new_segments[index_end]->m_length_x;
                        double length_y_last = new_segments[index_end]->m_length_y;

                        double length_segment = (length_last - length_outlet) / number_segments;
                        double depth_segment = (depth_last - depth_outlet) / number_segments;
                        double length_x_segment = (length_x_last - length_x_outlet) / number_segments;
                        double length_y_segment = (length_y_last - length_y_outlet) / number_segments;

                        for(size_t index = index_begin; index < index_end; ++index) {
                            new_segments[index]->m_length = length_outlet + (index - index_begin + 1) * length_segment;
                            new_segments[index]->m_depth = depth_outlet + (index - index_begin + 1) * depth_segment;
                            new_segments[index]->m_length_x = length_x_outlet + (index - index_begin + 1) * length_x_segment;
                            new_segments[index]->m_length_y = length_y_outlet + (index - index_begin + 1) * length_y_segment;

                        }
                    }
                    break;
                }

            } while (!all_ready);
        } */

        // compress new_segments to m_segments in a orderly way and generate the mapping.
        // TODO: The segment number is not necessarily reduced by 1
        // it is just a number like a name
        // counting the number of the segments
        int number_segments = 1;
        for (size_t recordIndex = 1; recordIndex < welsegsKeyword->size(); ++recordIndex) {
            DeckRecordConstPtr record = welsegsKeyword->getRecord(recordIndex);
            int K1 = record->getItem("SEGMENT1")->getInt(0);
            int K2 = record->getItem("SEGMENT2")->getInt(0);
            number_segments += K2 - K1 + 1;
        }
        m_segments.resize(number_segments);

        int i_segment = 0;
        for (int i = 0; i < new_segments.size(); ++i){
            if (new_segments[i] != NULL) {
                m_segments[i_segment] = new_segments[i];
                m_number_to_location[m_segments[i_segment]->segmentNumber()] = i_segment;
                ++i_segment;
            }
        }

        assert(i_segment == number_segments);

        m_number_segment = number_segments;
    }
}
