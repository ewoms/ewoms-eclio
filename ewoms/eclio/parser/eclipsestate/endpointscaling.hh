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
#ifndef EWOMS_ENDPOINTSCALING_H
#define EWOMS_ENDPOINTSCALING_H

#include <bitset>

namespace Ewoms {
class Deck;

class EndpointScaling {
    public:
        EndpointScaling() noexcept = default;
        explicit EndpointScaling( const Deck& );

        static EndpointScaling serializeObject();

        /* true if endpoint scaling is enabled, otherwise false */
        operator bool() const noexcept;

        bool directional() const noexcept;
        bool nondirectional() const noexcept;
        bool reversible() const noexcept;
        bool irreversible() const noexcept;
        bool twopoint() const noexcept;
        bool threepoint() const noexcept;

        bool operator==(const EndpointScaling& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            if (serializer.isSerializing())
                serializer(options.to_ulong());
            else {
                unsigned long bits = 0;
                serializer(bits);
                options = std::bitset<4>(bits);
            }
        }

    private:
        enum class option {
            any         = 0,
            directional = 1,
            reversible  = 2,
            threepoint  = 3,
        };

        using ue = std::underlying_type< option >::type;
        std::bitset< 4 > options;
};
}

#endif
