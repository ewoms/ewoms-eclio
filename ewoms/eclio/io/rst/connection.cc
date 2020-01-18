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

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;

namespace Ewoms {
namespace RestartIO {

Connection::Connection(const int* icon, const float* scon, const double* xcon) :
    insert_index(icon[VI::IConn::SeqIndex]),
    ijk({icon[VI::IConn::CellI] - 1, icon[VI::IConn::CellJ] - 1, icon[VI::IConn::CellK] - 1}),
    status(icon[VI::IConn::ConnStat]),
    drain_sat_table(icon[VI::IConn::Drainage]),
    imb_sat_table(icon[VI::IConn::Imbibition]),
    completion(icon[VI::IConn::ComplNum] - 1),
    dir(icon[VI::IConn::ConnDir]),
    segment(icon[VI::IConn::Segment]),
    tran(scon[VI::SConn::ConnTrans]),
    depth(scon[VI::SConn::Depth]),
    diameter(scon[VI::SConn::Diameter]),
    kh(scon[VI::SConn::EffectiveKH]),
    segdist_end(scon[VI::SConn::SegDistEnd]),
    segdist_start(scon[VI::SConn::SegDistStart]),
    oil_rate(xcon[VI::XConn::OilRate]),
    water_rate(xcon[VI::XConn::WaterRate]),
    gas_rate(xcon[VI::XConn::GasRate]),
    pressure(xcon[VI::XConn::Pressure]),
    resv_rate(xcon[VI::XConn::ResVRate])
{}

}
}
