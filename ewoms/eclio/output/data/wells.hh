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
#ifndef EWOMS_OUTPUT_WELLS_H
#define EWOMS_OUTPUT_WELLS_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <ewoms/eclio/output/data/guideratevalue.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>

namespace Ewoms {

    namespace data {

    class Rates {
        /* Methods are defined inline for performance, as the actual *work* done
         * is trivial, but somewhat frequent (typically once per time step per
         * completion per well).
         *
         * To add a new rate type, add an entry in the enum with the correct
         * shift, and if needed, increase the size type. Add a member variable
         * and a new case in get_ref.
         */

        public:
            Rates() = default;
            enum class opt : uint32_t {
                wat               = (1 << 0),
                oil               = (1 << 1),
                gas               = (1 << 2),
                polymer           = (1 << 3),
                solvent           = (1 << 4),
                energy            = (1 << 5),
                dissolved_gas     = (1 << 6),
                vaporized_oil     = (1 << 7),
                reservoir_water   = (1 << 8),
                reservoir_oil     = (1 << 9),
                reservoir_gas     = (1 << 10),
                productivity_index_water = (1 << 11),
                productivity_index_oil   = (1 << 12),
                productivity_index_gas   = (1 << 13),
                well_potential_water   = (1 << 14),
                well_potential_oil     = (1 << 15),
                well_potential_gas     = (1 << 16),
                brine            = (1 << 17),
                alq              = (1 << 18)
            };

            using enum_size = std::underlying_type< opt >::type;

            /// Query if a value is set.
            inline bool has( opt ) const;

            /// Read the value indicated by m. Throws an exception if
            /// if the requested value is unset.
            inline double get( opt m ) const;
            /// Read the value indicated by m. Returns a default value if
            /// the requested value is unset.
            inline double get( opt m, double default_value ) const;
            /// Set the value specified by m. Throws an exception if multiple
            /// values are requested. Returns a self-reference to support
            /// chaining.
            inline Rates& set( opt m, double value );

            /// Returns true if any of the rates oil, gas, water is nonzero
            inline bool flowing() const;

            template <class MessageBufferType>
            void write(MessageBufferType& buffer) const;
            template <class MessageBufferType>
            void read(MessageBufferType& buffer);

            bool operator==(const Rates& rat2) const;

        private:
            double& get_ref( opt );
            const double& get_ref( opt ) const;

            opt mask = static_cast< opt >( 0 );

            double wat = 0.0;
            double oil = 0.0;
            double gas = 0.0;
            double polymer = 0.0;
            double solvent = 0.0;
            double energy = 0.0;
            double dissolved_gas = 0.0;
            double vaporized_oil = 0.0;
            double reservoir_water = 0.0;
            double reservoir_oil = 0.0;
            double reservoir_gas = 0.0;
            double productivity_index_water = 0.0;
            double productivity_index_oil = 0.0;
            double productivity_index_gas = 0.0;
            double well_potential_water = 0.0;
            double well_potential_oil = 0.0;
            double well_potential_gas = 0.0;
            double brine = 0.0;
            double alq = 0.0;
    };

    struct Connection {
        using global_index = size_t;
        static const constexpr int restart_size = 6;

        global_index index;
        Rates rates;
        double pressure;
        double reservoir_rate;
        double cell_pressure;
        double cell_saturation_water;
        double cell_saturation_gas;
        double effective_Kh;
        double trans_factor;

        bool operator==(const Connection& conn2) const
        {
            return index == conn2.index &&
                   rates == conn2.rates &&
                   pressure == conn2.pressure &&
                   reservoir_rate == conn2.reservoir_rate &&
                   cell_pressure == conn2.cell_pressure &&
                   cell_saturation_water == conn2.cell_saturation_water &&
                   cell_saturation_gas == conn2.cell_saturation_gas &&
                   effective_Kh == conn2.effective_Kh;
        }

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const;
        template <class MessageBufferType>
        void read(MessageBufferType& buffer);
    };

    class SegmentPressures {
    public:
        enum class Value : std::size_t {
            Pressure, PDrop, PDropHydrostatic, PDropAccel, PDropFriction,
        };

        double& operator[](const Value i)
        {
            return this->values_[this->index(i)];
        }

        double operator[](const Value i) const
        {
            return this->values_[this->index(i)];
        }

        bool operator==(const SegmentPressures& segpres2) const
        {
            return this->values_ == segpres2.values_;
        }

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const
        {
            for (const auto& value : this->values_) {
                buffer.write(value);
            }
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer)
        {
            for (auto& value : this->values_) {
                buffer.read(value);
            }
        }

