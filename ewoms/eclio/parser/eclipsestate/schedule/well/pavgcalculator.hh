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
#ifndef PAVE_CALC_H
#define PAVE_CALC_H

#include <functional>
#include <map>
#include <ewoms/common/optional.hh>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/well/pavg.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/connection.hh>

namespace Ewoms {

class WellConnections;
class EclipseGrid;
class Serializer;
class PAvgCalculator {
public:

    PAvgCalculator(const std::string& well, const EclipseGrid& grid, const WellConnections& connections, const PAvg& pavg);

    enum class WBPMode {
        WBP,
        WBP4,
        WBP5,
        WBP9
    };

    struct Neighbour {
        Neighbour(double porv_arg, std::size_t index_arg) :
            porv(porv_arg),
            global_index(index_arg)
        {}

        double porv;
        std::size_t global_index;
    };

    struct Connection {
        Connection(double porv_arg, double cf, ::Ewoms::Connection::Direction dir_arg, std::size_t index_arg) :
            porv(porv_arg),
            cfactor(cf),
            dir(dir_arg),
            global_index(index_arg)
        {
        }

        double porv;
        double cfactor;
        ::Ewoms::Connection::Direction dir;
        std::size_t global_index;
        std::vector<Neighbour> rect_neighbours;
        std::vector<Neighbour> diag_neighbours;
    };

    const std::string& wname() const;
    double wbp() const;
    double wbp4() const;
    double wbp5() const;
    double wbp9() const;
    bool add_pressure(std::size_t global_index, double pressure);
    void update(Serializer& serializer);
    const std::vector< std::size_t >& index_list() const;
    std::pair< std::reference_wrapper<const std::vector<double>>, std::reference_wrapper<const std::vector<bool>> > data() const;
    void serialize(Serializer& serializer) const;

private:
    void update(const std::vector<double>& p, const std::vector<char>& m);
    void add_connection(const PAvgCalculator::Connection& conn);
    void add_neighbour(std::size_t global_index, Ewoms::optional<PAvgCalculator::Neighbour> neighbour, bool rect_neighbour);
    double get_pressure(std::size_t global_index) const;
    double connection_pressure(const std::vector<Ewoms::optional<double>>& block_pressure) const;
    double wbp(WBPMode mode) const;

    std::string well_name;
    PAvg m_pavg;
    std::vector<Connection> m_connections;
    std::map<std::size_t, std::size_t> m_index_map;
    std::vector<std::size_t> m_index_list;
    std::vector<double> pressure;
    std::vector<char> valid_pressure;
};

}
#endif
