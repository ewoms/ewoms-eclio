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

#ifndef TLMIXPAR_H
#define TLMIXPAR_H

#include <cstddef>
#include <vector>

namespace Ewoms {
class Deck;

struct TLMixRecord {
    double viscosity_parameter;
    double density_parameter;

    TLMixRecord() = default;
    TLMixRecord(double v, double d) :
        viscosity_parameter(v),
        density_parameter(d)
    {};

    bool operator==(const TLMixRecord& other) const {
        return this->viscosity_parameter == other.viscosity_parameter &&
               this->density_parameter == other.density_parameter;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(viscosity_parameter);
        serializer(density_parameter);
    }

};

class TLMixpar {
public:

    TLMixpar() = default;
    TLMixpar(const Deck& deck);
    static TLMixpar serializeObject();
    std::size_t size() const;
    bool empty() const;
    const TLMixRecord& operator[](const std::size_t index) const;

    bool operator==(const TLMixpar& other) const {
        return this->data == other.data;
    }

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.vector(data);
    }

private:
    std::vector<TLMixRecord> data;
};
}

#endif
