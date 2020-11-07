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
#ifndef CONNECTIONSET_HH_
#define CONNECTIONSET_HH_

#include <ewoms/eclio/parser/eclipsestate/schedule/well/connection.hh>

#include <ewoms/eclio/utility/activegridcells.hh>

#include <cstddef>
#include <vector>

#include <stddef.h>

namespace Ewoms {
    class DeckRecord;
    class EclipseGrid;
    class FieldPropsManager;
    class WellConnections {
    public:

        WellConnections();
        WellConnections(Connection::Order ordering, int headI, int headJ);
        WellConnections(Connection::Order ordering, int headI, int headJ,
                        const std::vector<Connection>& connections);

        static WellConnections serializeObject();

        // cppcheck-suppress noExplicitConstructor
        WellConnections(const WellConnections& src, const EclipseGrid& grid);
        void addConnection(int i, int j , int k ,
                           std::size_t global_index,
                           double depth,
                           Connection::State state ,
                           double CF,
                           double Kh,
                           double rw,
                           double r0,
                           double skin_factor,
                           const int satTableId,
                           const Connection::Direction direction = Connection::Direction::Z,
                           const Connection::CTFKind ctf_kind = Connection::CTFKind::DeckValue,
                           const std::size_t seqIndex = 0,
                           const bool defaultSatTabId = true);
        void loadCOMPDAT(const DeckRecord& record, const EclipseGrid& grid, const FieldPropsManager& field_properties);

        using const_iterator = std::vector< Connection >::const_iterator;

        void add( Connection );
        size_t size() const;
        bool empty() const;
        const Connection& operator[](size_t index) const;
        const Connection& get(size_t index) const;
        const Connection& getFromIJK(const int i, const int j, const int k) const;
        const Connection& getFromGlobalIndex(std::size_t global_index) const;
        const Connection& lowest() const;
        Connection& getFromIJK(const int i, const int j, const int k);
        bool hasGlobalIndex(std::size_t global_index) const;
        double segment_perf_length(int segment) const;

        const_iterator begin() const { return this->m_connections.begin(); }
        const_iterator end() const { return this->m_connections.end(); }
        void filter(const ActiveGridCells& grid);
        bool allConnectionsShut() const;
        /// Order connections irrespective of input order.
        /// The algorithm used is the following:
        ///     1. The connection nearest to the given (well_i, well_j)
        ///        coordinates in terms of the connection's (i, j) is chosen
        ///        to be the first connection. If non-unique, choose one with
        ///        lowest z-depth (shallowest).
        ///     2. Choose next connection to be nearest to current in (i, j) sense.
        ///        If non-unique choose closest in z-depth (not logical cartesian k).
        ///
        /// \param[in] well_i  logical cartesian i-coordinate of well head
        /// \param[in] well_j  logical cartesian j-coordinate of well head
        /// \param[in] grid    EclipseGrid object, used for cell depths
        void order();

        bool operator==( const WellConnections& ) const;
        bool operator!=( const WellConnections& ) const;

        Connection::Order ordering() const { return this->m_ordering; }
        std::vector<const Connection *> output(const EclipseGrid& grid) const;

        /// Activate or reactivate WELPI scaling for this connection set.
        ///
        /// Following this call, any WELPI-based scaling will apply to all
        /// connections whose properties are not reset in COMPDAT.
        ///
        /// Returns whether or not this call to prepareWellPIScaling() is
        /// a state change (e.g., no WELPI to active WELPI or WELPI for
        /// some connections to WELPI for all connections).
        bool prepareWellPIScaling();

        /// Scale pertinent connections' CF value by supplied value.  Scaling
        /// factor typically derived from 'WELPI' input keyword and a dynamic
        /// productivity index calculation.  Applicability array specifies
        /// whether or not a particular connection is exempt from scaling.
        /// Empty array means "apply scaling to all eligible connections".
        /// This array is updated on return (entries set to 'false' if
        /// corresponding connection is not eligible).
        void applyWellPIScaling(const double       scaleFactor,
                                std::vector<bool>& scalingApplicable);

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_ordering);
            serializer(headI);
            serializer(headJ);
            serializer.vector(m_connections);
        }

    private:
        void addConnection(int i, int j , int k ,
                           std::size_t global_index,
                           int complnum,
                           double depth,
                           Connection::State state ,
                           double CF,
                           double Kh,
                           double rw,
                           double r0,
                           double skin_factor,
                           const int satTableId,
                           const Connection::Direction direction = Connection::Direction::Z,
                           const Connection::CTFKind ctf_kind = Connection::CTFKind::DeckValue,
                           const std::size_t seqIndex = 0,
                           const bool defaultSatTabId = true);

        void loadCOMPDAT(const DeckRecord& record,
                         const EclipseGrid& grid,
                         const std::vector<int>& satnum_data,
                         const std::vector<double>* permx,
                         const std::vector<double>* permy,
                         const std::vector<double>* permz,
                         const std::vector<double>& ntg);

        size_t findClosestConnection(int oi, int oj, double oz, size_t start_pos);
        void orderTRACK();
        void orderMSW();

        Connection::Order m_ordering = Connection::Order::TRACK;
        int headI, headJ;
        std::vector< Connection > m_connections;
    };
}

#endif
