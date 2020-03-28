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

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <ewoms/eclio/parser/eclipsestate/schedule/group/group.hh>

namespace Ewoms {

    namespace data {

    struct currentGroupConstraints {
        Ewoms::Group::ProductionCMode currentProdConstraint;
        Ewoms::Group::InjectionCMode  currentGasInjectionConstraint;
        Ewoms::Group::InjectionCMode  currentWaterInjectionConstraint;

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const;
        template <class MessageBufferType>
        void read(MessageBufferType& buffer);

        inline currentGroupConstraints& set(  Ewoms::Group::ProductionCMode cpc,
        Ewoms::Group::InjectionCMode  cgic,
        Ewoms::Group::InjectionCMode  cwic);

        inline bool has();

    };

    class Group : public std::map<std::string, Ewoms::data::currentGroupConstraints>  {
    public:

        template <class MessageBufferType>
        void write(MessageBufferType& buffer) const {
            unsigned int size = this->size();
            buffer.write(size);
            for (const auto& witr : *this) {
                const std::string& name = witr.first;
                buffer.write(name);
                const auto& pi_constr = witr.second;
                pi_constr.write(buffer);
            }
        }

        template <class MessageBufferType>
        void read(MessageBufferType& buffer) {
             unsigned int size;
            buffer.read(size);
            for (size_t i = 0; i < size; ++i) {
                std::string name;
                buffer.read(name);
                currentGroupConstraints cgc;
                cgc.read(buffer);
                this->emplace(name, cgc);
            }
        }
    };

    /* IMPLEMENTATIONS */

    template <class MessageBufferType>
    void currentGroupConstraints::write(MessageBufferType& buffer) const {
        buffer.write(this->currentProdConstraint);
        buffer.write(this->currentGasInjectionConstraint);
        buffer.write(this->currentWaterInjectionConstraint);
    }

    template <class MessageBufferType>
    void currentGroupConstraints::read(MessageBufferType& buffer) {
        buffer.read(this->currentProdConstraint);
        buffer.read(this->currentGasInjectionConstraint);
        buffer.read(this->currentWaterInjectionConstraint);
    }

    inline currentGroupConstraints& currentGroupConstraints::set(  Ewoms::Group::ProductionCMode cpc,
        Ewoms::Group::InjectionCMode  cgic,
        Ewoms::Group::InjectionCMode  cwic) {
        this->currentGasInjectionConstraint = cgic;
        this->currentWaterInjectionConstraint = cwic;
        this->currentProdConstraint = cpc;
        return *this;
    }

    inline bool currentGroupConstraints::has() {
        return true;
    }

}} // Ewoms::data

#endif //EWOMS_OUTPUT_GROUPS_H
