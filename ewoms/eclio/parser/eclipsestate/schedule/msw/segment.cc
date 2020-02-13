/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <ewoms/eclio/parser/eclipsestate/schedule/msw/segment.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/msw/spiralicd.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/msw/valve.hh>

#include <cassert>

namespace Ewoms {
namespace {

static constexpr double invalid_value = -1.e100;

}

    Segment::Segment()
    : m_segment_number(-1),
      m_branch(-1),
      m_outlet_segment(-1),
      m_total_length(invalid_value),
      m_depth(invalid_value),
      m_internal_diameter(invalid_value),
      m_roughness(invalid_value),
      m_cross_area(invalid_value),
      m_volume(invalid_value),
      m_data_ready(false)
    {
    }

    Segment::Segment(int segment_number_in, int branch_in, int outlet_segment_in, double length_in, double depth_in,
                     double internal_diameter_in, double roughness_in, double cross_area_in,
                     double volume_in, bool data_ready_in, SegmentType segment_type_in)
    : m_segment_number(segment_number_in),
      m_branch(branch_in),
      m_outlet_segment(outlet_segment_in),
      m_total_length(length_in),
      m_depth(depth_in),
      m_internal_diameter(internal_diameter_in),
      m_roughness(roughness_in),
      m_cross_area(cross_area_in),
      m_volume(volume_in),
      m_data_ready(data_ready_in),
      m_segment_type(segment_type_in)
    {
    }

    Segment::Segment(int segmentNum, int branchNum, int outletSeg,
                     const std::vector<int>& inletSegs,
                     double totalLen, double dept, double internalDiam,
                     double rough, double crossA, double vol,
                     bool dr, SegmentType segment,
                     std::shared_ptr<SpiralICD> spiralIcd,
                     std::shared_ptr<Valve> valv)
    : m_segment_number(segmentNum),
      m_branch(branchNum),
      m_outlet_segment(outletSeg),
      m_inlet_segments(inletSegs),
      m_total_length(totalLen),
      m_depth(dept),
      m_internal_diameter(internalDiam),
      m_roughness(rough),
      m_cross_area(crossA),
      m_volume(vol),
      m_data_ready(dr),
      m_segment_type(segment),
      m_spiral_icd(spiralIcd),
      m_valve(valv)
  {
  }

    int Segment::segmentNumber() const {
        return m_segment_number;
    }

    int Segment::branchNumber() const {
        return m_branch;
    }

    int Segment::outletSegment() const {
        return m_outlet_segment;
    }

    double Segment::totalLength() const {
        return m_total_length;
    }

    double Segment::depth() const {
        return m_depth;
    }

    double Segment::internalDiameter() const {
        return m_internal_diameter;
    }

    double Segment::roughness() const {
        return m_roughness;
    }

    double Segment::crossArea() const {
        return m_cross_area;
    }

    double Segment::volume() const {
        return m_volume;
    }

    bool Segment::dataReady() const {
        return m_data_ready;
    }

    Segment::SegmentType Segment::segmentType() const {
        return m_segment_type;
    }

    void Segment::setDepthAndLength(const double depth_in,  const double length_in) {
        m_total_length = length_in;
        m_depth = depth_in;
        m_data_ready = true;
    }

    void Segment::setVolume(const double volume_in) {
        m_volume = volume_in;
    }

    const std::vector<int>& Segment::inletSegments() const {
        return m_inlet_segments;
    }

    void Segment::addInletSegment(const int segment_number_in) {
        m_inlet_segments.push_back(segment_number_in);
    }

    double Segment::invalidValue() {
        return invalid_value;
    }

    bool Segment::operator==( const Segment& rhs ) const {
        return this->m_segment_number    == rhs.m_segment_number
            && this->m_branch            == rhs.m_branch
            && this->m_outlet_segment    == rhs.m_outlet_segment
            && this->m_total_length      == rhs.m_total_length
            && this->m_depth             == rhs.m_depth
            && this->m_internal_diameter == rhs.m_internal_diameter
            && this->m_roughness         == rhs.m_roughness
            && this->m_cross_area        == rhs.m_cross_area
            && this->m_volume            == rhs.m_volume
            && this->m_data_ready        == rhs.m_data_ready;
    }

    bool Segment::operator!=( const Segment& rhs ) const {
        return !this->operator==(rhs);
    }

    void Segment::updateSpiralICD(const SpiralICD& spiral_icd) {
        m_segment_type = SegmentType::SICD;
        m_spiral_icd = std::make_shared<SpiralICD>(spiral_icd);
    }

    const std::shared_ptr<SpiralICD>& Segment::spiralICD() const {
        return m_spiral_icd;
    }

    void Segment::updateValve(const Valve& valve, const double segment_length) {
        // we need to update some values for the vale
        auto valve_ptr = std::make_shared<Valve>(valve);

        if (valve_ptr->pipeAdditionalLength() < 0.) { // defaulted for this
            valve_ptr->setPipeAdditionalLength(segment_length);
        }

        if (valve_ptr->pipeDiameter() < 0.) {
            valve_ptr->setPipeDiameter(m_internal_diameter);
        } else {
            this->m_internal_diameter = valve_ptr->pipeDiameter();
        }

        if (valve_ptr->pipeRoughness() < 0.) {
            valve_ptr->setPipeRoughness(m_roughness);
        } else {
            this->m_roughness = valve_ptr->pipeRoughness();
        }

        if (valve_ptr->pipeCrossArea() < 0.) {
            valve_ptr->setPipeCrossArea(m_cross_area);
        } else {
            this->m_cross_area = valve_ptr->pipeCrossArea();
        }

        if (valve_ptr->conMaxCrossArea() < 0.) {
            valve_ptr->setConMaxCrossArea(valve_ptr->pipeCrossArea());
        }

        this->m_valve = valve_ptr;

        m_segment_type = SegmentType::VALVE;
    }

    const Valve* Segment::valve() const {
        return m_valve.get();
    }

    const std::shared_ptr<Valve>& Segment::getValve() const {
        return m_valve;
    }

    int Segment::ecl_type_id() const {
        switch (this->m_segment_type) {
        case SegmentType::REGULAR:
            return -1;
        case SegmentType::SICD:
            return -7;
        case SegmentType::AICD:
            return -8;
        case SegmentType::VALVE:
            return -5;
        default:
            throw std::invalid_argument("Unhanedled segment type");
        }
    }

    Segment::SegmentType Segment::type_from_int(int ecl_id) {
        switch(ecl_id) {
        case -1:
            return SegmentType::REGULAR;
        case -7:
            return SegmentType::SICD;
        case -8:
            return SegmentType::AICD;
        case -5:
            return SegmentType::VALVE;
        default:
            throw std::invalid_argument("Unhanedled segment type");
        }
    }
}

