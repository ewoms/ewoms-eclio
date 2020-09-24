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

#ifndef EWOMS_OUTPUT_GROUPS_H
#define EWOMS_OUTPUT_GROUPS_H

#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <ewoms/eclio/output/data/guideratevalue.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>

namespace Ewoms { namespace data {

    struct GroupConstraints {
        Ewoms::Group::ProductionCMode currentProdConstraint;
        Ewoms::Group::InjectionCMode  currentGasInjectionConstraint;
        Ewoms::Group::InjectionCMode  currentWaterInjectionConstraint;

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const;

        template <class MessageBufferType>
        void read(MessageBufferType& buffer);

        bool operator==(const GroupConstraints& other) const
        {
            return this->currentProdConstraint == other.currentProdConstraint &&
                   this->currentGasInjectionConstraint == other.currentGasInjectionConstraint &&
                   this->currentWaterInjectionConstraint == other.currentWaterInjectionConstraint;
        }

        inline GroupConstraints& set(Ewoms::Group::ProductionCMode cpc,
                                     Ewoms::Group::InjectionCMode  cgic,
                                     Ewoms::Group::InjectionCMode  cwic);
    };

    struct GroupGuideRates {
        GuideRateValue production{};
        GuideRateValue injection{};

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const
        {
            this->production.write(buffer);
            this->injection .write(buffer);
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer)
        {
            this->production.read(buffer);
            this->injection .read(buffer);
        }

        bool operator==(const GroupGuideRates& other) const
        {
            return this->production == other.production
                && this->injection  == other.injection;
        }
    };

    struct GroupData {
        GroupConstraints currentControl;
        GroupGuideRates  guideRates{};

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const
        {
            this->currentControl.write(buffer);
            this->guideRates    .write(buffer);
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer)
        {
            this->currentControl.read(buffer);
            this->guideRates    .read(buffer);
        }

        bool operator==(const GroupData& other) const
        {
            return this->currentControl == other.currentControl
                && this->guideRates     == other.guideRates;
        }
    };

    struct NodeData {
        double pressure { 0.0 };

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const
        {
            buffer.write(this->pressure);
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer)
        {
            buffer.read(this->pressure);
        }

        bool operator==(const NodeData& other) const
        {
            return this->pressure == other.pressure;
        }
    };

    class GroupAndNetworkValues {
    public:
        std::map<std::string, GroupData> groupData {};
        std::map<std::string, NodeData>  nodeData {};

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const
        {
            this->writeMap(this->groupData, buffer);
            this->writeMap(this->nodeData, buffer);
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer)
        {
            this->readMap(buffer, this->groupData);
            this->readMap(buffer, this->nodeData);
        }

        bool operator==(const GroupAndNetworkValues& other) const
        {
            return (this->groupData == other.groupData)
                && (this->nodeData  == other.nodeData);
        }

        void clear()
        {
            this->groupData.clear();
            this->nodeData.clear();
        }

    private:
        template <class MessageBufferType, class ValueType>
        void writeMap(const std::map<std::string, ValueType>& map,
                      MessageBufferType&                      buffer) const
        {
            const unsigned int size = map.size();
            buffer.write(size);

            for (const auto& mapPair : map) {
                const auto& name = mapPair.first;
                const auto& elm = mapPair.second;
                buffer.write(name);
                elm   .write(buffer);
            }
        }

        template <class MessageBufferType, class ValueType>
        void readMap(MessageBufferType&                buffer,
                     std::map<std::string, ValueType>& map)
        {
            unsigned int size;
            buffer.read(size);

            for (std::size_t i = 0; i < size; ++i) {
                std::string name;
                buffer.read(name);

                auto elm = ValueType{};
                elm.read(buffer);

                map.emplace(name, std::move(elm));
            }
        }
    };

    /* IMPLEMENTATIONS */

    template <class MessageBufferType>
    void GroupConstraints::write(MessageBufferType& buffer) const {
        buffer.write(this->currentProdConstraint);
        buffer.write(this->currentGasInjectionConstraint);
        buffer.write(this->currentWaterInjectionConstraint);
    }

    template <class MessageBufferType>
    void GroupConstraints::read(MessageBufferType& buffer) {
        buffer.read(this->currentProdConstraint);
        buffer.read(this->currentGasInjectionConstraint);
        buffer.read(this->currentWaterInjectionConstraint);
    }

    inline GroupConstraints&
    GroupConstraints::set(Ewoms::Group::ProductionCMode cpc,
                          Ewoms::Group::InjectionCMode  cgic,
                          Ewoms::Group::InjectionCMode  cwic)
    {
        this->currentGasInjectionConstraint = cgic;
        this->currentWaterInjectionConstraint = cwic;
        this->currentProdConstraint = cpc;

        return *this;
    }

}} // Ewoms::data

#endif //EWOMS_OUTPUT_GROUPS_H
