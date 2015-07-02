#include <iostream>
#include <cassert>
#include <cmath>

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

    WellSegment::LengthDepthEnum SegmentSet::lengthDepthType() const {
        return m_length_depth_type;
    }

    WellSegment::CompPresureDropEnum SegmentSet::compPressureDrop() const {
        return m_comp_pressure_drop;
    }

    WellSegment::MultiPhaseModelEnum SegmentSet::multiPhaseModel() const {
        return m_multiphase_model;
    }

    void SegmentSet::segmentsFromWELSEGSKeyword(DeckKeywordConstPtr welsegsKeyword) {
        std::map<std::string, std::vector<SegmentPtr>> segmentMapList;
        // for the first record, which provides the information for the top segment
        // and information for the whole segment set
        DeckRecordConstPtr record1 = welsegsKeyword->getRecord(0);
        std::string well_name = record1->getItem("WELL")->getTrimmedString(0);

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
            SegmentPtr top_segment(new Segment(0, -1, 0., 0., meaningless_value, meaningless_value, meaningless_value,
                                               m_volume_top, 0., 0.));
            m_segments.push_back(top_segment);
        } else if (m_length_depth_type == WellSegment::ABS) {
            SegmentPtr top_segment(new Segment(0, -1, m_length_top, m_depth_top, meaningless_value, meaningless_value,
                                               meaningless_value, m_volume_top, m_x_top, m_y_top));
            m_segments.push_back(top_segment);
        }

        // counting the number of the segments
        int number_segments = 1;
        for (size_t recordIndex = 1; recordIndex < welsegsKeyword->size(); ++recordIndex) {
            DeckRecordConstPtr record = welsegsKeyword->getRecord(recordIndex);
            int K1 = record->getItem("SEGMENT1")->getInt(0);
            int K2 = record->getItem("SEGMENT2")->getInt(0);
            number_segments += K2 - K1 + 1;
        }

        m_segments.resize(number_segments);


        // read all the information out from the DECK first then process to get all the required information
        for (size_t recordIndex = 1; recordIndex < welsegsKeyword->size(); ++recordIndex) {
            DeckRecordConstPtr record = welsegsKeyword->getRecord(recordIndex);
            int K1 = record->getItem("SEGMENT1")->getInt(0);
            int K2 = record->getItem("SEGMENT2")->getInt(0);
            assert((K1 >= 2) && (K2 >= K1));
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
            DeckItemConstPtr itemVolume = record->getItem("VOLUME");
            double volume;
            if (itemVolume->hasValue(0)) {
                volume = itemVolume->getRawDouble(0);
            } else {
                volume = meaningless_value; // A * L
            }

            double segment_length;
            double depth_change;
            // if the values are incremental values, then we can just use the values
            // if the values are absolute values, then we need to calculate them during the next process
            // only the value for the last segment in the range is recorded
            segment_length = record->getItem("SEGMENT_LENGTH")->getRawDouble(0);
            depth_change = record->getItem("DEPTH_CHANGE")->getRawDouble(0);

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

                if ((m_length_depth_type == WellSegment::INC) || (i == K2)) {
                    m_segments[i - 1].reset(new Segment(branch, outlet_segment - 1, segment_length, depth_change,
                                                        diameter, roughness, area, volume, length_x, length_y));
                } else {
                    m_segments[i - 1].reset(new Segment(branch, outlet_segment - 1, meaningless_value, meaningless_value,
                                                        diameter, roughness, area, volume, meaningless_value, meaningless_value));
                }

            }
        }

        // process to make all the values ready.
        // only required when the m_length_depth_type is ABS
        // TODO: VERIFYING THIS PART OF CODE.
        if (m_length_depth_type == WellSegment::ABS) {
            bool all_ready;
            do {
                all_ready = true;
                // skip the top segment
                for (size_t i = 1; i < m_segments.size(); ++i) {
                    if (m_segments[i]->m_length < 0.5 * meaningless_value) {
                        all_ready = false;
                        // check the outlet_segment value
                        size_t index_begin = i;
                        size_t outlet_segment = m_segments[i]->m_outlet_segment;
                        while ( m_segments[outlet_segment]->m_length < 0.5 * meaningless_value) {
                            index_begin = outlet_segment;
                            outlet_segment = m_segments[outlet_segment]->m_outlet_segment;
                        }

                        // the values from the outlet segment
                        double length_outlet = m_segments[outlet_segment]->m_length;
                        double depth_outlet = m_segments[outlet_segment]->m_depth;
                        double length_x_outlet = m_segments[outlet_segment]->m_length_x;
                        double length_y_outlet = m_segments[outlet_segment]->m_length_y;

                        // look for the last segment in the range
                        int index_end;
                        for (size_t j = index_begin+1; j < m_segments.size(); ++j) {
                            if (m_segments[j]->m_length > 0.5 * meaningless_value) {
                                index_end = j;
                                break;
                            }
                        }

                        // set the values for the segments in the range
                        int number_segments = index_end - index_begin + 1;

                        double length_last = m_segments[index_end]->m_length;
                        double depth_last = m_segments[index_end]->m_depth;
                        double length_x_last = m_segments[index_end]->m_length_x;
                        double length_y_last = m_segments[index_end]->m_length_y;

                        double length_segment = (length_last - length_outlet) / number_segments;
                        double depth_segment = (depth_last - depth_outlet) / number_segments;
                        double length_x_segment = (length_x_last - length_x_outlet) / number_segments;
                        double length_y_segment = (length_y_last - length_y_outlet) / number_segments;

                        for(size_t index = index_begin; index < index_end; ++index) {
                            m_segments[index]->m_length = length_outlet + (index - index_begin + 1) * length_segment;
                            m_segments[index]->m_depth = depth_outlet + (index - index_begin + 1) * depth_segment;
                            m_segments[index]->m_length_x = length_x_outlet + (index - index_begin + 1) * length_x_segment;
                            m_segments[index]->m_length_y = length_y_outlet + (index - index_begin + 1) * length_y_segment;

                        }
                    }
                    break;
                }

            } while (!all_ready);
        }

    }
}
