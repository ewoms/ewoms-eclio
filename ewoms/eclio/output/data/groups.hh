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
#include <vector>

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

    struct GroupData {
        GroupConstraints currentControl;

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const
        {
            this->currentControl.write(buffer);
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer)
        {
            this->currentControl.read(buffer);
        }

        bool operator==(const GroupData& other) const
        {
            return this->currentControl == other.currentControl;
        }
    };

    class GroupValues : public std::map<std::string, GroupData>  {
    public:
        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const
        {
            unsigned int size = this->size();
            buffer.write(size);

            for (const auto& it : *this) {
                const auto& gname = it.first;
                const auto& gdata = it.second;
                buffer.write(gname);
                gdata .write(buffer);
            }
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer)
        {
            unsigned int size;
            buffer.read(size);

            for (size_t i = 0; i < size; ++i) {
                std::string name;
                buffer.read(name);

                auto gdata = GroupData{};
                gdata.read(buffer);

                this->emplace(name, gdata);
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
