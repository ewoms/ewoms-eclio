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

#ifndef FIELD_DATA_H
#define FIELD_DATA_H

#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/keywords.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fielddata.hh>
#include <ewoms/eclio/parser/deck/value_status.hh>

#include <ewoms/common/optional.hh>

#include<string>
#include<vector>
#include<array>
#include<algorithm>

namespace Ewoms
{
namespace Fieldprops
{
   template<typename T>
    static void compress(std::vector<T>& data, const std::vector<bool>& active_map) {
        std::size_t shift = 0;
        for (std::size_t g = 0; g < active_map.size(); g++) {
            if (active_map[g] && shift > 0) {
                data[g - shift] = data[g];
                continue;
            }

            if (!active_map[g])
                shift += 1;
        }

        data.resize(data.size() - shift);
    }

    template<typename T>
    struct FieldData {
        std::vector<T> data;
        std::vector<value::status> value_status;
        keywords::keyword_info<T> kw_info;
        Ewoms::optional<std::vector<T>> global_data;
        Ewoms::optional<std::vector<value::status>> global_value_status;
        mutable bool all_set;

        FieldData() = default;

        FieldData(const keywords::keyword_info<T>& info, std::size_t active_size, std::size_t global_size) :
            data(std::vector<T>(active_size)),
            value_status(active_size, value::status::uninitialized),
            kw_info(info),
            all_set(false)
        {
            if (global_size != 0) {
                this->global_data = std::vector<T>(global_size);
                this->global_value_status = std::vector<value::status>(global_size, value::status::uninitialized);
            }

            if (info.scalar_init)
                this->default_assign( *info.scalar_init );
        }

        std::size_t size() const {
            return this->data.size();
        }

        bool valid() const {
            if (this->all_set)
                return true;

            static const std::array<value::status,2> invalid_value = {value::status::uninitialized, value::status::empty_default};
            const auto& it = std::find_first_of(this->value_status.begin(), this->value_status.end(), invalid_value.begin(), invalid_value.end());
            this->all_set = (it == this->value_status.end());

            return this->all_set;
        }

        void compress(const std::vector<bool>& active_map) {
            Fieldprops::compress(this->data, active_map);
            Fieldprops::compress(this->value_status, active_map);
        }

        void copy(const FieldData<T>& src, const std::vector<Box::cell_index>& index_list) {
            for (const auto& ci : index_list) {
                this->data[ci.active_index] = src.data[ci.active_index];
                this->value_status[ci.active_index] = src.value_status[ci.active_index];
            }
        }

        void default_assign(T value) {
            std::fill(this->data.begin(), this->data.end(), value);
            std::fill(this->value_status.begin(), this->value_status.end(), value::status::valid_default);

            if (this->global_data) {
                std::fill(this->global_data->begin(), this->global_data->end(), value);
                std::fill(this->global_value_status->begin(), this->global_value_status->end(), value::status::valid_default);
            }
        }

        void default_assign(const std::vector<T>& src) {
            if (src.size() != this->size())
                throw std::invalid_argument("Size mismatch got: " + std::to_string(src.size()) + " expected: " + std::to_string(this->size()));

            std::copy(src.begin(), src.end(), this->data.begin());
            std::fill(this->value_status.begin(), this->value_status.end(), value::status::valid_default);
        }

        void default_update(const std::vector<T>& src) {
            if (src.size() != this->size())
                throw std::invalid_argument("Size mismatch got: " + std::to_string(src.size()) + " expected: " + std::to_string(this->size()));

            for (std::size_t i = 0; i < src.size(); i++) {
                if (!value::has_value(this->value_status[i])) {
                    this->value_status[i] = value::status::valid_default;
                    this->data[i] = src[i];
                }
            }
        }

        void update(std::size_t index, T value, value::status status) {
            this->data[index] = value;
            this->value_status[index] = status;
        }

    };
} // end namespace Fieldprops
} // end namespace Ewoms
#endif // FIELD_DATA_H
