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
#include "config.h"

#include <ewoms/eclio/opmlog/opmlog.hh>
#include <ewoms/eclio/output/aggregateudqdata.hh>
#include <ewoms/eclio/output/aggregategroupdata.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>
#include <ewoms/eclio/output/intehead.hh>
#include <ewoms/eclio/output/vectoritems/intehead.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>

#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqstate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqinput.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqinput.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqactive.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqdefine.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqassign.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqenums.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqparams.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqfunctiontable.hh>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>
#include <iostream>

// #####################################################################
// Class Ewoms::RestartIO::Helpers::AggregateGroupData
// ---------------------------------------------------------------------

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;
namespace {

    // maximum number of groups
    std::size_t ngmaxz(const std::vector<int>& inteHead)
    {
        return inteHead[20];
    }

    // maximum number of wells
    std::size_t nwmaxz(const std::vector<int>& inteHead)
    {
        return inteHead[163];
    }

     // Categorize function in terms of which token-types are used in formula
    int define_type(const std::set<Ewoms::UDQTokenType> tokens) {
        int type = -4;
        std::vector <Ewoms::UDQTokenType> type_1 = {
        Ewoms::UDQTokenType::elemental_func_sorta,
        Ewoms::UDQTokenType::elemental_func_sortd,
        Ewoms::UDQTokenType::elemental_func_undef,
        Ewoms::UDQTokenType::scalar_func_sum,
        Ewoms::UDQTokenType::scalar_func_avea,
        Ewoms::UDQTokenType::scalar_func_aveg,
        Ewoms::UDQTokenType::scalar_func_aveh,
        Ewoms::UDQTokenType::scalar_func_max,
        Ewoms::UDQTokenType::scalar_func_min,
        Ewoms::UDQTokenType::binary_op_div
        };

        int num_type_1 = 0;
        for (const auto& tok_type : type_1) {
            num_type_1 += tokens.count(tok_type);
        }
        type = (num_type_1 > 0) ? -1 : -4;
        return type;
    }

    namespace iUdq {

