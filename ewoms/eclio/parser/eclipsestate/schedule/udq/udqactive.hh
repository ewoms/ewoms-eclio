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

#ifndef UDQ_USAGE_H
#define UDQ_USAGE_H

#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>

namespace Ewoms {

class UDAValue;
class UDQConfig;
class UDQActive {
public:

    class Record{
    public:
        Record() :
            input_index(0),
            control(UDAControl::WCONPROD_ORAT),
            uad_code(0),
            use_count(1)
        {}

        Record(const std::string& udq_arg, std::size_t input_index_arg, std::size_t use_index_arg, const std::string& wgname_arg, UDAControl control_arg) :
            udq(udq_arg),
            input_index(input_index_arg),
            use_index(use_index_arg),
            wgname(wgname_arg),
            control(control_arg),
            uad_code(UDQ::uadCode(control_arg)),
            use_count(1)
        {}

        bool operator==(const Record& other) const  {
            if ((this->udq == other.udq) &&
                (this->input_index == other.input_index) &&
                (this->use_index == other.use_index) &&
                (this->wgname == other.wgname) &&
                (this->control == other.control) &&
                (this->uad_code == other.uad_code) &&
                (this->use_count == other.use_count))
                return true;
            return false;
        }

        bool operator!=(const Record& other) const  {
            return !(*this == other);
        }

        std::string udq;
        std::size_t input_index;
        std::size_t use_index = 0;
        std::string wgname;
        UDAControl  control;
        int uad_code;
        std::size_t use_count;
    };

    class InputRecord {
    public:
        InputRecord() :
            input_index(0),
            control(UDAControl::WCONPROD_ORAT)
        {}

        InputRecord(std::size_t input_index_arg, const std::string& udq_arg, const std::string& wgname_arg, UDAControl control_arg) :
            input_index(input_index_arg),
            udq(udq_arg),
            wgname(wgname_arg),
            control(control_arg)
        {}

        bool operator==(const InputRecord& other) const {
            return this->input_index == other.input_index &&
                   this->udq == other.udq &&
                   this->wgname == other.wgname &&
                   this->control == other.control;
        }

        std::size_t input_index;
        std::string udq;
        std::string wgname;
        UDAControl control;
    };

    UDQActive() = default;
    UDQActive(const std::vector<InputRecord>& inputRecs,
              const std::vector<Record>& outputRecs,
              const std::unordered_map<std::string,std::size_t>& udqkeys,
              const std::unordered_map<std::string,std::size_t>& wgkeys);
    int update(const UDQConfig& udq_config, const UDAValue& uda, const std::string& wgname, UDAControl control);
    std::size_t IUAD_size() const;
    std::size_t IUAP_size() const;
    explicit operator bool() const;
    Record operator[](std::size_t index) const;
    const std::vector<Record>& get_iuad() const;
    std::vector<InputRecord> get_iuap() const;

    const std::vector<InputRecord>& getInputRecords() const;
    const std::vector<Record>& getOutputRecords() const;
    const std::unordered_map<std::string, std::size_t>& getUdqKeys() const;
    const std::unordered_map<std::string, std::size_t>& getWgKeys() const;

    bool operator==(const UDQActive& data) const;

private:
    std::string udq_hash(const std::string& udq, UDAControl control);
    std::string wg_hash(const std::string& wgname, UDAControl control);
    int add(const UDQConfig& udq_config, const std::string& udq, const std::string& wgname, UDAControl control);
    int update_input(const UDQConfig& udq_config, const UDAValue& uda, const std::string& wgname, UDAControl control);
    int drop(const std::string& wgname, UDAControl control);

    std::vector<InputRecord> input_data;
    std::vector<Record> mutable output_data;
    std::unordered_map<std::string, std::size_t> udq_keys;
    std::unordered_map<std::string, std::size_t> wg_keys;
};

}

#endif
