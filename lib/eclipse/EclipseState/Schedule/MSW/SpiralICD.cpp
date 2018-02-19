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
#include <cmath>


namespace Opm {

    SpiralICD::SpiralICD(const DeckRecord& record)
            : m_strength(record.getItem("STRENGTH").getSIDouble(0)),
              m_length(record.getItem("LENGTH").getSIDouble(0)),
              m_density_calibration(record.getItem("DENSITY_CALI").getSIDouble(0)),
              m_viscosity_calibration(record.getItem("VISCOSITY_CALI").getSIDouble(0)),
              m_critical_value(record.getItem("CRITICAL_VALUE").getSIDouble(0)),
              m_width_transition_region(record.getItem("WIDTH_TRANS").get<double>(0)),
              m_max_viscosity_ratio(record.getItem("MAX_VISC_RATIO").get<double>(0)),
              m_method_flow_scaling(record.getItem("METHOD_SCALING_FACTOR").get<int>(0)),
              m_max_absolute_rate(record.getItem("MAX_ABS_RATE").hasValue(0)
                                  ? record.getItem("MAX_ABS_RATE").getSIDouble(0)
                                  : std::numeric_limits<double>::max()),
              m_status(record.getItem("STATUS").getTrimmedString(0)),
              m_scaling_fractor(std::numeric_limits<double>::lowest())
    {
    }

    std::map<std::string, std::vector<std::pair<int, SpiralICD> > >
    SpiralICD::fromWSEGSICD(const DeckKeyword& wsegsicd)
    {
        std::map<std::string, std::vector<std::pair<int, SpiralICD> > > res;

        for (const DeckRecord &record : wsegsicd) {
            const std::string well_name = record.getItem("WELL").getTrimmedString(0);

            const int start_segment = record.getItem("SEG1").get<int>(0);
            const int end_segment = record.getItem("SEG2").get<int>(0);

            if (start_segment < 2 || end_segment < 2 || end_segment < start_segment) {
                const std::string message = "Segment numbers " + std::to_string(start_segment) + " and "
                                            + std::to_string(end_segment) + " specified in WSEGSICD for well " +
                                            well_name
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

    double SpiralICD::maxAbsoluteRate() const {
        return m_max_absolute_rate;
    }

    const std::string &SpiralICD::status() const {
        return m_status;
    }

    double SpiralICD::strength() const {
        return m_strength;
    }

    double SpiralICD::length() const {
        return m_length;
    }

    double SpiralICD::densityCalibration() const {
        return m_density_calibration;
    }

    double SpiralICD::viscosityCalibration() const
    {
        return m_viscosity_calibration;
    }

    double SpiralICD::criticalValue() const {
        return m_critical_value;
    }

    double SpiralICD::widthTransitionRegion() const
    {
        return m_width_transition_region;
    }

    double SpiralICD::maxViscosityRatio() const
    {
        return m_max_viscosity_ratio;
    }

    double SpiralICD::methodFlowScaling() const
    {
        return m_method_flow_scaling;
    }

    double SpiralICD::scalingFactor() const
    {
        if (m_scaling_fractor <= 0.)
            throw std::runtime_error("the scaling factor has invalid value " + std::to_string(m_scaling_fractor));

        return m_scaling_fractor;
    }

    void SpiralICD::updateScalingFactor(const double outlet_segment_length, const double completion_length)
    {
        if (m_method_flow_scaling < 0) {
            if (m_length > 0.) { // icd length / outlet segment length
                m_scaling_fractor = m_length / outlet_segment_length;
            } else if (m_length < 0.) {
                m_scaling_fractor = std::abs(m_length);
            } else { // icd length is zero, not sure the proper way to handle this yet
                throw std::logic_error("Zero-value length of SICD is found when calcuating scaling factor");
            }
        } else if (m_method_flow_scaling == 0) {
            if (m_length  <= 0.)
                throw std::logic_error("Non positive length of SICD if found when method of scaling is zero");

            m_scaling_fractor = m_length / outlet_segment_length;
        } else if (m_method_flow_scaling == 1) {
            m_scaling_fractor = std::abs(m_length);
        } else if (m_method_flow_scaling == 2) {
            m_scaling_fractor = m_length / completion_length;
        } else {
            throw std::logic_error(" invalid method specified to calculate flow scaling factor for SICD");
        }
    }
}
