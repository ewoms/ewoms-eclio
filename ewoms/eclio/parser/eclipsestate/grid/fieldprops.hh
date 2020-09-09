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

#ifndef FIELDPROPS_H
#define FIELDPROPS_H

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include <ewoms/common/optional.hh>

#include <ewoms/eclio/parser/deck/value_status.hh>
#include <ewoms/eclio/parser/deck/decksection.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/box.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/satfuncpropertyinitializers.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>

namespace Ewoms {

class Deck;
class EclipseGrid;
class TableManager;

namespace keywords {

template <typename T>
struct keyword_info {
    Ewoms::optional<std::string> unit = Ewoms::nullopt;
    Ewoms::optional<T> scalar_init = Ewoms::nullopt;
    bool multiplier = false;
    bool top = false;
    bool global = false;

    keyword_info<T>& init(T init_value) {
        this->scalar_init = init_value;
        return *this;
    }

    keyword_info<T>& unit_string(const std::string& unit_string) {
        this->unit = unit_string;
        return *this;
    }

    keyword_info<T>& distribute_top(bool dtop) {
        this->top = dtop;
        return *this;
    }

    keyword_info<T>& mult(bool m) {
        this->multiplier = m;
        return *this;
    }

    keyword_info<T>& global_kw(bool g) {
        this->global = g;
        return *this;
    }
};

inline bool isFipxxx(const std::string& keyword) {
    // FIPxxxx can be any keyword, e.g. FIPREG or FIPXYZ that has the pattern "FIP.+"
    // However, it can not be FIPOWG as that is an actual keyword.
    if (keyword.size() < 4 || keyword == "FIPOWG") {
        return false;
    }
    return keyword[0] == 'F' && keyword[1] == 'I' && keyword[2] == 'P';
}

namespace GRID {
static const std::unordered_map<std::string, keyword_info<double>> double_keywords = {{"MULTPV",  keyword_info<double>{}.init(1.0)},
                                                                                      {"NTG",     keyword_info<double>{}.init(1.0)},
                                                                                      {"PORO",    keyword_info<double>{}.distribute_top(true)},
                                                                                      {"PERMX",   keyword_info<double>{}.unit_string("Permeability").distribute_top(true)},
                                                                                      {"PERMY",   keyword_info<double>{}.unit_string("Permeability").distribute_top(true)},
                                                                                      {"PERMZ",   keyword_info<double>{}.unit_string("Permeability").distribute_top(true)},
                                                                                      {"TEMPI",   keyword_info<double>{}.unit_string("Temperature")},
                                                                                      {"THCONR",  keyword_info<double>{}},
                                                                                      {"THCONSF", keyword_info<double>{}},
                                                                                      {"THCONR",  keyword_info<double>{}},
                                                                                      {"THCROCK", keyword_info<double>{}.unit_string("Energy/AbsoluteTemperature*Length*Time")},
                                                                                      {"THCOIL",  keyword_info<double>{}.unit_string("Energy/AbsoluteTemperature*Length*Time")},
                                                                                      {"THCGAS",  keyword_info<double>{}.unit_string("Energy/AbsoluteTemperature*Length*Time")},
                                                                                      {"THCWATER",keyword_info<double>{}.unit_string("Energy/AbsoluteTemperature*Length*Time")},
                                                                                      {"MULTX",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTX-",  keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTY",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTY-",  keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTZ",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTZ-",  keyword_info<double>{}.init(1.0).mult(true)}};

static const std::unordered_map<std::string, keyword_info<int>> int_keywords = {{"ACTNUM",  keyword_info<int>{}.init(1)},
                                                                                {"FLUXNUM", keyword_info<int>{}},
                                                                                {"ISOLNUM", keyword_info<int>{}.init(1)},
                                                                                {"MULTNUM", keyword_info<int>{}.init(1)},
                                                                                {"OPERNUM", keyword_info<int>{}},
                                                                                {"ROCKNUM", keyword_info<int>{}}};

}

namespace EDIT {
static const std::unordered_map<std::string, keyword_info<double>> double_keywords = {{"MULTPV",  keyword_info<double>{}.init(1.0)},
                                                                                      {"PORV",    keyword_info<double>{}.unit_string("ReservoirVolume")},
                                                                                      {"TRANX",   keyword_info<double>{}.unit_string("Transmissibility").init(1.0)},
                                                                                      {"TRANY",   keyword_info<double>{}.unit_string("Transmissibility").init(1.0)},
                                                                                      {"TRANZ",   keyword_info<double>{}.unit_string("Transmissibility").init(1.0)},
                                                                                      {"MULTX",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTX-",  keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTY",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTY-",  keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTZ",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTZ-",  keyword_info<double>{}.init(1.0).mult(true)}};

static const std::unordered_map<std::string, keyword_info<int>> int_keywords = {};
}

namespace PROPS {
static const std::unordered_map<std::string, keyword_info<double>> double_keywords = {{"SWATINIT", keyword_info<double>{}}};
static const std::unordered_map<std::string, keyword_info<int>> int_keywords = {};

#define dirfunc(base) base, base "X", base "X-", base "Y", base "Y-", base "Z", base "Z-"

static const std::set<std::string> satfunc = {"SWLPC", "ISWLPC", "SGLPC", "ISGLPC",
                                              dirfunc("SGL"),
                                              dirfunc("ISGL"),
                                              dirfunc("SGU"),
                                              dirfunc("ISGU"),
                                              dirfunc("SWL"),
                                              dirfunc("ISWL"),
                                              dirfunc("SWU"),
                                              dirfunc("ISWU"),
                                              dirfunc("SGCR"),
                                              dirfunc("ISGCR"),
                                              dirfunc("SOWCR"),
                                              dirfunc("ISOWCR"),
                                              dirfunc("SOGCR"),
                                              dirfunc("ISOGCR"),
                                              dirfunc("SWCR"),
                                              dirfunc("ISWCR"),
                                              dirfunc("PCW"),
                                              dirfunc("IPCW"),
                                              dirfunc("PCG"),
                                              dirfunc("IPCG"),
                                              dirfunc("KRW"),
                                              dirfunc("IKRW"),
                                              dirfunc("KRWR"),
                                              dirfunc("IKRWR"),
                                              dirfunc("KRO"),
                                              dirfunc("IKRO"),
                                              dirfunc("KRORW"),
                                              dirfunc("IKRORW"),
                                              dirfunc("KRORG"),
                                              dirfunc("IKRORG"),
                                              dirfunc("KRG"),
                                              dirfunc("IKRG"),
                                              dirfunc("KRGR"),
                                              dirfunc("IKRGR")};

static const std::map<std::string,std::string> sogcr_shift = {{"SOGCR",    "SWL"},
                                                              {"SOGCRX",   "SWLX"},
                                                              {"SOGCRX-",  "SWLX-"},
                                                              {"SOGCRY",   "SWLY"},
                                                              {"SOGCRY-",  "SWLY-"},
                                                              {"SOGCRZ",   "SWLZ"},
                                                              {"SOGCRZ-",  "SWLZ-"},
                                                              {"ISOGCR",   "ISWL"},
                                                              {"ISOGCRX",  "ISWLX"},
                                                              {"ISOGCRX-", "ISWLX-"},
                                                              {"ISOGCRY",  "ISWLY"},
                                                              {"ISOGCRY-", "ISWLY-"},
                                                              {"ISOGCRZ",  "ISWLZ"},
                                                              {"ISOGCRZ-", "ISWLZ-"}};

}

namespace REGIONS {

static const std::unordered_map<std::string, keyword_info<int>> int_keywords = {{"ENDNUM",   keyword_info<int>{}.init(1)},
                                                                                {"EQLNUM",   keyword_info<int>{}.init(1)},
                                                                                {"FIPNUM",   keyword_info<int>{}.init(1)},
                                                                                {"IMBNUM",   keyword_info<int>{}.init(1)},
                                                                                {"OPERNUM",  keyword_info<int>{}},
                                                                                {"MISCNUM",  keyword_info<int>{}},
                                                                                {"MISCNUM",  keyword_info<int>{}},
                                                                                {"PVTNUM",   keyword_info<int>{}.init(1)},
                                                                                {"SATNUM",   keyword_info<int>{}.init(1)},
                                                                                {"LWSLTNUM", keyword_info<int>{}},
                                                                                {"ROCKNUM",  keyword_info<int>{}}};
}

namespace SOLUTION {

static const std::unordered_map<std::string, keyword_info<double>> double_keywords = {{"PRESSURE", keyword_info<double>{}.unit_string("Pressure")},
                                                                                      {"SPOLY",    keyword_info<double>{}.unit_string("Density")},
                                                                                      {"SPOLYMW",  keyword_info<double>{}},
                                                                                      {"SSOL",     keyword_info<double>{}},
                                                                                      {"SWAT",     keyword_info<double>{}},
                                                                                      {"SGAS",     keyword_info<double>{}},
                                                                                      {"TEMPI",    keyword_info<double>{}.unit_string("Temperature")},
                                                                                      {"RS",       keyword_info<double>{}.unit_string("GasDissolutionFactor")},
                                                                                      {"RV",       keyword_info<double>{}.unit_string("OilDissolutionFactor")}};

}

namespace SCHEDULE {

static const std::unordered_map<std::string, keyword_info<double>> double_keywords = {};
static const std::unordered_map<std::string, keyword_info<int>> int_keywords = {{"ROCKNUM",   keyword_info<int>{}}};

}

template <typename T>
keyword_info<T> global_kw_info(const std::string& name);

}

class FieldProps {
public:

    struct MultregpRecord {
        int region_value;
        double multiplier;
        std::string region_name;

        MultregpRecord(int rv, double m, const std::string& rn) :
            region_value(rv),
            multiplier(m),
            region_name(rn)
        {}

    };

    enum class ScalarOperation {
         ADD = 1,
         EQUAL = 2,
         MUL = 3,
         MIN = 4,
         MAX = 5
    };

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

    enum class GetStatus {
         OK = 1,
         INVALID_DATA = 2,               // std::runtime_error
         MISSING_KEYWORD = 3,            // std::out_of_range
         NOT_SUPPPORTED_KEYWORD = 4      // std::logic_error
    };

    template<typename T>
    struct FieldData {
        std::vector<T> data;
        std::vector<value::status> value_status;
        keywords::keyword_info<T> kw_info;
        mutable bool all_set;

        FieldData() = default;

        FieldData(const keywords::keyword_info<T>& info, std::size_t active_size) :
            data(std::vector<T>(active_size)),
            value_status(active_size, value::status::uninitialized),
            kw_info(info),
            all_set(false)
        {
            if (info.scalar_init)
                this->default_assign( *info.scalar_init );
        }

        std::size_t size() const {
            return this->data.size();
        }

        bool valid() const {
            if (this->all_set)
                return true;

            static const std::array<value::status,2> invalid_value = {{value::status::uninitialized, value::status::empty_default}};
            const auto& it = std::find_first_of(this->value_status.begin(), this->value_status.end(), invalid_value.begin(), invalid_value.end());
            this->all_set = (it == this->value_status.end());

            return this->all_set;
        }

        void compress(const std::vector<bool>& active_map) {
            FieldProps::compress(this->data, active_map);
            FieldProps::compress(this->value_status, active_map);
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

    template<typename T>
    struct FieldDataManager {
        const std::string& keyword;
        GetStatus status;
        const FieldData<T> * data_ptr;

        FieldDataManager(const std::string& k, GetStatus s, const FieldData<T> * d) :
            keyword(k),
            status(s),
            data_ptr(d)
        { }

        void verify_status() const {
            switch (status) {
            case FieldProps::GetStatus::OK:
                return;
            case FieldProps::GetStatus::INVALID_DATA:
                throw std::runtime_error("The keyword: " + keyword + " has not been fully initialized");
            case FieldProps::GetStatus::MISSING_KEYWORD:
                throw std::out_of_range("No such keyword in deck: " + keyword);
            case FieldProps::GetStatus::NOT_SUPPPORTED_KEYWORD:
                throw std::logic_error("The kewyord  " + keyword + " is not supported");
            }
        }

        const std::vector<T>* ptr() const {
            if (this->data_ptr)
                return std::addressof(this->data_ptr->data);
            else
                return nullptr;
        }

        const std::vector<T>& data() const {
            this->verify_status();
            return this->data_ptr->data;
        }

        const FieldData<T>& field_data() const {
            this->verify_status();
            return *this->data_ptr;
        }

        bool valid() const {
            return (this->status == GetStatus::OK);
        }

    };

    FieldProps(const Deck& deck, const Phases& phases, const EclipseGrid& grid, const TableManager& table_arg);
    void reset_actnum(const std::vector<int>& actnum);

    const std::string& default_region() const;

    std::vector<int> actnum();

    template <typename T>
    static bool supported(const std::string& keyword);

    template <typename T>
    bool has(const std::string& keyword) const;

    template <typename T>
    std::vector<std::string> keys() const;

    template <typename T>
    FieldDataManager<T> try_get(const std::string& keyword) {
        if (!FieldProps::supported<T>(keyword))
            return FieldDataManager<T>(keyword, GetStatus::NOT_SUPPPORTED_KEYWORD, nullptr);

        const FieldData<T> * field_data;
        bool has0 = this->has<T>(keyword);

        field_data = std::addressof(this->init_get<T>(keyword));
        if (field_data->valid())
            return FieldDataManager<T>(keyword, GetStatus::OK, field_data);

        if (!has0) {
            this->erase<T>(keyword);
            return FieldDataManager<T>(keyword, GetStatus::MISSING_KEYWORD, nullptr);
        }

        return FieldDataManager<T>(keyword, GetStatus::INVALID_DATA, nullptr);
    }

    template <typename T>
    const std::vector<T>& get(const std::string& keyword) {
        const auto& data = this->try_get<T>(keyword);
        return data.data();
    }

    template <typename T>
    std::vector<T> get_global(const std::string& keyword) {
        const auto& managed_field_data = this->try_get<T>(keyword);
        const auto& field_data = managed_field_data.field_data();
        const auto& kw_info = keywords::global_kw_info<T>(keyword);
        return this->global_copy(field_data.data, kw_info.scalar_init);
    }

    template <typename T>
    std::vector<T> get_copy(const std::string& keyword, bool global) {
        bool has0 = this->has<T>(keyword);
        const auto& field_data = this->try_get<T>(keyword).field_data();

        if (has0) {
            if (global)
                return this->global_copy(field_data.data, field_data.kw_info.scalar_init);
            else
                return field_data.data;
        } else {
            if (global) {
                const auto& kw_info = keywords::global_kw_info<T>(keyword);
                return this->global_copy(this->extract<T>(keyword), kw_info.scalar_init);
            } else
                return this->extract<T>(keyword);
        }
    }

    template <typename T>
    std::vector<bool> defaulted(const std::string& keyword) {
        const auto& field = this->init_get<T>(keyword);
        std::vector<bool> def(field.size());

        for (std::size_t i=0; i < def.size(); i++)
            def[i] = value::defaulted( field.value_status[i]);

        return def;
    }

    template <typename T>
    std::vector<T> global_copy(const std::vector<T>& data, const Ewoms::optional<T>& default_value) const {
        T fill_value = static_cast<bool>(default_value) ? *default_value : 0;
        std::vector<T> global_data(this->global_size, fill_value);
        std::size_t i = 0;
        for (std::size_t g = 0; g < this->global_size; g++) {
            if (this->m_actnum[g]) {
                global_data[g] = data[i];
                i++;
            }
        }
        return global_data;
    }

    std::size_t active_size;
    std::size_t global_size;

    std::size_t num_int() const {
        return this->int_data.size();
    }

    std::size_t num_double() const {
        return this->double_data.size();
    }

private:
    void scanGRIDSection(const GRIDSection& grid_section);
    void scanEDITSection(const EDITSection& edit_section);
    void scanPROPSSection(const PROPSSection& props_section);
    void scanREGIONSSection(const REGIONSSection& regions_section);
    void scanSOLUTIONSection(const SOLUTIONSection& solution_section);
    void scanSCHEDULESection(const SCHEDULESection& schedule_section);
    double getSIValue(const std::string& keyword, double raw_value) const;
    template <typename T>
    void erase(const std::string& keyword);

    template <typename T>
    std::vector<T> extract(const std::string& keyword);

    template <typename T>
    void apply(const DeckRecord& record, FieldData<T>& target_data, const FieldData<T>& src_data, const std::vector<Box::cell_index>& index_list);

    template <typename T>
    static void apply(ScalarOperation op, FieldData<T>& data, T scalar_value, const std::vector<Box::cell_index>& index_list);

    template <typename T>
    FieldData<T>& init_get(const std::string& keyword);

    std::vector<Box::cell_index> region_index( const DeckItem& regionItem, int region_value );
    std::vector<Box::cell_index> region_index( const std::string& region_name, int region_value );
    void handle_operation(const DeckKeyword& keyword, Box box);
    void handle_region_operation(const DeckKeyword& keyword);
    void handle_COPY(const DeckKeyword& keyword, Box box, bool region);
    void distribute_toplayer(FieldProps::FieldData<double>& field_data, const std::vector<double>& deck_data, const Box& box);
    double get_beta(const std::string& func_name, const std::string& target_array, double raw_beta);
    double get_alpha(const std::string& func_name, const std::string& target_array, double raw_alpha);

    void handle_keyword(const DeckKeyword& keyword, Box& box);
    void handle_double_keyword(Section section, const keywords::keyword_info<double>& kw_info, const DeckKeyword& keyword, const Box& box);
    void handle_int_keyword(const DeckKeyword& keyword, const Box& box);
    void init_satfunc(const std::string& keyword, FieldData<double>& satfunc);
    void init_porv(FieldData<double>& porv);
    void init_tempi(FieldData<double>& tempi);

    const UnitSystem unit_system;
    std::size_t nx,ny,nz;
    Phases m_phases;
    SatFuncControls m_satfuncctrl;
    std::vector<int> m_actnum;
    std::vector<double> cell_volume;
    std::vector<double> cell_depth;
    const std::string m_default_region;
    const EclipseGrid * grid_ptr;      // A bit undecided whether to properly use the grid or not ...
    const TableManager& tables;
    std::shared_ptr<satfunc::RawTableEndPoints> m_rtep;
    std::vector<MultregpRecord> multregp;
    std::unordered_map<std::string, FieldData<int>> int_data;
    std::unordered_map<std::string, FieldData<double>> double_data;
};

}
#endif
