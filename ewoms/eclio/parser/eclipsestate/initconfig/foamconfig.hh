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

#ifndef EWOMS_FOAMCONFIG_H
#define EWOMS_FOAMCONFIG_H

#include <cstddef>
#include <vector>

namespace Ewoms
{

class Deck;
class DeckRecord;

/// Foam behaviour data for a single SATNUM region.
class FoamData
{
public:
    FoamData();
    FoamData(const DeckRecord& FOAMFSC_record, const DeckRecord& FOAMROCK_record);
    explicit FoamData(const DeckRecord& FOAMROCK_record);
    FoamData(double reference_surfactant_concentration,
             double exponent,
             double minimum_surfactant_concentration,
             bool allow_desorption,
             double rock_density);

    double referenceSurfactantConcentration() const;
    double exponent() const;
    double minimumSurfactantConcentration() const;

    bool allowDesorption() const;
    double rockDensity() const;

    bool operator==(const FoamData& data) const;

private:
    double reference_surfactant_concentration_;
    double exponent_;
    double minimum_surfactant_concentration_;
    bool allow_desorption_;
    double rock_density_;
};

/// Foam behaviour data for all SATNUM regions.
class FoamConfig
{
public:
    FoamConfig() = default;
    explicit FoamConfig(const Deck&);
    FoamConfig(const std::vector<FoamData>& data);

    const FoamData& getRecord(std::size_t index) const;
    const std::vector<FoamData>& records() const;

    std::size_t size() const;
    bool empty() const;

    using const_iterator = std::vector<FoamData>::const_iterator;
    const_iterator begin() const;
    const_iterator end() const;

    bool operator==(const FoamConfig& data) const;

private:
    std::vector<FoamData> data_;
};

} // end namespace Ewoms

#endif // EWOMS_FOAMCONFIG_H