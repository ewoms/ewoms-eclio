/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  eWoms is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "config.h"

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fieldpropsmanager.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/utility/serializer.hh>

#include "fieldprops.hh"

namespace Ewoms {

FieldPropsManager::FieldPropsManager(const Deck& deck, const Phases& phases, const EclipseGrid& grid_arg, const TableManager& tables) :
    fp(std::make_shared<FieldProps>(deck, phases, grid_arg, tables))
{}

void FieldPropsManager::reset_actnum(const std::vector<int>& actnum) {
    this->fp->reset_actnum(actnum);
}

template <typename T>
const std::vector<T>& FieldPropsManager::get(const std::string& keyword) const {
    return this->fp->get<T>(keyword);
}

template <typename T>
const std::vector<T>* FieldPropsManager::try_get(const std::string& keyword) const {
    const auto& data = this->fp->try_get<T>(keyword);
    if (data.valid())
        return data.ptr();

    return nullptr;
}

const Fieldprops::FieldData<int>&
FieldPropsManager::get_int_field_data(const std::string& keyword) const
{
    const auto& data = this->fp->try_get<int>(keyword);
    if (!data.valid())
        throw std::out_of_range("Invalid field data requested.");
    return data.field_data();
}

const Fieldprops::FieldData<double>&
FieldPropsManager::get_double_field_data(const std::string& keyword,
                                         bool allow_unsupported) const
{
    const auto& data = this->fp->try_get<double>(keyword, allow_unsupported);
    if (!data.valid())
        throw std::out_of_range("Invalid field data requested.");
    return data.field_data();
}

template <typename T>
std::vector<T> FieldPropsManager::get_global(const std::string& keyword) const {
    return this->fp->get_global<T>(keyword);
}

template <typename T>
bool FieldPropsManager::supported(const std::string& keyword) {
    return FieldProps::supported<T>(keyword);
}

template <typename T>
bool FieldPropsManager::has(const std::string& keyword) const {
    if (!this->fp->has<T>(keyword))
        return false;
    const auto& data = this->fp->try_get<T>(keyword);
    return data.valid();
}

template <typename T>
std::vector<bool> FieldPropsManager::defaulted(const std::string& keyword) const {
    return this->fp->defaulted<T>(keyword);
}

const std::string& FieldPropsManager::default_region() const {
    return this->fp->default_region();
}

template <typename T>
std::vector<std::string> FieldPropsManager::keys() const {
    return this->fp->keys<T>();
}

std::vector<int> FieldPropsManager::actnum() const {
    return this->fp->actnum();
}

std::vector<double> FieldPropsManager::porv(bool global) const {
    const auto& field_data = this->fp->try_get<double>("PORV").field_data();
    if (global)
        return this->fp->global_copy(field_data.data, field_data.kw_info.scalar_init);
    else
        return field_data.data;
}

std::size_t FieldPropsManager::active_size() const {
    return this->fp->active_size;
}

void FieldPropsManager::apply_tran(const std::string& keyword, std::vector<double>& data) const {
    this->fp->apply_tran(keyword, data);
}

std::vector<char> FieldPropsManager::serialize_tran() const {
    return this->fp->serialize_tran();
}

void FieldPropsManager::deserialize_tran(const std::vector<char>& buffer) {
    this->fp->deserialize_tran(buffer);
}

bool FieldPropsManager::tran_active(const std::string& keyword) const {
    return this->fp->tran_active(keyword);
}

template<class MapType>
void apply_tran(const std::unordered_map<std::string, Fieldprops::TranCalculator>& tran,
                const MapType& double_data,
                std::size_t active_size,
                const std::string& keyword, std::vector<double>& data)
{
    const auto& calculator = tran.at(keyword);
    for (const auto& action : calculator) {
        const auto& action_data = double_data.at(action.field);

        for (std::size_t index = 0; index < active_size; index++) {

            if (action_data.value_status[index] != value::status::deck_value)
                continue;

            switch (action.op) {
            case Fieldprops::ScalarOperation::EQUAL:
                data[index] = action_data.data[index];
                break;

            case Fieldprops::ScalarOperation::MUL:
                data[index] *= action_data.data[index];
                break;

            case Fieldprops::ScalarOperation::ADD:
                data[index] += action_data.data[index];
                break;

            case Fieldprops::ScalarOperation::MAX:
                data[index] = std::min(action_data.data[index], data[index]);
                break;

            case Fieldprops::ScalarOperation::MIN:
                data[index] = std::max(action_data.data[index], data[index]);
                break;

            default:
                throw std::logic_error("Unhandled value in switch");
            }
        }
    }
}

void deserialize_tran(std::unordered_map<std::string, Fieldprops::TranCalculator>& tran, const std::vector<char>& buffer) {
    tran.clear();

    Serializer ser(buffer);
    std::size_t size = ser.get<std::size_t>();
    for (std::size_t calc_index = 0; calc_index < size; calc_index++) {
        std::string calc_name = ser.get<std::string>();
        Fieldprops::TranCalculator calc(calc_name);
        std::size_t calc_size = ser.get<std::size_t>();
        for (std::size_t action_index = 0; action_index < calc_size; action_index++) {
            auto op = static_cast<Fieldprops::ScalarOperation>(ser.get<int>());
            auto field = ser.get<std::string>();

            calc.add_action(op, field);
        }
        tran.emplace(calc_name, std::move(calc));
    }
}

template
void apply_tran(const std::unordered_map<std::string, Fieldprops::TranCalculator>&,
                const std::unordered_map<std::string, Fieldprops::FieldData<double>>&,
                std::size_t, const std::string&, std::vector<double>&);

template
void apply_tran(const std::unordered_map<std::string, Fieldprops::TranCalculator>&,
                const std::map<std::string, Fieldprops::FieldData<double>>&,
                std::size_t, const std::string&, std::vector<double>&);

template bool FieldPropsManager::supported<int>(const std::string&);
template bool FieldPropsManager::supported<double>(const std::string&);

template bool FieldPropsManager::has<int>(const std::string&) const;
template bool FieldPropsManager::has<double>(const std::string&) const;

template std::vector<bool> FieldPropsManager::defaulted<int>(const std::string&) const;
template std::vector<bool> FieldPropsManager::defaulted<double>(const std::string&) const;

template std::vector<std::string> FieldPropsManager::keys<int>() const;
template std::vector<std::string> FieldPropsManager::keys<double>() const;

template std::vector<int> FieldPropsManager::get_global(const std::string& keyword) const;
template std::vector<double> FieldPropsManager::get_global(const std::string& keyword) const;

template const std::vector<int>& FieldPropsManager::get(const std::string& keyword) const;
template const std::vector<double>& FieldPropsManager::get(const std::string& keyword) const;

template const std::vector<int>* FieldPropsManager::try_get(const std::string& keyword) const;
template const std::vector<double>* FieldPropsManager::try_get(const std::string& keyword) const;

}
