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

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/deck/deckitem.hh>
#include <ewoms/eclio/parser/deck/deckkeyword.hh>
#include <ewoms/eclio/parser/deck/deckrecord.hh>
#include <ewoms/eclio/parser/eclipsestate/initconfig/foamconfig.hh>
#include <ewoms/eclio/parser/parserkeywords/f.hh>

namespace Ewoms
{

// FoamData member functions.

FoamData::FoamData(const DeckRecord& FOAMFSC_record, const DeckRecord& FOAMROCK_record)
    : reference_surfactant_concentration_(FOAMFSC_record.getItem(0).getSIDouble(0))
    , exponent_(FOAMFSC_record.getItem(1).getSIDouble(0))
    , minimum_surfactant_concentration_(FOAMFSC_record.getItem(2).getSIDouble(0))
    , allow_desorption_(true) // will be overwritten below
    , rock_density_(FOAMROCK_record.getItem(1).getSIDouble(0))
{
    // Check validity of adsorption index and set allow_desorption_ member.
    const int ads_ind = FOAMROCK_record.getItem(0).get<int>(0);
    if (ads_ind < 1 || ads_ind > 2) {
        throw std::runtime_error("Illegal adsorption index in FOAMROCK, must be 1 or 2.");
    }
    allow_desorption_ = (ads_ind == 1);
}

FoamData::FoamData(const DeckRecord& FOAMROCK_record)
    : reference_surfactant_concentration_(0.0)
    , exponent_(0.0)
    , minimum_surfactant_concentration_(1e-20)
    , allow_desorption_(true) // will be overwritten below
    , rock_density_(FOAMROCK_record.getItem(1).getSIDouble(0))
{
    // Check validity of adsorption index and set allow_desorption_ member.
    const int ads_ind = FOAMROCK_record.getItem(0).get<int>(0);
    if (ads_ind < 1 || ads_ind > 2) {
        throw std::runtime_error("Illegal adsorption index in FOAMROCK, must be 1 or 2.");
    }
    allow_desorption_ = (ads_ind == 1);
}

double
FoamData::referenceSurfactantConcentration() const
{
    return this->reference_surfactant_concentration_;
}

double
FoamData::exponent() const
{
    return this->exponent_;
}

double
FoamData::minimumSurfactantConcentration() const
{
    return this->minimum_surfactant_concentration_;
}

bool
FoamData::allowDesorption() const
{
    return this->allow_desorption_;
}

double
FoamData::rockDensity() const
{
    return this->rock_density_;
}

// FoamConfig member functions.

FoamConfig::FoamConfig(const Deck& deck)
{
    if (deck.hasKeyword<ParserKeywords::FOAMOPTS>()) {
        // We only support the default (GAS transport phase, TAB mobility reduction model)
        // setup for foam at this point, so we detect and deal with it here even though we
        // do not store any data related to it.
        const auto& kw_foamopts = deck.getKeyword<ParserKeywords::FOAMOPTS>();
        if (kw_foamopts.getRecord(0).getItem(0).get<std::string>(0) != "GAS") {
            throw std::runtime_error("In FOAMOPTS, only the GAS transport phase is supported.");
        }
        if (kw_foamopts.getRecord(0).getItem(1).get<std::string>(0) != "TAB") {
            throw std::runtime_error("In FOAMOPTS, only the TAB gas mobility reduction model is supported.");
        }
    }
    if (deck.hasKeyword<ParserKeywords::FOAMFSC>()) {
        if (!deck.hasKeyword<ParserKeywords::FOAMROCK>()) {
            throw std::runtime_error("FOAMFSC present but no FOAMROCK keyword found.");
        }
        // We have both FOAMFSC and FOAMROCK.
        const auto& kw_foamfsc = deck.getKeyword<ParserKeywords::FOAMFSC>();
        const auto& kw_foamrock = deck.getKeyword<ParserKeywords::FOAMROCK>();
        if (kw_foamfsc.size() != kw_foamrock.size()) {
            throw std::runtime_error("FOAMFSC and FOAMROCK keywords have different number of records.");
        }
        const int num_records = kw_foamfsc.size();
        for (int record_index = 0; record_index < num_records; ++record_index) {
            this->data_.emplace_back(kw_foamfsc.getRecord(record_index), kw_foamrock.getRecord(record_index));
        }
    } else if (deck.hasKeyword<ParserKeywords::FOAMROCK>()) {
        // We have FOAMROCK, but not FOAMFSC.
        const auto& kw_foamrock = deck.getKeyword<ParserKeywords::FOAMROCK>();
        for (const auto& record : kw_foamrock) {
            this->data_.emplace_back(record);
        }
    }
}

const FoamData&
FoamConfig::getRecord(std::size_t index) const
{
    return this->data_.at(index);
}

std::size_t
FoamConfig::size() const
{
    return this->data_.size();
}

bool
FoamConfig::empty() const
{
    return this->data_.empty();
}

FoamConfig::const_iterator
FoamConfig::begin() const
{
    return this->data_.begin();
}

FoamConfig::const_iterator
FoamConfig::end() const
{
    return this->data_.end();
}
} // namespace Ewoms
