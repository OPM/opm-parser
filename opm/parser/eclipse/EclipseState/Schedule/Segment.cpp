#include <opm/parser/eclipse/EclipseState/Schedule/Segment.hpp>


namespace Opm {

    Segment::Segment()
    : m_segment_number(-1),
      m_branch(-1),
      m_outlet_segment(-1),
      m_length(-1.e100),
      m_depth(-1.e100),
      m_internal_diameter(-1.e100),
      m_roughness(-1.e100),
      m_cross_area(-1.e100),
      m_volume(-1.e100),
      m_length_x(0.0),
      m_length_y(0.0),
      m_data_ready(false)
    {
    }


    Segment::Segment(int segment_number, int branch, int outlet_segment, double length, double depth,
                     double internal_diameter, double roughness, double cross_area,
                     double volume, double length_x, double length_y, bool data_ready)
    : m_segment_number(segment_number),
      m_branch(branch),
      m_outlet_segment(outlet_segment),
      m_length(length),
      m_depth(depth),
      m_internal_diameter(internal_diameter),
      m_roughness(roughness),
      m_cross_area(cross_area),
      m_volume(volume),
      m_length_x(length_x),
      m_length_y(length_y),
      m_data_ready(data_ready)
    {
    }


    int& Segment::segmentNumber() {
        return m_segment_number;
    }


    int& Segment::branchNumber() {
        return m_branch;
    }


    int& Segment::outletSegment() {
        return m_outlet_segment;
    }


    double& Segment::length() {
        return m_length;
    }


    double& Segment::depth() {
        return m_depth;
    }


    double& Segment::internalDiameter() {
        return m_internal_diameter;
    }


    double& Segment::roughness() {
        return m_roughness;
    }


    double& Segment::crossArea() {
        return m_cross_area;
    }


    double& Segment::volume() {
        return m_volume;
    }


    double& Segment::lengthX() {
        return m_length_x;
    }


    double& Segment::lengthY() {
        return m_length_y;
    }


    bool& Segment::dataReady() {
        return m_data_ready;
    }
}
