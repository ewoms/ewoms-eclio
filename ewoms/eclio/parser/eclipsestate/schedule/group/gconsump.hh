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

#ifndef GCONSUMP_H
#define GCONSUMP_H

#include <map>
#include <string>

#include <ewoms/eclio/parser/deck/udavalue.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/summarystate.hh>

namespace Ewoms {

    class GConSump {
    public:
        struct GCONSUMPGroup {
            UDAValue consumption_rate;
            UDAValue import_rate;
            std::string network_node;
            double udq_undefined;
            UnitSystem unit_system;

            bool operator==(const GCONSUMPGroup& data) const {
                return consumption_rate == data.consumption_rate &&
                       import_rate == data.import_rate &&
                       network_node == data.network_node &&
                       udq_undefined == data.udq_undefined &&
                       unit_system == data.unit_system;
            }

            template<class Serializer>
            void serializeOp(Serializer& serializer)
            {
                consumption_rate.serializeOp(serializer);
                import_rate.serializeOp(serializer);
                serializer(network_node);
                serializer(udq_undefined);
                unit_system.serializeOp(serializer);
            }
        };

        struct GCONSUMPGroupProp {
            double consumption_rate;
            double import_rate;
            std::string network_node;
        };

        static GConSump serializeObject();

        bool has(const std::string& name) const;
        const GCONSUMPGroup& get(const std::string& name) const;
        const GCONSUMPGroupProp get(const std::string& name, const SummaryState& st) const;
        void add(const std::string& name, const UDAValue& consumption_rate, const UDAValue& import_rate, const std::string network_node, double udq_undefined_arg, const UnitSystem& unit_system);
        size_t size() const;

        bool operator==(const GConSump& data) const;

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer.map(groups);
        }

    private:
        std::map<std::string, GCONSUMPGroup> groups;
    };

}

#endif