        Ewoms::RestartIO::Helpers::WindowedArray<int>
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<int>;
            int nwin = std::max(udqDims[0], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(udqDims[1]) }
            };
        }

        template <class IUDQArray>
        void staticContrib(const Ewoms::UDQInput& udq_input, IUDQArray& iUdq)
        {
            if (udq_input.is<Ewoms::UDQDefine>()) {
                const auto& udq_define = udq_input.get<Ewoms::UDQDefine>();
                const auto& tokens = udq_define.func_tokens();
                iUdq[0] = 2;
                iUdq[1] = define_type(tokens);
            } else {
                iUdq[0] = 0;
                iUdq[1] = -4;
            }
            iUdq[2] = udq_input.index.typed_insert_index;
        }

    } // iUdq

    namespace iUad {

        Ewoms::RestartIO::Helpers::WindowedArray<int>
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<int>;
            int nwin = std::max(udqDims[2], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(udqDims[3]) }
            };
        }

        template <class IUADArray>
        void staticContrib(const Ewoms::UDQActive::Record& udq_record, IUADArray& iUad, int use_cnt_diff)
        {
            iUad[0] = udq_record.uad_code;
            iUad[1] = udq_record.input_index + 1;

            // entry 3  - unknown meaning - value = 1
            iUad[2] = 1;

            iUad[3] = udq_record.use_count;
            iUad[4] = udq_record.use_index + 1 - use_cnt_diff;
        }
    } // iUad

    namespace zUdn {

        Ewoms::RestartIO::Helpers::WindowedArray<
            Ewoms::EclIO::PaddedOutputString<8>
        >
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<
                Ewoms::EclIO::PaddedOutputString<8>>;
            int nwin = std::max(udqDims[0], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(udqDims[4]) }
            };
        }

    template <class zUdnArray>
    void staticContrib(const Ewoms::UDQInput& udq_input, zUdnArray& zUdn)
    {
        // entry 1 is udq keyword
        zUdn[0] = udq_input.keyword();
        zUdn[1] = udq_input.unit();
    }
    } // zUdn

    namespace zUdl {

        Ewoms::RestartIO::Helpers::WindowedArray<
            Ewoms::EclIO::PaddedOutputString<8>
        >
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<
                Ewoms::EclIO::PaddedOutputString<8>>;
            int nwin = std::max(udqDims[0], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(udqDims[5]) }
            };
        }

    template <class zUdlArray>
    void staticContrib(const Ewoms::UDQInput& input, zUdlArray& zUdl)
        {
            int l_sstr = 8;
            int max_l_str = 128;
            // write out the input formula if key is a DEFINE udq
            if (input.is<Ewoms::UDQDefine>()) {
                const auto& udq_define = input.get<Ewoms::UDQDefine>();
                const std::string& z_data = udq_define.input_string();
                int n_sstr =  z_data.size()/l_sstr;
                if (static_cast<int>(z_data.size()) > max_l_str) {
                    std::cout << "Too long input data string (max 128 characters): " << z_data << std::endl;
                    throw std::invalid_argument("UDQ - variable: " + udq_define.keyword());
                }
                else {
                    for (int i = 0; i < n_sstr; i++) {
                        zUdl[i] = z_data.substr(i*l_sstr, l_sstr);
                    }
                    //add remainder of last non-zero string
                    if ((z_data.size() % l_sstr) > 0)
                        zUdl[n_sstr] = z_data.substr(n_sstr*l_sstr);
                }
            }
        }
    } // zUdl

    namespace iGph {

        Ewoms::RestartIO::Helpers::WindowedArray<int>
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<int>;
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(udqDims[6]) },
                WV::WindowSize{ static_cast<std::size_t>(1) }
            };
        }

        template <class IGPHArray>
        void staticContrib(const int    inj_phase,
                           IGPHArray&   iGph)
        {
                iGph[0] = inj_phase;
        }
    } // iGph

    namespace iUap {

        Ewoms::RestartIO::Helpers::WindowedArray<int>
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<int>;
            int nwin = std::max(udqDims[7], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(1) }
            };
        }

        template <class IUAPArray>
        void staticContrib(const int    wg_no,
                           IUAPArray&   iUap)
        {
                iUap[0] = wg_no+1;
        }
    } // iUap

    namespace dUdw {

        Ewoms::RestartIO::Helpers::WindowedArray<double>
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<double>;
            int nwin = std::max(udqDims[9], 1);
            int nitPrWin = std::max(udqDims[8], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(nitPrWin) }
            };
        }

        template <class DUDWArray>
        void staticContrib(const Ewoms::UDQState& udq_state,
                           const std::vector<std::string>& wnames,
                           const std::string udq,
                           const std::size_t nwmaxz,
                           DUDWArray&   dUdw)
        {
            //initialize array to the default value for the array
            for (std::size_t ind = 0; ind < nwmaxz; ind++) {
                dUdw[ind] = Ewoms::UDQ::restart_default;
            }
            for (std::size_t ind = 0; ind < wnames.size(); ind++) {
                if (udq_state.has_well_var(wnames[ind], udq)) {
                    dUdw[ind] = udq_state.get_well_var(wnames[ind], udq);
                }
            }
        }
    } // dUdw

        namespace dUdg {

        Ewoms::RestartIO::Helpers::WindowedArray<double>
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<double>;
            int nwin = std::max(udqDims[11], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(udqDims[10]) }
            };
        }

        template <class DUDGArray>
        void staticContrib(const Ewoms::UDQState& udq_state,
                           const std::vector<const Ewoms::Group*> groups,
                           const std::string udq,
                           const std::size_t ngmaxz,
                           DUDGArray&   dUdg)
        {
            //initialize array to the default value for the array
            for (std::size_t ind = 0; ind < groups.size(); ind++) {
                if ((groups[ind] == nullptr) || (ind == ngmaxz-1)) {
                    dUdg[ind] = Ewoms::UDQ::restart_default;
                }
                else {
                    if (udq_state.has_group_var((*groups[ind]).name(), udq)) {
                        dUdg[ind] = udq_state.get_group_var((*groups[ind]).name(), udq);
                    }
                    else {
                        dUdg[ind] = Ewoms::UDQ::restart_default;
                    }
                }
            }
        }
    } // dUdg

        namespace dUdf {

        Ewoms::RestartIO::Helpers::WindowedArray<double>
        allocate(const std::vector<int>& udqDims)
        {
            using WV = Ewoms::RestartIO::Helpers::WindowedArray<double>;
            int nwin = std::max(udqDims[12], 1);
            return WV {
                WV::NumWindows{ static_cast<std::size_t>(nwin) },
                WV::WindowSize{ static_cast<std::size_t>(1) }
            };
        }

        template <class DUDFArray>
        void staticContrib(const Ewoms::UDQState& udq_state,
                           const std::string udq,
                           DUDFArray&   dUdf)
        {
            //set value for group name "FIELD"
            if (udq_state.has(udq)) {
                dUdf[0] = udq_state.get(udq);
            }
            else {
                dUdf[0] = Ewoms::UDQ::restart_default;
            }
        }
    } // dUdf
}

