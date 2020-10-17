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
#ifndef EWOMS_EQUIL_H
#define EWOMS_EQUIL_H

#include <cstddef>
#include <vector>

namespace Ewoms {
    class DeckKeyword;
    class EquilRecord {
        public:
            double datumDepth() const;
            double datumDepthPressure() const;
            double waterOilContactDepth() const;
            double waterOilContactCapillaryPressure() const;
            double gasOilContactDepth() const;
            double gasOilContactCapillaryPressure() const;

            bool liveOilInitConstantRs() const;
            bool wetGasInitConstantRv() const;
            int initializationTargetAccuracy() const;

            EquilRecord();

            EquilRecord( double datum_depth_arg, double datum_depth_pc_arg, double woc_depth, double woc_pc, double goc_depth, double goc_pc, bool live_oil_init, bool wet_gas_init, int target_accuracy);

            bool operator==(const EquilRecord& data) const;

            template<class Serializer>
            void serializeOp(Serializer& serializer)
            {
                serializer(datum_depth);
                serializer(datum_depth_ps);
                serializer(water_oil_contact_depth);
                serializer(water_oil_contact_capillary_pressure);
                serializer(gas_oil_contact_depth);
                serializer(gas_oil_contact_capillary_pressure);
                serializer(live_oil_init_proc);
                serializer(wet_gas_init_proc);
                serializer(init_target_accuracy);
            }

        private:
            double datum_depth;
            double datum_depth_ps;
            double water_oil_contact_depth;
            double water_oil_contact_capillary_pressure;
            double gas_oil_contact_depth;
            double gas_oil_contact_capillary_pressure;

            bool live_oil_init_proc;
            bool wet_gas_init_proc;
            int init_target_accuracy;
    };

    class Equil {
        public:
            using const_iterator = std::vector< EquilRecord >::const_iterator;

            Equil() = default;
            explicit Equil( const DeckKeyword& );

            static Equil serializeObject();

            const EquilRecord& getRecord( size_t id ) const;

            size_t size() const;
            bool empty() const;

            const_iterator begin() const;
            const_iterator end() const;

            bool operator==(const Equil& data) const;

            template<class Serializer>
            void serializeOp(Serializer& serializer)
            {
                serializer.vector(m_records);
            }

        private:
            std::vector< EquilRecord > m_records;
    };

}

#endif //EWOMS_EQUIL_H
