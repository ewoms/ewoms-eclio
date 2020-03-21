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

#ifndef EWOMS_ROCK_CONFIG_H
#define EWOMS_ROCK_CONFIG_H

#include <string>
#include <ewoms/eclio/parser/parserkeywords/r.hh>

namespace Ewoms {

class Deck;
class FieldPropsManager;

class RockConfig {
public:

enum class Hysteresis {
    REVERS = 1,
    IRREVERS = 2,
    HYSTER = 3,
    BOBERG = 4,
    REVLIMIT = 5,
    PALM_MAN = 6,
    NONE = 7
};

struct RockComp {
    double pref;
    double compressibility;

    RockComp() = default;
    RockComp(double pref_arg, double comp_arg);
    bool operator==(const RockComp& other) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(pref);
        serializer(compressibility);
    }
};

    RockConfig() = default;
    RockConfig(const Deck& deck, const FieldPropsManager& fp);
    RockConfig(bool active, const std::vector<RockComp>& comp, const std::string& num_prop, std::size_t num_rock_tables, bool water_compaction, Hysteresis hyst);

    bool active() const;
    const std::vector<RockConfig::RockComp>& comp() const;
    const std::string& rocknum_property() const;
    std::size_t num_rock_tables() const;
    Hysteresis hysteresis_mode() const;
    bool water_compaction() const;

    bool operator==(const RockConfig& other) const;

    template<class Serializer>
    void serializeOp(Serializer& serializer)
    {
        serializer(m_active);
        serializer.vector(m_comp);
        serializer(num_property);
        serializer(num_tables);
        serializer(m_water_compaction);
        serializer(hyst_mode);
    }

private:
    bool m_active = false;
    std::vector<RockComp> m_comp;
    std::string num_property = ParserKeywords::ROCKOPTS::TABLE_TYPE::defaultValue;
    std::size_t num_tables = ParserKeywords::ROCKCOMP::NTROCC::defaultValue;
    bool m_water_compaction;
    Hysteresis hyst_mode;
};

} //namespace Ewoms

#endif
