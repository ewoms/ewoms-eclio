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

namespace Ewoms {
    class EclipseGrid;
    class Eclipse3DProperties;
    class FieldPropsManager;
    class WellConnections {
    public:

        WellConnections();
        WellConnections(int headI, int headJ);
        WellConnections(int headI, int headJ,
                        size_t numRemoved,
                        const std::vector<Connection>& connections);

        // cppcheck-suppress noExplicitConstructor
        WellConnections(const WellConnections& src, const EclipseGrid& grid);
        void addConnection(int i, int j , int k ,
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
                           const double segDistStart= 0.0,
                           const double segDistEnd= 0.0,
                           const bool defaultSatTabId = true);
        void loadCOMPDAT(const DeckRecord& record, const EclipseGrid& grid, const Eclipse3DProperties& eclipseProperties);
        void loadCOMPDAT(const DeckRecord& record, const EclipseGrid& grid, const FieldPropsManager& field_properties);

        using const_iterator = std::vector< Connection >::const_iterator;

        void add( Connection );
        size_t size() const;
        size_t inputSize() const;
        const Connection& operator[](size_t index) const;
        const Connection& get(size_t index) const;
        const Connection& getFromIJK(const int i, const int j, const int k) const;
        Connection& getFromIJK(const int i, const int j, const int k);

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
        void orderConnections(size_t well_i, size_t well_j);

        bool operator==( const WellConnections& ) const;
        bool operator!=( const WellConnections& ) const;

        int getHeadI() const;
        int getHeadJ() const;
        size_t getNumRemoved() const;
        const std::vector<Connection>& getConnections() const;

    private:
        void addConnection(int i, int j , int k ,
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
                           const double segDistStart= 0.0,
                           const double segDistEnd= 0.0,
                           const bool defaultSatTabId = true);

        void loadCOMPDAT(const DeckRecord& record,
                         const EclipseGrid& grid,
                         const std::vector<int>& satnum_data,
                         const std::vector<double>* permx,
                         const std::vector<double>* permy,
                         const std::vector<double>* permz,
                         const std::vector<double>& ntg);

        size_t findClosestConnection(int oi, int oj, double oz, size_t start_pos);

        int headI, headJ;
        size_t num_removed = 0;
        std::vector< Connection > m_connections;
    };
}

#endif
