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
#ifndef RESTART_VALUE_H
#define RESTART_VALUE_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <ewoms/eclio/output/data/aquifer.hh>
#include <ewoms/eclio/output/data/solution.hh>
#include <ewoms/eclio/output/data/wells.hh>
#include <ewoms/eclio/output/data/groups.hh>

namespace Ewoms {

    class RestartKey {
    public:

        std::string key;
        UnitSystem::measure dim;
        bool required = false;

        RestartKey() = default;

        RestartKey( const std::string& _key, UnitSystem::measure _dim)
            : key(_key),
              dim(_dim),
              required(true)
        {}

        RestartKey( const std::string& _key, UnitSystem::measure _dim, bool _required)
            : key(_key),
              dim(_dim),
              required(_required)
        {}

        bool operator==(const RestartKey& key2) const
        {
            return key == key2.key &&
                   dim == key2.dim &&
                   required == key2.required;
        }
    };

    /*
      A simple class used to communicate values between the simulator and
      the RestartIO functions.
    */
    class RestartValue {
    public:
        using ExtraVector = std::vector<std::pair<RestartKey, std::vector<double>>>;
        data::Solution solution;
        data::Wells wells;
        data::GroupAndNetworkValues grp_nwrk;
        ExtraVector extra;
        std::vector<data::AquiferData> aquifer;

        RestartValue(data::Solution sol, data::Wells wells_arg, data::GroupAndNetworkValues grpn_nwrk_arg);

        RestartValue() {}

        bool hasExtra(const std::string& key) const;
        void addExtra(const std::string& key, UnitSystem::measure dimension, std::vector<double> data);
        void addExtra(const std::string& key, std::vector<double> data);
        const std::vector<double>& getExtra(const std::string& key) const;

        void convertFromSI(const UnitSystem& units);
        void convertToSI(const UnitSystem& units);

        bool operator==(const RestartValue& val2) const
        {
          return solution == val2.solution &&
                 wells == val2.wells &&
                 grp_nwrk == val2.grp_nwrk &&
                 extra == val2.extra;
        }
    };

}

#endif // RESTART_VALUE_H