// =====================================================================

template < typename T>
std::pair<bool, int > findInVector(const std::vector<T>  & vecOfElements, const T  & element)
{
    std::pair<bool, int > result;

    // Find given element in vector
    auto it = std::find(vecOfElements.begin(), vecOfElements.end(), element);

    if (it != vecOfElements.end())
    {
        result.second = std::distance(vecOfElements.begin(), it);
        result.first = true;
    }
    else
    {
        result.first = false;
        result.second = -1;
    }
    return result;
}

const std::vector<int> Ewoms::RestartIO::Helpers::igphData::ig_phase(const Ewoms::Schedule& sched,
                                                                   const std::size_t simStep,
                                                                   const std::vector<int>& inteHead )
{
    const auto curGroups = sched.restart_groups(simStep);
    std::vector<int> inj_phase(ngmaxz(inteHead), 0);
    for (std::size_t ind = 0; ind < curGroups.size(); ind++) {
        if (curGroups[ind] != nullptr) {
            const auto& group = *curGroups[ind];
            if (group.isInjectionGroup()) {
                /*
                  Initial code could only inject one phase for each group, then
                  numerical value '3' was used for the gas phase, that can not
                  be right?
                */
                int phase_sum = 0;
                if (group.hasInjectionControl(Ewoms::Phase::OIL))
                    phase_sum += 1;
                if (group.hasInjectionControl(Ewoms::Phase::WATER))
                    phase_sum += 2;
                if (group.hasInjectionControl(Ewoms::Phase::GAS))
                    phase_sum += 4;
                inj_phase[group.insert_index()] = phase_sum;
            }
        }
    }
    return inj_phase;
}

const std::vector<int> iuap_data(const Ewoms::Schedule& sched,
                                    const std::size_t simStep,
                                    const std::vector<Ewoms::UDQActive::InputRecord>& iuap)
{
    //construct the current list of well or group sequence numbers to output the IUAP array
    std::vector<int> wg_no;
    Ewoms::UDAKeyword wg_key;

    for (std::size_t ind = 0; ind < iuap.size(); ind++) {
        auto& ctrl = iuap[ind].control;
        wg_key = Ewoms::UDQ::keyword(ctrl);
        if ((wg_key == Ewoms::UDAKeyword::WCONPROD) || (wg_key == Ewoms::UDAKeyword::WCONINJE)) {
            const auto& well = sched.getWell(iuap[ind].wgname, simStep);
            wg_no.push_back(well.seqIndex());
        }
        else if ((wg_key == Ewoms::UDAKeyword::GCONPROD) || (wg_key == Ewoms::UDAKeyword::GCONINJE)) {
            const auto& group = sched.getGroup(iuap[ind].wgname, simStep);
            if (iuap[ind].wgname != "FIELD") {
                wg_no.push_back(group.insert_index() - 1);
            }
        }
        else {
            std::cout << "Invalid Control keyword: " << static_cast<int>(ctrl) << std::endl;
            throw std::invalid_argument("UDQ - variable: " + iuap[ind].udq );
        }

    }

    return wg_no;
}

