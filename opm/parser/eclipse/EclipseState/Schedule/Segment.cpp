#include <opm/parser/eclipse/EclipseState/Schedule/Segment.hpp>


namespace Opm {

    Segment::Segment() {
        m_branch = -1;
        m_outlet_segment = -1;
        m_length = -1.e100;
        m_depth = -1.e100;
        m_internal_dimaeter = -1.e100;
        m_roughness = -1.e100;
        m_cross_area = -1.e100;
        m_volume = -1.e100;
        m_length_x = 0.0;
        m_length_y = 0.0;
    }

}