    private:
        constexpr static std::size_t numvals = 5;

        std::array<double, numvals> values_;

        std::size_t index(const Value ix) const
        {
            return static_cast<std::size_t>(ix);
        }
    };

    struct Segment {
        Rates rates;
        SegmentPressures pressures;
        std::size_t segNumber;

        bool operator==(const Segment& seg2) const
        {
          return rates == seg2.rates &&
                 pressures == seg2.pressures &&
                 segNumber == seg2.segNumber;
        }

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const;

        template <class MessageBufferType>
        void read(MessageBufferType& buffer);
    };

    struct CurrentControl {
        bool isProducer{true};

        ::Ewoms::Well::ProducerCMode prod {
            ::Ewoms::Well::ProducerCMode::CMODE_UNDEFINED
        };

        ::Ewoms::Well::InjectorCMode inj {
            ::Ewoms::Well::InjectorCMode::CMODE_UNDEFINED
        };

        bool operator==(const CurrentControl& rhs) const
        {
            return (this->isProducer == rhs.isProducer)
                && ((this->isProducer && (this->prod == rhs.prod)) ||
                    (!this->isProducer && (this->inj == rhs.inj)));
        }

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const;

        template <class MessageBufferType>
        void read(MessageBufferType& buffer);
    };

    struct Well {
        Rates rates;
        double bhp;
        double thp;
        double temperature;
        int control;
        std::vector< Connection > connections;
        std::unordered_map<std::size_t, Segment> segments;
        CurrentControl current_control;
        GuideRateValue guide_rates{};

        inline bool flowing() const noexcept;
        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const;
        template <class MessageBufferType>
        void read(MessageBufferType& buffer);

        const Connection* find_connection(Connection::global_index connection_grid_index) const {
            const auto connection = std::find_if( this->connections.begin() ,
                                                  this->connections.end() ,
                                                  [=]( const Connection& c ) {
                                                      return c.index == connection_grid_index; });

            if( connection == this->connections.end() )
                return nullptr;

            return &*connection;
        }

        Connection* find_connection(Connection::global_index connection_grid_index) {
            auto connection = std::find_if( this->connections.begin() ,
                                            this->connections.end() ,
                                            [=]( const Connection& c ) {
                                                return c.index == connection_grid_index; });

            if( connection == this->connections.end() )
                return nullptr;

            return &*connection;
        }

        bool operator==(const Well& well2) const
        {
          return rates == well2.rates &&
                 bhp == well2.bhp &&
                 thp == well2.thp &&
                 temperature == well2.temperature &&
                 control == well2.control &&
                 connections == well2.connections &&
                 segments == well2.segments &&
                 current_control == well2.current_control &&
                 guide_rates == well2.guide_rates;
        }
    };

    class WellRates : public std::map<std::string , Well> {
    public:

        double get(const std::string& well_name , Rates::opt m) const {
            const auto& well = this->find( well_name );
            if( well == this->end() ) return 0.0;

            return well->second.rates.get( m, 0.0 );
        }

        double get(const std::string& well_name , Connection::global_index connection_grid_index, Rates::opt m) const {
            const auto& witr = this->find( well_name );
            if( witr == this->end() ) return 0.0;

            const auto& well = witr->second;
            const auto& connection = std::find_if( well.connections.begin() ,
                                                   well.connections.end() ,
                                                   [=]( const Connection& c ) {
                                                        return c.index == connection_grid_index; });

            if( connection == well.connections.end() )
                return 0.0;

            return connection->rates.get( m, 0.0 );
        }

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const {
            unsigned int size = this->size();
            buffer.write(size);
            for (const auto& witr : *this) {
                const std::string& name = witr.first;
                buffer.write(name);
                const Well& well = witr.second;
                well.write(buffer);
            }
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer) {
            unsigned int size;
            buffer.read(size);
            for (size_t i = 0; i < size; ++i) {
                std::string name;
                buffer.read(name);
                Well well;
                well.read(buffer);
                this->emplace(name, well);
            }
        }
    };

    using Wells = WellRates;

    /* IMPLEMENTATIONS */

    inline bool Rates::has( opt m ) const {
        const auto mand = static_cast< enum_size >( this->mask )
                        & static_cast< enum_size >( m );

        return static_cast< opt >( mand ) == m;
    }

