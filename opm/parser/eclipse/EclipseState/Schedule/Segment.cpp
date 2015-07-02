#include <opm/parser/eclipse/EclipseState/Schedule/Segment.hpp>


namespace Opm {

    Segment::Segment()
    : m_branch(-1),
      m_outlet_segment(-1),
      m_length(-1.e100),
      m_depth(-1.e100),
      m_internal_dimaeter(-1.e100),
      m_roughness(-1.e100),
      m_cross_area(-1.e100),
      m_volume(-1.e100),
      m_length_x(0.0),
      m_length_y(0.0)
    {
       /*  m_branch = -1;
        m_outlet_segment = -1;
        m_length = -1.e100;
        m_depth = -1.e100;
        m_internal_dimaeter = -1.e100;
        m_roughness = -1.e100;
        m_cross_area = -1.e100;
        m_volume = -1.e100;
        m_length_x = 0.0;
        m_length_y = 0.0; */
    }


    Segment::Segment(int branch, int outlet_segment, double length, double depth,
                     double internal_diameter, double roughness, double cross_area,
                     double volume, double length_x, double length_y)
    : m_branch(branch),
    m_outlet_segment(outlet_segment),
    m_length(length),
    m_depth(depth),
    m_internal_dimaeter(internal_diameter),
    m_roughness(roughness),
    m_cross_area(cross_area),
    m_volume(volume),
    m_length_x(length_x),
    m_length_y(length_y)
    {
    }


}
