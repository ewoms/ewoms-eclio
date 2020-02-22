/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ewoms/eclio/io/rst/header.hh>
#include <ewoms/eclio/io/rst/connection.hh>
#include <ewoms/eclio/output/vectoritems/connection.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;

namespace Ewoms {
namespace RestartIO {

namespace {

template <typename T>
T from_int(int);

template <>
Connection::State from_int(int int_state) {
    if (int_state == 1)
        return Connection::State::OPEN;

    return Connection::State::SHUT;
}

template <>
Connection::Direction from_int(int int_dir) {
    switch (int_dir) {
    case 1:
        return Connection::Direction::X;
    case 2:
        return Connection::Direction::Y;
    case 3:
        return Connection::Direction::Z;
    default:
        throw std::invalid_argument("Can not convert: " + std::to_string(int_dir) + " to string");
    }
}

}

using M  = ::Ewoms::UnitSystem::measure;

RstConnection::RstConnection(const ::Ewoms::UnitSystem& unit_system, const int* icon, const float* scon, const double* xcon) :
    insert_index(                                            icon[VI::IConn::SeqIndex] - 1),
    ijk(                                                    {icon[VI::IConn::CellI] - 1, icon[VI::IConn::CellJ] - 1, icon[VI::IConn::CellK] - 1}),
    state(                                                   from_int<Connection::State>(icon[VI::IConn::ConnStat])),
    drain_sat_table(                                         icon[VI::IConn::Drainage]),
    imb_sat_table(                                           icon[VI::IConn::Imbibition]),
    completion(                                              icon[VI::IConn::ComplNum] - 1),
    dir(                                                     from_int<Connection::Direction>(icon[VI::IConn::ConnDir])),
    segment(                                                 icon[VI::IConn::Segment] - 1),
    tran(          unit_system.to_si(M::transmissibility,    scon[VI::SConn::ConnTrans])),
    depth(         unit_system.to_si(M::length,              scon[VI::SConn::Depth])),
    diameter(      unit_system.to_si(M::length,              scon[VI::SConn::Diameter])),
    kh(            unit_system.to_si(M::effective_Kh,        scon[VI::SConn::EffectiveKH])),
    segdist_end(   unit_system.to_si(M::length,              scon[VI::SConn::SegDistEnd])),
    segdist_start( unit_system.to_si(M::length,              scon[VI::SConn::SegDistStart])),
    oil_rate(      unit_system.to_si(M::liquid_surface_rate, xcon[VI::XConn::OilRate])),
    water_rate(    unit_system.to_si(M::liquid_surface_rate, xcon[VI::XConn::WaterRate])),
    gas_rate(      unit_system.to_si(M::gas_surface_rate,    xcon[VI::XConn::GasRate])),
    pressure(      unit_system.to_si(M::pressure,            xcon[VI::XConn::Pressure])),
    resv_rate(     unit_system.to_si(M::rate,                xcon[VI::XConn::ResVRate]))
{}

}
}
