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

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/simulationconfig/simulationconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/simulationconfig/thresholdpressure.hh>
#include <ewoms/eclio/parser/parserkeywords/c.hh>
#include <ewoms/eclio/parser/parserkeywords/d.hh>
#include <ewoms/eclio/parser/parserkeywords/t.hh>
#include <ewoms/eclio/parser/parserkeywords/v.hh>

/*
  The internalization of the CPR keyword has been temporarily
  disabled, suddenly decks with 'CPR' in the summary section turned
  up. Keywords with section aware keyword semantics is currently not
  handled by the parser.

  When the CPR is added again the following keyword configuration must
  be added:

    {"name" : "CPR" , "sections" : ["RUNSPEC"], "size": 1 }

*/

namespace Ewoms {

    SimulationConfig::SimulationConfig() :
        m_useCPR(false),
        m_DISGAS(false),
        m_VAPOIL(false),
        m_isThermal(false)
    {
    }

    SimulationConfig::SimulationConfig(bool restart,
                                       const Deck& deck,
                                       const FieldPropsManager& fp) :
        m_ThresholdPressure( restart, deck, fp),
        m_bcconfig(deck),
        m_rock_config(deck, fp),
        m_useCPR(false),
        m_DISGAS(false),
        m_VAPOIL(false),
        m_isThermal(false)
    {
        if (DeckSection::hasRUNSPEC(deck)) {
            const RUNSPECSection runspec(deck);
            if (runspec.hasKeyword<ParserKeywords::CPR>()) {
                const auto& cpr = runspec.getKeyword<ParserKeywords::CPR>();
                if (cpr.size() > 0)
                    throw std::invalid_argument("ERROR: In the RUNSPEC section the CPR keyword should EXACTLY one empty record.");

                m_useCPR = true;
            }
            if (runspec.hasKeyword<ParserKeywords::DISGAS>()) {
                m_DISGAS = true;
            }
            if (runspec.hasKeyword<ParserKeywords::VAPOIL>()) {
                m_VAPOIL = true;
            }

            this->m_isThermal = runspec.hasKeyword<ParserKeywords::THERMAL>()
                || runspec.hasKeyword<ParserKeywords::TEMP>();
        }
    }

    SimulationConfig SimulationConfig::serializeObject()
    {
        SimulationConfig result;
        result.m_ThresholdPressure = ThresholdPressure::serializeObject();
        result.m_bcconfig = BCConfig::serializeObject();
        result.m_rock_config = RockConfig::serializeObject();
        result.m_useCPR = false;
        result.m_DISGAS = true;
        result.m_VAPOIL = false;
        result.m_isThermal = true;

        return result;
    }

    const ThresholdPressure& SimulationConfig::getThresholdPressure() const {
        return m_ThresholdPressure;
    }

    const BCConfig& SimulationConfig::bcconfig() const {
        return m_bcconfig;
    }

    const RockConfig& SimulationConfig::rock_config() const {
        return this->m_rock_config;
    }

    bool SimulationConfig::useThresholdPressure() const {
        return m_ThresholdPressure.active();
    }

    bool SimulationConfig::useCPR() const {
        return m_useCPR;
    }

    bool SimulationConfig::hasDISGAS() const {
        return m_DISGAS;
    }

    bool SimulationConfig::hasVAPOIL() const {
        return m_VAPOIL;
    }

    bool SimulationConfig::isThermal() const {
        return this->m_isThermal;
    }

    bool SimulationConfig::operator==(const SimulationConfig& data) const {
        return this->getThresholdPressure() == data.getThresholdPressure() &&
               this->bcconfig() == data.bcconfig() &&
               this->rock_config() == data.rock_config() &&
               this->useCPR() == data.useCPR() &&
               this->hasDISGAS() == data.hasDISGAS() &&
               this->hasVAPOIL() == data.hasVAPOIL() &&
               this->isThermal() == data.isThermal();
    }

} //namespace Ewoms
