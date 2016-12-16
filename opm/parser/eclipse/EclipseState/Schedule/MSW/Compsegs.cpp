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

#include <cmath>

#include <opm/parser/eclipse/Parser/Deck.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/CompletionSet.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/MSW/Compsegs.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/MSW/SegmentSet.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/C.hpp>

namespace Opm {


    Compsegs::Compsegs(int i_in, int j_in, int k_in, int branch_number_in, double distance_start_in, double distance_end_in,
                       WellCompletion::DirectionEnum dir_in, double center_depth_in, int segment_number_in)
    : m_i(i_in),
      m_j(j_in),
      m_k(k_in),
      m_branch_number(branch_number_in),
      m_distance_start(distance_start_in),
      m_distance_end(distance_end_in),
      m_dir(dir_in),
      m_center_depth(center_depth_in),
      m_segment_number(segment_number_in)
    {
    }

    std::vector< Compsegs > Compsegs::compsegsFromCOMPSEGSKeyword( const DeckKeyword& compsegsKeyword ) {

        // only handle the second record here
        // The first record here only contains the well name
        std::vector< Compsegs > compsegs;

        for (size_t recordIndex = 1; recordIndex < compsegsKeyword.size(); ++recordIndex) {
            const auto& record = compsegsKeyword.getRecord(recordIndex);
            // following the coordinate rule for completions
            const int I = record.getItem<ParserKeywords::COMPSEGS::I>().get< int >(0) - 1;
            const int J = record.getItem<ParserKeywords::COMPSEGS::J>().get< int >(0) - 1;
            const int K = record.getItem<ParserKeywords::COMPSEGS::K>().get< int >(0) - 1;
            const int branch = record.getItem<ParserKeywords::COMPSEGS::BRANCH>().get< int >(0);

            double distance_start;
            double distance_end;
            if (record.getItem<ParserKeywords::COMPSEGS::DISTANCE_START>().hasValue(0)) {
                distance_start = record.getItem<ParserKeywords::COMPSEGS::DISTANCE_START>().getSIDouble(0);
            } else if (recordIndex == 1) {
                distance_start = 0.;
            } else {
                // TODO: the end of the previous connection or range
                // 'previous' should be in term of the input order
                // since basically no specific order for the completions
                throw std::runtime_error("this way to obtain DISTANCE_START not implemented yet!");
            }
            if (record.getItem<ParserKeywords::COMPSEGS::DISTANCE_END>().hasValue(0)) {
                distance_end = record.getItem<ParserKeywords::COMPSEGS::DISTANCE_END>().getSIDouble(0);
            } else {
                // TODO: the distance_start plus the thickness of the grid block
                throw std::runtime_error("this way to obtain DISTANCE_END not implemented yet!");
            }

            if( !record.getItem< ParserKeywords::COMPSEGS::DIRECTION >().hasValue( 0 ) &&
                !record.getItem< ParserKeywords::COMPSEGS::DISTANCE_END >().hasValue( 0 ) ) {
                throw std::runtime_error("the direction has to be specified when DISTANCE_END in the record is not specified");
            }

            if( record.getItem< ParserKeywords::COMPSEGS::END_IJK >().hasValue( 0 ) &&
               !record.getItem< ParserKeywords::COMPSEGS::DIRECTION >().hasValue( 0 ) ) {
                throw std::runtime_error("the direction has to be specified when END_IJK in the record is specified");
            }

            /*
             * Defaulted well completion. Must be non-defaulted if DISTANCE_END
             * is set or a range is specified. If not this is effectively ignored.
             */
            WellCompletion::DirectionEnum direction = WellCompletion::X;
            if( record.getItem< ParserKeywords::COMPSEGS::DIRECTION >().hasValue( 0 ) ) {
                direction = WellCompletion::DirectionEnumFromString(record.getItem<ParserKeywords::COMPSEGS::DIRECTION>().get< std::string >(0));
            }

            double center_depth;
            if (!record.getItem<ParserKeywords::COMPSEGS::CENTER_DEPTH>().defaultApplied(0)) {
                center_depth = record.getItem<ParserKeywords::COMPSEGS::CENTER_DEPTH>().getSIDouble(0);
            } else {
                // 0.0 is also the defaulted value
                // which is used to indicate to obtain the final value through related segment
                center_depth = 0.;
            }

            if (center_depth < 0.) {
                //TODO: get the depth from COMPDAT data.
                throw std::runtime_error("this way to obtain CENTER_DISTANCE not implemented yet either!");
            }

            int segment_number;
            if (record.getItem<ParserKeywords::COMPSEGS::SEGMENT_NUMBER>().hasValue(0)) {
                segment_number = record.getItem<ParserKeywords::COMPSEGS::SEGMENT_NUMBER>().get< int >(0);
            } else {
                segment_number = 0;
                // will decide the segment number based on the distance in a process later.
            }

            if (!record.getItem<ParserKeywords::COMPSEGS::END_IJK>().hasValue(0)) { // only one compsegs
                compsegs.emplace_back( I, J, K,
                                       branch,
                                       distance_start, distance_end,
                                       direction,
                                       center_depth,
                                       segment_number );
            } else { // a range is defined. genrate a range of Compsegs
                throw std::runtime_error("entering COMPSEGS entries with a range is not supported yet!");
            }
        }

        return compsegs;
    }

