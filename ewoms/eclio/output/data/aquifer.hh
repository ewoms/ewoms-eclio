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
        int aquiferID = 0;    //< One-based ID, range 1..NANAQ
        double pressure = 0.0;  //< Aquifer pressure
        double fluxRate = 0.0; //< Aquifer influx rate (liquid aquifer)
        // TODO: volume should have a better name, since meaning not clear
        double volume = 0.0;    //< Produced liquid volume
        double initPressure = 0.0;    //< Aquifer's initial pressure
        double datumDepth = 0.0;      //< Aquifer's pressure reference depth

        AquiferType type;
        std::shared_ptr<FetkovichData> aquFet;

        double get(const std::string& key) const
        {
            if ( key == "AAQR" ) {
                return this->fluxRate;
            } else if ( key == "AAQT" ) {
                return this->volume;
            } else if ( key == "AAQP" ) {
                return this->pressure;
            }
            return 0.;
        }

        // MessageBufferType API should be similar to Dune::MessageBufferIF
        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const {
            buffer.write(aquiferID);
            buffer.write(pressure);
            buffer.write(fluxRate);
            buffer.write(volume);
            buffer.write(initPressure);
            buffer.write(datumDepth);
            int aqu = aquFet ? 1 : 0;
            buffer.write(aqu);
            if (aquFet) {
                buffer.write(aquFet->initVolume);
                buffer.write(aquFet->prodIndex);
                buffer.write(aquFet->timeConstant);
            }
        }

        // MessageBufferType API should be similar to Dune::MessageBufferIF
        template <class MessageBufferType>
        void read(MessageBufferType& buffer) {
            buffer.read(aquiferID);
            buffer.read(pressure);
            buffer.read(fluxRate);
            buffer.read(volume);
            buffer.read(initPressure);
            buffer.read(datumDepth);
            int aqu;
            buffer.read(aqu);
            if (aqu == 1) {
                if (!aquFet)
                    aquFet = std::make_shared<FetkovichData>();
                buffer.read(aquFet->initVolume);
                buffer.read(aquFet->prodIndex);
                buffer.read(aquFet->timeConstant);
            }
        }
    };

    // TODO: not sure what extension we will need
    using Aquifers = std::map<int, AquiferData>;

}} // Ewoms::data

#endif // EWOMS_OUTPUT_AQUIFER_H