    inline double Rates::get( opt m ) const {
        if( !this->has( m ) )
            throw std::invalid_argument( "Uninitialized value." );

        return this->get_ref( m );
    }

    inline double Rates::get( opt m, double default_value ) const {
        if( !this->has( m ) ) return default_value;

        return this->get_ref( m );
    }

    inline Rates& Rates::set( opt m, double value ) {
        this->get_ref( m ) = value;
        /* mask |= m */
        this->mask = static_cast< opt >(
                        static_cast< enum_size >( this->mask ) |
                        static_cast< enum_size >( m )
                    );

        return *this;
    }

    inline bool Rates::operator==(const Rates& rate) const
    {
      return mask == rate.mask &&
             wat == rate.wat &&
             oil == rate.oil &&
             gas == rate.gas &&
             polymer == rate.polymer &&
             solvent == rate.solvent &&
             energy == rate.energy &&
             dissolved_gas == rate.dissolved_gas &&
             vaporized_oil == rate.vaporized_oil &&
             reservoir_water == rate.reservoir_water &&
             reservoir_oil == rate.reservoir_oil &&
             reservoir_gas == rate.reservoir_gas &&
             productivity_index_water == rate.productivity_index_water &&
             productivity_index_gas == rate.productivity_index_gas &&
             productivity_index_oil == rate.productivity_index_oil &&
             well_potential_water == rate.well_potential_water &&
             well_potential_oil == rate.well_potential_oil &&
             well_potential_gas == rate.well_potential_gas &&
             brine == rate.brine &&
             alq == rate.alq;
    }

    /*
     * To avoid error-prone and repetitve work when extending rates with new
     * values, the get+set methods use this helper get_ref to determine what
     * member to manipulate. To add a new option, just add another case
     * corresponding to the enum entry in Rates to this function.
     *
     * This is an implementation detail and understanding this has no
     * significant impact on correct use of the class.
     */
    inline const double& Rates::get_ref( opt m ) const {
        switch( m ) {
            case opt::wat: return this->wat;
            case opt::oil: return this->oil;
            case opt::gas: return this->gas;
            case opt::polymer: return this->polymer;
            case opt::solvent: return this->solvent;
            case opt::energy: return this->energy;
            case opt::dissolved_gas: return this->dissolved_gas;
            case opt::vaporized_oil: return this->vaporized_oil;
            case opt::reservoir_water: return this->reservoir_water;
            case opt::reservoir_oil: return this->reservoir_oil;
            case opt::reservoir_gas: return this->reservoir_gas;
            case opt::productivity_index_water: return this->productivity_index_water;
            case opt::productivity_index_oil: return this->productivity_index_oil;
            case opt::productivity_index_gas: return this->productivity_index_gas;
            case opt::well_potential_water: return this->well_potential_water;
            case opt::well_potential_oil: return this->well_potential_oil;
            case opt::well_potential_gas: return this->well_potential_gas;
            case opt::brine: return this->brine;
            case opt::alq: return this->alq;
        }

        throw std::invalid_argument(
                "Unknown value type '"
                + std::to_string( static_cast< enum_size >( m ) )
                + "'" );

    }

    inline double& Rates::get_ref( opt m ) {
        return const_cast< double& >(
                static_cast< const Rates* >( this )->get_ref( m )
                );
    }

    bool inline Rates::flowing() const {
        return ((this->wat != 0) ||
                (this->oil != 0) ||
                (this->gas != 0));
    }

    inline bool Well::flowing() const noexcept {
        return this->rates.flowing();
    }

    template <class MessageBufferType>
    void Rates::write(MessageBufferType& buffer) const {
            buffer.write(this->mask);
            buffer.write(this->wat);
            buffer.write(this->oil);
            buffer.write(this->gas);
            buffer.write(this->polymer);
            buffer.write(this->solvent);
            buffer.write(this->energy);
            buffer.write(this->dissolved_gas);
            buffer.write(this->vaporized_oil);
            buffer.write(this->reservoir_water);
            buffer.write(this->reservoir_oil);
            buffer.write(this->reservoir_gas);
            buffer.write(this->productivity_index_water);
            buffer.write(this->productivity_index_oil);
            buffer.write(this->productivity_index_gas);
            buffer.write(this->well_potential_water);
            buffer.write(this->well_potential_oil);
            buffer.write(this->well_potential_gas);
            buffer.write(this->brine);
            buffer.write(this->alq);
    }

