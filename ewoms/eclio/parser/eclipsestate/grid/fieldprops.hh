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
#ifndef FIELDPROPS_H
#define FIELDPROPS_H

#include <memory>
#include <limits>
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
#include <ewoms/eclio/parser/eclipsestate/grid/keywords.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/trancalculator.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/fielddata.hh>

namespace Ewoms {

class Deck;
class EclipseGrid;
class TableManager;

namespace Fieldprops
{

namespace keywords {

/*
  Regarding global keywords
  =========================

  It turns out that when the option 'ALL' is used for the PINCH keyword we
  require the MULTZ keyword specified for all cells, also the inactive cells.
  The premise for the FieldProps implementation has all the way been that only
  the active cells should be stored.

  In order to support the ALL option of the PINCH keyword we have bolted on a
  limited support for global storage. By setting .global = true in the
  keyword_info describing the keyword you get:

  1. Normal deck assignment like

       MULTZ
           ..... /

  2. Scalar operations like EQUALS and MULTIPLY.

  These operations also support the full details of the BOX behavior.

  The following operations do not work
  ------------------------------------

  1. Operations involving multiple keywords like

       COPY
         MULTX  MULTZ /
       /

    this also includes the OPERATE which involves multiple keywords for some
    of its operations.

  2. All region operatins like EQUALREG and MULTREG.

  The operations which are not properly implemented will be intercepted and a
  std::logic_error() exception will be thrown.
*/

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
                                                                                      {"MULTZ",   keyword_info<double>{}.init(1.0).mult(true).global_kw(true)},
                                                                                      {"MULTZ-",  keyword_info<double>{}.init(1.0).mult(true)}};

static const std::unordered_map<std::string, keyword_info<int>> int_keywords = {{"ACTNUM",  keyword_info<int>{}.init(1)},
                                                                                {"FLUXNUM", keyword_info<int>{}},
                                                                                {"ISOLNUM", keyword_info<int>{}.init(1)},
                                                                                {"MULTNUM", keyword_info<int>{}.init(1)},
                                                                                {"OPERNUM", keyword_info<int>{}},
                                                                                {"ROCKNUM", keyword_info<int>{}}};

}

namespace EDIT {

/*
  The TRANX, TRANY and TRANZ properties are handled very differently from the
  other properties. It is important that these fields are not entered into the
  double_keywords list of the EDIT section, that way we risk silent failures
  due to the special treatment of the TRAN fields.
*/

static const std::unordered_map<std::string, keyword_info<double>> double_keywords = {{"MULTPV",  keyword_info<double>{}.init(1.0)},
                                                                                      {"PORV",    keyword_info<double>{}.unit_string("ReservoirVolume")},
                                                                                      {"MULTX",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTX-",  keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTY",   keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTY-",  keyword_info<double>{}.init(1.0).mult(true)},
                                                                                      {"MULTZ",   keyword_info<double>{}.init(1.0).mult(true).global_kw(true)},
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
keyword_info<T> global_kw_info(const std::string& name, bool allow_unsupported = false);

} // end namespace keywords

} // end namespace FieldProps

class FieldProps {
public:

    using ScalarOperation = Fieldprops::ScalarOperation;

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

    enum class GetStatus {
         OK = 1,
         INVALID_DATA = 2,               // std::runtime_error
         MISSING_KEYWORD = 3,            // std::out_of_range
         NOT_SUPPPORTED_KEYWORD = 4      // std::logic_error
    };

    template<typename T>
    struct FieldDataManager {
        const std::string& keyword;
        GetStatus status;
        using Data = Fieldprops::FieldData<T>;
        const Data * data_ptr;

        FieldDataManager(const std::string& k, GetStatus s, const Data * d) :
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

