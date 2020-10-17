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
#include "config.h"

#include <ewoms/eclio/utility/opminputerror.hh>
#include <ewoms/eclio/opmlog/opmlog.hh>
#include <ewoms/eclio/opmlog/infologger.hh>
#include <ewoms/eclio/parser/parserkeywords/t.hh>
#include <ewoms/eclio/parser/eclipsestate/tracerconfig.hh>
#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/units/dimension.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/units/units.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tracervdtable.hh>

namespace Ewoms {

namespace {

Phase phase_from_string(const std::string& phase_string) {
    if (phase_string == "WAT")
        return Phase::WATER;

    if (phase_string == "OIL")
        return Phase::OIL;

    if (phase_string == "GAS")
        return Phase::GAS;

    throw std::invalid_argument("Tracer: invalid fluid name " + phase_string);
}

}

TracerConfig::TracerConfig(const UnitSystem& unit_system, const Deck& deck)
{
    using TR = ParserKeywords::TRACER;
    if (deck.hasKeyword<TR>()) {
        const auto& keyword = deck.getKeyword<TR>();
        OpmLog::info( keyword.location().format("Initializing tracers from {keyword} in {file} line {line}") );
        InfoLogger logger("Tracer tables", 3);
        for (const auto& record : keyword) {
            const auto& name = record.getItem<TR::NAME>().get<std::string>(0);
            Phase phase = phase_from_string(record.getItem<TR::FLUID>().get<std::string>(0));
            double inv_volume;

            if (phase == Phase::GAS)
                inv_volume = unit_system.getDimension(UnitSystem::measure::gas_surface_volume).getSIScaling();
            else
                inv_volume = unit_system.getDimension(UnitSystem::measure::liquid_surface_volume).getSIScaling();

            std::string tracer_field = "TBLKF" + name;
            if (deck.hasKeyword(tracer_field)) {
                const auto& tracer_keyword = deck.getKeyword(tracer_field);
                auto concentration = tracer_keyword.getRecord(0).getItem(0).getData<double>();
                logger(tracer_keyword.location().format("Loading tracer concentration from {keyword} in {file} line {line}"));
                for (auto& c : concentration)
                    c *= inv_volume;

                this->tracers.emplace_back(name, phase, std::move(concentration)) ;
                continue;
            }

            std::string tracer_table = "TVDPF" + name;
            if (deck.hasKeyword(tracer_table)) {
                const auto& tracer_keyword = deck.getKeyword(tracer_table);
                const auto& deck_item = tracer_keyword.getRecord(0).getItem(0);
                logger(tracer_keyword.location().format("Loading tracer concentration from {keyword} in {file} line {line}"));
                this->tracers.emplace_back(name, phase, TracerVdTable(deck_item, inv_volume));
                continue;
            }

            throw std::runtime_error("Uninitialized tracer concentration for tracer " + name);
        }
    }
}

TracerConfig TracerConfig::serializeObject()
{
    TracerConfig result;
    result.tracers = {{"test", Phase::OIL, {1.0}}};

    return result;
}

size_t TracerConfig::size() const {
    return this->tracers.size();
}

const std::vector<TracerConfig::TracerEntry>::const_iterator TracerConfig::begin() const {
    return this->tracers.begin();
}

const std::vector<TracerConfig::TracerEntry>::const_iterator TracerConfig::end() const {
    return this->tracers.end();
}

bool TracerConfig::operator==(const TracerConfig& other) const {
    return this->tracers == other.tracers;
}

}
