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

#ifndef EWOMS_TRACER_CONFIG_H
#define EWOMS_TRACER_CONFIG_H

#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tracervdtable.hh>

namespace Ewoms {

class Deck;
class UnitSystem;

class TracerConfig {
public:
    struct TracerEntry {
        std::string name;
        Phase phase = Phase::OIL;
        std::vector<double> concentration;
        TracerVdTable tvdpf;

        TracerEntry() = default;
        TracerEntry(const std::string& name_, Phase phase_, std::vector<double> concentration_)
            : name(name_)
            , phase(phase_)
            , concentration(std::move(concentration_))
        {}

        TracerEntry(const std::string& name_, Phase phase_, TracerVdTable tvdpf_)
            : name(name_)
            , phase(phase_)
            , tvdpf(std::move(tvdpf_))
        {}

        bool operator==(const TracerEntry& data) const {
            return this->name == data.name &&
                   this->phase == data.phase &&
                   this->concentration == data.concentration &&
                   this->tvdpf == data.tvdpf;
        }

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(name);
            serializer(phase);
            serializer(concentration);
            tvdpf.serializeOp(serializer);
        }
    };

    TracerConfig() = default;
    TracerConfig(const UnitSystem& unit_system, const Deck& deck);

    static TracerConfig serializeObject();

    size_t size() const;

    const std::vector<TracerEntry>::const_iterator begin() const;
    const std::vector<TracerEntry>::const_iterator end() const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer.vector(tracers);
    }

    bool operator==(const TracerConfig& data) const;

private:
    std::vector<TracerEntry> tracers;
};

}

#endif
