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

#ifndef COMPLETION_HH_
#define COMPLETION_HH_

#include <array>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/eclipsestate/util/value.hh>

namespace Ewoms {

    class DeckKeyword;
    class DeckRecord;

    class Connection {
    public:
        enum class State {
            OPEN = 1,
            SHUT = 2,
            AUTO = 3
        };

        static const std::string State2String( State enumValue );
        static State StateFromString( const std::string& stringValue );

        enum class Direction{
            X = 1,
            Y = 2,
            Z = 3
        };

        static std::string Direction2String(const Direction enumValue);
        static Direction   DirectionFromString(const std::string& stringValue);

        enum class Order {
                          DEPTH,
                          INPUT,
                          TRACK
        };

        static const std::string Order2String( Order enumValue );
        static Order OrderFromString(const std::string& comporderStringValue);

        Connection(int i, int j , int k ,
                   int complnum,
                   double depth,
                   State state,
                   double CF,
                   double Kh,
                   double rw,
                   double r0,
                   double skin_factor,
                   const int satTableId,
                   const Direction direction,
                   const std::size_t seqIndex,
                   const double segDistStart,
                   const double segDistEnd,
                   const bool defaultSatTabId);

        bool attachedToSegment() const;
        bool sameCoordinate(const int i, const int j, const int k) const;
        int getI() const;
        int getJ() const;
        int getK() const;
        State state() const;
        Direction dir() const;
        double depth() const;
        int satTableId() const;
        int complnum() const;
        int segment() const;
        double CF() const;
        double Kh() const;
        double rw() const;
        double r0() const;
        double skinFactor() const;
        double wellPi() const;

        void setState(State state);
        void setComplnum(int compnum);
        void scaleWellPi(double wellPi);
        void updateSegment(int segment_number, double center_depth, std::size_t seqIndex);
        const std::size_t& getSeqIndex() const;
        const bool& getDefaultSatTabId() const;
        const std::size_t& getCompSegSeqIndex() const;
        void setCompSegSeqIndex(std::size_t index);
        void setDefaultSatTabId(bool id);
        const double& getSegDistStart() const;
        const double& getSegDistEnd() const;
        void setSegDistStart(const double& distStart);
        void setSegDistEnd(const double& distEnd);
        std::string str() const;

        bool operator==( const Connection& ) const;
        bool operator!=( const Connection& ) const;
    private:
        Direction direction;
        double center_depth;
        State open_state;
        int sat_tableId;
        int m_complnum;
        double m_CF;
        double m_Kh;
        double m_rw;
        double m_r0;
        double m_skin_factor;

        std::array<int,3> ijk;
        std::size_t m_seqIndex;
        double m_segDistStart;
        double m_segDistEnd;
        bool m_defaultSatTabId;
        std::size_t m_compSeg_seqIndex=0;

        // related segment number
        // -1 means the completion is not related to segment
        int segment_number = -1;
        double wPi = 1.0;
    };
}

#endif /* COMPLETION_HH_ */

