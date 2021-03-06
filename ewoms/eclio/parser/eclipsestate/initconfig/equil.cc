// -*- mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
// vi: set et ts=4 sw=4 sts=4:
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
#include "config.h"

#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/equil.hh>

namespace Ewoms {

    EquilRecord::EquilRecord() :
        EquilRecord(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0)
  {
  }

   EquilRecord::EquilRecord( double datum_depth_arg, double datum_depth_pc_arg, double woc_depth, double woc_pc, double goc_depth, double goc_pc, bool live_oil_init, bool wet_gas_init, int target_accuracy) :
       datum_depth(datum_depth_arg),
       datum_depth_ps(datum_depth_pc_arg),
       water_oil_contact_depth(woc_depth),
       water_oil_contact_capillary_pressure(woc_pc),
       gas_oil_contact_depth(goc_depth),
       gas_oil_contact_capillary_pressure(goc_pc),
       live_oil_init_proc(live_oil_init),
       wet_gas_init_proc(wet_gas_init),
       init_target_accuracy(target_accuracy)
    {}

    double EquilRecord::datumDepth() const {
        return this->datum_depth;
    }

    double EquilRecord::datumDepthPressure() const {
        return this->datum_depth_ps;
    }

    double EquilRecord::waterOilContactDepth() const {
        return this->water_oil_contact_depth;
    }

    double EquilRecord::waterOilContactCapillaryPressure() const {
        return this->water_oil_contact_capillary_pressure;
    }

    double EquilRecord::gasOilContactDepth() const {
        return this->gas_oil_contact_depth;
    }

    double EquilRecord::gasOilContactCapillaryPressure() const {
        return this->gas_oil_contact_capillary_pressure;
    }

    bool EquilRecord::liveOilInitConstantRs() const {
        return this->live_oil_init_proc;
    }

    bool EquilRecord::wetGasInitConstantRv() const {
        return this->wet_gas_init_proc;
    }

    int EquilRecord::initializationTargetAccuracy() const {
        return this->init_target_accuracy;
    }

    bool EquilRecord::operator==(const EquilRecord& data) const {
        return datum_depth == data.datum_depth &&
               datum_depth_ps == data.datum_depth_ps &&
               water_oil_contact_depth == data.water_oil_contact_depth &&
               water_oil_contact_capillary_pressure ==
               data.water_oil_contact_capillary_pressure &&
               data.gas_oil_contact_depth == data.gas_oil_contact_depth &&
               gas_oil_contact_capillary_pressure ==
               data.gas_oil_contact_capillary_pressure &&
               live_oil_init_proc == data.live_oil_init_proc &&
               wet_gas_init_proc == data.wet_gas_init_proc &&
               init_target_accuracy == data.init_target_accuracy;
    }

    /* */

    Equil::Equil( const DeckKeyword& keyword )
    {
        for (const auto& record : keyword) {
            auto datum_depth_arg = record.getItem( 0 ).getSIDouble( 0 );
            auto datum_depth_pc_arg = record.getItem( 1 ).getSIDouble( 0 );
            auto woc_depth = record.getItem(2).getSIDouble(0);
            auto woc_pc = record.getItem(3).getSIDouble(0);
            auto goc_depth = record.getItem(4).getSIDouble(0);
            auto goc_pc = record.getItem(5).getSIDouble(0);
            auto live_oil_init = record.getItem(6).get<int>(0) <= 0;
            auto wet_gas_init = record.getItem(7).get<int>(0) <= 0;
            auto target_accuracy = record.getItem(8).get<int>(0);

            this->m_records.push_back( EquilRecord(datum_depth_arg, datum_depth_pc_arg, woc_depth, woc_pc, goc_depth, goc_pc, live_oil_init, wet_gas_init, target_accuracy) );
        }
    }

    Equil Equil::serializeObject()
    {
        Equil result;
        result.m_records = {{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, true, false, 1}};

        return result;
    }

    const EquilRecord& Equil::getRecord( size_t id ) const {
        return this->m_records.at( id );
    }

    size_t Equil::size() const {
        return this->m_records.size();
    }

    bool Equil::empty() const {
        return this->m_records.empty();
    }

    Equil::const_iterator Equil::begin() const {
        return this->m_records.begin();
    }

    Equil::const_iterator Equil::end() const {
        return this->m_records.end();
    }

    bool Equil::operator==(const Equil& data) const {
        return this->m_records == data.m_records;
    }
}
