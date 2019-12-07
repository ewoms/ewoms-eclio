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

#ifndef EWOMS_OUTPUT_AQUIFER_H
#define EWOMS_OUTPUT_AQUIFER_H

#include <map>
#include <memory>
#include <vector>

#include <ewoms/eclio/parser/units/unitsystem.hh>

namespace Ewoms { namespace data {

    /**
     * Small struct that keeps track of data for output to restart/summary
     * files.
     */
    enum class AquiferType
    {
        Fetkovich, CarterTracey,
    };

    struct FetkovichData {
        double initVolume;
        double prodIndex;
        double timeConstant;
    };

    struct AquiferData {
        int aquiferID;    //< One-based ID, range 1..NANAQ
        double pressure;  //< Aquifer pressure
        double volume;    //< Produced liquid volume
        double initPressure;    //< Aquifer's initial pressure
        double datumDepth;      //< Aquifer's pressure reference depth

        AquiferType type;
        std::shared_ptr<FetkovichData> aquFet;
    };

}} // Ewoms::data

#endif // EWOMS_OUTPUT_AQUIFER_H