    template <class MessageBufferType>
    void Connection::write(MessageBufferType& buffer) const {
            buffer.write(this->index);
            this->rates.write(buffer);
            buffer.write(this->pressure);
            buffer.write(this->reservoir_rate);
            buffer.write(this->cell_pressure);
            buffer.write(this->cell_saturation_water);
            buffer.write(this->cell_saturation_gas);
            buffer.write(this->effective_Kh);
            buffer.write(this->trans_factor);
    }

    template <class MessageBufferType>
    void Segment::write(MessageBufferType& buffer) const {
        buffer.write(this->segNumber);
        this->rates.write(buffer);
        this->pressures.write(buffer);
    }

    template <class MessageBufferType>
    void CurrentControl::write(MessageBufferType& buffer) const
    {
        buffer.write(this->isProducer);
        if (this->isProducer) {
            buffer.write(this->prod);
        }
        else {
            buffer.write(this->inj);
        }
    }

    template <class MessageBufferType>
    void Well::write(MessageBufferType& buffer) const {
        this->rates.write(buffer);
        buffer.write(this->bhp);
        buffer.write(this->thp);
        buffer.write(this->temperature);
        buffer.write(this->control);
        unsigned int size = this->connections.size();
        buffer.write(size);
        for (const Connection& comp : this->connections)
            comp.write(buffer);

        {
            const auto nSeg =
                static_cast<unsigned int>(this->segments.size());
            buffer.write(nSeg);

            for (const auto& seg : this->segments) {
                seg.second.write(buffer);
            }
        }

        this->current_control.write(buffer);
        this->guide_rates.write(buffer);
    }

    template <class MessageBufferType>
    void Rates::read(MessageBufferType& buffer) {
            buffer.read(this->mask);
            buffer.read(this->wat);
            buffer.read(this->oil);
            buffer.read(this->gas);
            buffer.read(this->polymer);
            buffer.read(this->solvent);
            buffer.read(this->energy);
            buffer.read(this->dissolved_gas);
            buffer.read(this->vaporized_oil);
            buffer.read(this->reservoir_water);
            buffer.read(this->reservoir_oil);
            buffer.read(this->reservoir_gas);
            buffer.read(this->productivity_index_water);
            buffer.read(this->productivity_index_oil);
            buffer.read(this->productivity_index_gas);
            buffer.read(this->well_potential_water);
            buffer.read(this->well_potential_oil);
            buffer.read(this->well_potential_gas);
            buffer.read(this->brine);
            buffer.read(this->alq);
    }

  template <class MessageBufferType>
   void Connection::read(MessageBufferType& buffer) {
            buffer.read(this->index);
            this->rates.read(buffer);
            buffer.read(this->pressure);
            buffer.read(this->reservoir_rate);
            buffer.read(this->cell_pressure);
            buffer.read(this->cell_saturation_water);
            buffer.read(this->cell_saturation_gas);
            buffer.read(this->effective_Kh);
            buffer.read(this->trans_factor);
   }

    template <class MessageBufferType>
    void Segment::read(MessageBufferType& buffer) {
        buffer.read(this->segNumber);
        this->rates.read(buffer);
        this->pressures.read(buffer);
    }

    template <class MessageBufferType>
    void CurrentControl::read(MessageBufferType& buffer)
    {
        buffer.read(this->isProducer);
        if (this->isProducer) {
            buffer.read(this->prod);
        }
        else {
            buffer.read(this->inj);
        }
    }

    template <class MessageBufferType>
    void Well::read(MessageBufferType& buffer) {
        this->rates.read(buffer);
        buffer.read(this->bhp);
        buffer.read(this->thp);
        buffer.read(this->temperature);
        buffer.read(this->control);

        // Connection information
        unsigned int size = 0.0; //this->connections.size();
        buffer.read(size);
        this->connections.resize(size);
        for (size_t i = 0;  i < size; ++i)
        {
            auto& comp = this->connections[ i ];
            comp.read(buffer);
        }

        // Segment information (if applicable)
        const auto nSeg = [&buffer]() -> unsigned int
        {
            auto n = 0u;
            buffer.read(n);

            return n;
        }();

        for (auto segID = 0*nSeg; segID < nSeg; ++segID) {
            auto seg = Segment{};
            seg.read(buffer);

            const auto segNumber = seg.segNumber;
            this->segments.emplace(segNumber, std::move(seg));
        }

        this->current_control.read(buffer);
        this->guide_rates.read(buffer);
    }

}} // Ewoms::data

#endif //EWOMS_OUTPUT_WELLS_H