    void Compsegs::processCOMPSEGS(std::vector< Compsegs >& compsegs, const SegmentSet& segment_set) {
        // for the current cases we have at the moment, the distance information is specified explicitly,
        // while the depth information is defaulted though, which need to be obtained from the related segment
        for( auto& compseg : compsegs ) {

            // need to determine the related segment number first
            if (compseg.m_segment_number != 0) continue;

            const double center_distance = (compseg.m_distance_start + compseg.m_distance_end) / 2.0;
            const int branch_number = compseg.m_branch_number;

            int segment_number = 0;
            double min_distance_difference = 1.e100; // begin with a big value
            for (int i_segment = 0; i_segment < segment_set.numberSegment(); ++i_segment) {
                const Segment& current_segment = segment_set[i_segment];
                if( branch_number != current_segment.branchNumber() ) continue;

                const double distance = current_segment.totalLength();
                const double distance_difference = std::abs(center_distance - distance);
                if (distance_difference < min_distance_difference) {
                    min_distance_difference = distance_difference;
                    segment_number = current_segment.segmentNumber();
                }
            }

            if (segment_number == 0) {
                throw std::runtime_error("The perforation failed in finding a related segment \n");
            }

            if (compseg.m_center_depth < 0.) {
                throw std::runtime_error("Obtaining perforation depth from COMPDAT data is not supported yet");
            }

            compseg.m_segment_number = segment_number;
            if (compseg.m_center_depth == 0.) {
                // using the depth of the segment node as the depth of the completion
                // TODO: now only one completion for one segment is hanlded,
                // TODO: later we will try to handle more than one completion for each segment,
                // TODO: which will be a linear interpolation based on the segment node depth
                // TODO: in the same branch, while the actually way is not clear yet
                const int segment_location = segment_set.numberToLocation(segment_number);
                compseg.m_center_depth = segment_set[segment_location].depth();
            }
        }
    }


    void Compsegs::updateCompletionsWithSegment(const std::vector< Compsegs >& compsegs,
                                                CompletionSet& completion_set) {

        for( const auto& compseg : compsegs ) {
            const int i = compseg.m_i;
            const int j = compseg.m_j;
            const int k = compseg.m_k;

            auto new_completion = completion_set.getFromIJK( i, j, k );
            new_completion.attachSegment(compseg.m_segment_number, compseg.m_center_depth);
            completion_set.add(new_completion);
        }

        for (size_t ic = 0; ic < completion_set.size(); ++ic) {
            if ( !(completion_set.get(ic).attachedToSegment()) ) {
                throw std::runtime_error("Not all the completions are attached with a segment. "
                                         "The information from COMPSEGS is not complete");
            }
        }
    }
}