Ewoms::RestartIO::Helpers::AggregateUDQData::
AggregateUDQData(const std::vector<int>& udqDims)
    : iUDQ_ (iUdq::allocate(udqDims)),
      iUAD_ (iUad::allocate(udqDims)),
      zUDN_ (zUdn::allocate(udqDims)),
      zUDL_ (zUdl::allocate(udqDims)),
      iGPH_ (iGph::allocate(udqDims)),
      iUAP_ (iUap::allocate(udqDims)),
      dUDW_ (dUdw::allocate(udqDims)),
      dUDG_ (dUdg::allocate(udqDims)),
      dUDF_ (dUdf::allocate(udqDims))
{}

// ---------------------------------------------------------------------

void
Ewoms::RestartIO::Helpers::AggregateUDQData::
captureDeclaredUDQData(const Ewoms::Schedule&                 sched,
                       const std::size_t                    simStep,
                       const Ewoms::UDQState&                 udq_state,
                       const std::vector<int>&              inteHead)
{
    const auto& udqCfg = sched.getUDQConfig(simStep);
    const auto nudq = inteHead[VI::intehead::NO_WELL_UDQS] + inteHead[VI::intehead::NO_GROUP_UDQS] + inteHead[VI::intehead::NO_FIELD_UDQS];
    int cnt_udq = 0;
    for (const auto& udq_input : udqCfg.input()) {
        auto udq_index = udq_input.index.insert_index;
        {
            auto i_udq = this->iUDQ_[udq_index];
            iUdq::staticContrib(udq_input, i_udq);
        }
        {
            auto z_udn = this->zUDN_[udq_index];
            zUdn::staticContrib(udq_input, z_udn);
        }
        {
            auto z_udl = this->zUDL_[udq_index];
            zUdl::staticContrib(udq_input, z_udl);
        }
        cnt_udq += 1;
    }
    if (cnt_udq != nudq) {
        std::stringstream str;
        str << "Inconsistent total number of udqs: " << cnt_udq << " and sum of well, group and field udqs: " << nudq;
        OpmLog::error(str.str());
    }

    auto udq_active = sched.udqActive(simStep);
    if (udq_active) {
        const auto& udq_records = udq_active.get_iuad();
        int cnt_iuad = 0;
        for (std::size_t index = 0; index < udq_records.size(); index++) {
            const auto& record = udq_records[index];
            auto i_uad = this->iUAD_[cnt_iuad];
            const auto& ctrl = record.control;
            const auto wg_key = Ewoms::UDQ::keyword(ctrl);
            if (!(((wg_key == Ewoms::UDAKeyword::GCONPROD) || (wg_key == Ewoms::UDAKeyword::GCONINJE)) && (record.wg_name() == "FIELD"))) {
                int use_count_diff = static_cast<int>(index) - cnt_iuad;
                iUad::staticContrib(record, i_uad, use_count_diff);
                cnt_iuad += 1;
            }
        }
        if (cnt_iuad != inteHead[VI::intehead::NO_IUADS]) {
            std::stringstream str;
            str << "Inconsistent number of iuad's: " << cnt_iuad << " number of iuad's from intehead " << inteHead[VI::intehead::NO_IUADS];
            OpmLog::error(str.str());
        }

        const auto& iuap_records = udq_active.get_iuap();
        int cnt_iuap = 0;
        const auto iuap_vect = iuap_data(sched, simStep,iuap_records);
        for (std::size_t index = 0; index < iuap_vect.size(); index++) {
            const auto& wg_no = iuap_vect[index];
            auto i_uap = this->iUAP_[index];
            iUap::staticContrib(wg_no, i_uap);
            cnt_iuap += 1;
        }
        if (cnt_iuap != inteHead[VI::intehead::NO_IUAPS]) {
            std::stringstream str;
            str << "Inconsistent number of iuap's: " << cnt_iuap << " number of iuap's from intehead " << inteHead[VI::intehead::NO_IUAPS];
            OpmLog::error(str.str());
        }

    }
    if (inteHead[VI::intehead::NO_GROUP_UDQS] > 0) {
        Ewoms::RestartIO::Helpers::igphData igph_dat;
        int cnt_igph = 0;
        auto igph = igph_dat.ig_phase(sched, simStep, inteHead);
        for (std::size_t index = 0; index < igph.size(); index++) {
                auto i_igph = this->iGPH_[index];
                iGph::staticContrib(igph[index], i_igph);
                cnt_igph += 1;
        }
        if (cnt_igph != inteHead[VI::intehead::NGMAXZ]) {
            std::stringstream str;
            str << "Inconsistent number of igph's: " << cnt_igph << " number of igph's from intehead " << inteHead[VI::intehead::NGMAXZ];
            OpmLog::error(str.str());
        }
    }

    std::size_t i_wudq = 0;
    const auto& wnames = sched.wellNames(simStep);
    const auto nwmax = nwmaxz(inteHead);
    int cnt_dudw = 0;
    for (const auto& udq_input : udqCfg.input()) {
        if (udq_input.var_type() ==  UDQVarType::WELL_VAR) {
            const std::string& udq = udq_input.keyword();
            auto i_dudw = this->dUDW_[i_wudq];
            dUdw::staticContrib(udq_state, wnames, udq, nwmax, i_dudw);
            i_wudq++;
            cnt_dudw += 1;
        }
    }
    if (cnt_dudw != inteHead[VI::intehead::NO_WELL_UDQS]) {
        std::stringstream str;
        str << "Inconsistent number of dudw's: " << cnt_dudw << " number of dudw's from intehead " << inteHead[VI::intehead::NO_WELL_UDQS];
        OpmLog::error(str.str());
    }

    std::size_t i_gudq = 0;
    const auto curGroups = sched.restart_groups(simStep);
    const auto ngmax = ngmaxz(inteHead);
    int cnt_dudg = 0;
    for (const auto& udq_input : udqCfg.input()) {
        if (udq_input.var_type() ==  UDQVarType::GROUP_VAR) {
            const std::string& udq = udq_input.keyword();
            auto i_dudg = this->dUDG_[i_gudq];
            dUdg::staticContrib(udq_state, curGroups, udq, ngmax, i_dudg);
            i_gudq++;
            cnt_dudg += 1;
        }
    }
    if (cnt_dudg != inteHead[VI::intehead::NO_GROUP_UDQS]) {
        std::stringstream str;
        str << "Inconsistent number of dudg's: " << cnt_dudg << " number of dudg's from intehead " << inteHead[VI::intehead::NO_GROUP_UDQS];
        OpmLog::error(str.str());
    }

    std::size_t i_fudq = 0;
    int cnt_dudf = 0;
    for (const auto& udq_input : udqCfg.input()) {
        if (udq_input.var_type() ==  UDQVarType::FIELD_VAR) {
            const std::string& udq = udq_input.keyword();
            auto i_dudf = this->dUDF_[i_fudq];
            dUdf::staticContrib(udq_state, udq, i_dudf);
            i_fudq++;
            cnt_dudf += 1;
        }
    }
    if (cnt_dudf != inteHead[VI::intehead::NO_FIELD_UDQS]) {
        std::stringstream str;
        str << "Inconsistent number of dudf's: " << cnt_dudf << " number of dudf's from intehead " << inteHead[VI::intehead::NO_FIELD_UDQS];
        OpmLog::error(str.str());
    }

}

