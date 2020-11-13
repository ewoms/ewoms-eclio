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
#ifndef PAVE_H
#define PAVE_H

namespace Ewoms {
class DeckRecord;

class PAvg {
public:
    enum class DepthCorrection {
        WELL = 1,
        RES  = 2,
        NONE = 3
    };

    PAvg();
    explicit PAvg(const DeckRecord& record);
    PAvg(double inner_weight, double conn_weight, DepthCorrection depth_correction, bool use_open_connections);

    double inner_weight() const;
    double conn_weight() const;
    bool use_porv() const;
    bool open_connections() const;
    DepthCorrection depth_correction() const;

    template<class Serializer>
    void serializeOp(Serializer& serializer) {
        serializer(m_inner_weight);
        serializer(m_conn_weight);
        serializer(m_depth_correction);
        serializer(m_open_connections);
    }

    static PAvg serializeObject();

    bool operator==(const PAvg& other) const;
    bool operator!=(const PAvg& other) const;

private:
    double m_inner_weight;
    double m_conn_weight;
    DepthCorrection m_depth_correction;
    bool m_open_connections;
};

}
#endif