        const Data& field_data() const {
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
    FieldDataManager<T> try_get(const std::string& keyword,
                                bool allow_unsupported=false) {
        if (!allow_unsupported && !FieldProps::supported<T>(keyword))
            return FieldDataManager<T>(keyword, GetStatus::NOT_SUPPPORTED_KEYWORD, nullptr);

        const Fieldprops::FieldData<T> * field_data;
        bool has0 = this->has<T>(keyword);

        field_data = std::addressof(this->init_get<T>(keyword,
                                                      std::is_same<T,double>::value && allow_unsupported));
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
        const auto& kw_info = Fieldprops::keywords::global_kw_info<T>(keyword);
        if (kw_info.global)
            return *field_data.global_data;
        else
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
                const auto& kw_info = Fieldprops::keywords::global_kw_info<T>(keyword);
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

    bool tran_active(const std::string& keyword) const;
    void apply_tran(const std::string& keyword, std::vector<double>& data);
    std::vector<char> serialize_tran() const;
    void deserialize_tran(const std::vector<char>& buffer);
private:
    void scanGRIDSection(const GRIDSection& grid_section);
    void scanEDITSection(const EDITSection& edit_section);
    void scanPROPSSection(const PROPSSection& props_section);
    void scanREGIONSSection(const REGIONSSection& regions_section);
    void scanSOLUTIONSection(const SOLUTIONSection& solution_section);
    void scanSCHEDULESection(const SCHEDULESection& schedule_section);
    double getSIValue(const std::string& keyword, double raw_value) const;
    double getSIValue(ScalarOperation op, const std::string& keyword, double raw_value) const;
    template <typename T>
    void erase(const std::string& keyword);

    template <typename T>
    std::vector<T> extract(const std::string& keyword);

    template <typename T>
    void operate(const DeckRecord& record, Fieldprops::FieldData<T>& target_data, const Fieldprops::FieldData<T>& src_data, const std::vector<Box::cell_index>& index_list);

    template <typename T>
    static void apply(ScalarOperation op, std::vector<T>& data, std::vector<value::status>& value_status, T scalar_value, const std::vector<Box::cell_index>& index_list);

    template <typename T>
    Fieldprops::FieldData<T>& init_get(const std::string& keyword, bool allow_unsupported = false);

    template <typename T>
    Fieldprops::FieldData<T>& init_get(const std::string& keyword, const Fieldprops::keywords::keyword_info<T>& kw_info);

    std::string region_name(const DeckItem& region_item);
    std::vector<Box::cell_index> region_index( const std::string& region_name, int region_value );
    void handle_OPERATE(const DeckKeyword& keyword, Box box);
    void handle_operation(const DeckKeyword& keyword, Box box);
    void handle_region_operation(const DeckKeyword& keyword);
    void handle_COPY(const DeckKeyword& keyword, Box box, bool region);
    void distribute_toplayer(Fieldprops::FieldData<double>& field_data, const std::vector<double>& deck_data, const Box& box);
    double get_beta(const std::string& func_name, const std::string& target_array, double raw_beta);
    double get_alpha(const std::string& func_name, const std::string& target_array, double raw_alpha);

    void handle_keyword(const DeckKeyword& keyword, Box& box);
    void handle_double_keyword(Section section, const Fieldprops::keywords::keyword_info<double>& kw_info, const DeckKeyword& keyword, const std::string& keyword_name, const Box& box);
    void handle_double_keyword(Section section, const Fieldprops::keywords::keyword_info<double>& kw_info, const DeckKeyword& keyword, const Box& box);
    void handle_int_keyword(const Fieldprops::keywords::keyword_info<int>& kw_info, const DeckKeyword& keyword, const Box& box);
    void init_satfunc(const std::string& keyword, Fieldprops::FieldData<double>& satfunc);
    void init_porv(Fieldprops::FieldData<double>& porv);
    void init_tempi(Fieldprops::FieldData<double>& tempi);

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
    std::unordered_map<std::string, Fieldprops::FieldData<int>> int_data;
    std::unordered_map<std::string, Fieldprops::FieldData<double>> double_data;

    std::unordered_map<std::string, Fieldprops::TranCalculator> tran;
};

}
#endif
