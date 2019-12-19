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

#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/output/intehead.hh>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/restartconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/arraydimchecker.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/udq/udqactive.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actions.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/actionx.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/tuning.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/regdims.hh>
#include <ewoms/eclio/parser/eclipsestate/tables/tablemanager.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <vector>

namespace {

    using nph_enum = Ewoms::GuideRateModel::Target;
    const std::map<nph_enum, int> nph_enumToECL = {
        {nph_enum::NONE, 0},
        {nph_enum::OIL,  1},
        {nph_enum::GAS,  3},
        {nph_enum::LIQ,  4},
        {nph_enum::RES,  6},
        {nph_enum::COMB, 9},
    };

    using prod_cmode = Ewoms::Well::ProducerCMode;
    const std::map<prod_cmode, int> prod_cmodeToECL = {
        {prod_cmode::NONE,  0},
        {prod_cmode::ORAT,  1},
        {prod_cmode::WRAT,  2},
        {prod_cmode::GRAT,  3},
        {prod_cmode::LRAT,  4},
        {prod_cmode::RESV,  5},
        {prod_cmode::BHP,   7},
    };

    int maxConnPerWell(const Ewoms::Schedule& sched,
                       const std::size_t    lookup_step)
    {
        auto ncwmax = 0;

        for (const auto& well : sched.getWells(lookup_step)) {
            const auto ncw = well.getConnections().size();

            ncwmax = std::max(ncwmax, static_cast<int>(ncw));
        }

        return ncwmax;
    }

    int numGroupsInField(const Ewoms::Schedule& sched,
                         const std::size_t    lookup_step)
    {
        const auto ngmax = sched.numGroups(lookup_step);

        if (ngmax < 1) {
            throw std::invalid_argument {
                "Simulation run must include at least FIELD group"
            };
        }

        // Number of non-FIELD groups.
        return ngmax - 1;
    }

    int GroupControl(const Ewoms::Schedule& sched,
                         const std::size_t    lookup_step)
    {
        int gctrl = 0;

        for (const auto& group_name : sched.groupNames(lookup_step)) {
            const auto& group = sched.getGroup(group_name, lookup_step);
            if (group.isProductionGroup()) {
                gctrl = 1;
            }
            if (group.isInjectionGroup()) {
                gctrl = 2;
            }
        }

        // Index for group control
        return gctrl;
    }

    int noWellUdqs(const Ewoms::Schedule& sched,
               const std::size_t    simStep)
    {
        const auto& udqCfg = sched.getUDQConfig(simStep);
        std::size_t i_wudq = 0;
        for (const auto& udq_input : udqCfg.input()) {
            if (udq_input.var_type() ==  Ewoms::UDQVarType::WELL_VAR) {
                i_wudq++;
            }
        }
        return i_wudq;
    }

    int noGroupUdqs(const Ewoms::Schedule& sched,
                const std::size_t    simStep)
    {
        const auto& udqCfg = sched.getUDQConfig(simStep);
        const auto& input = udqCfg.input();
        return std::count_if(input.begin(), input.end(), [](const Ewoms::UDQInput inp) { return (inp.var_type() == Ewoms::UDQVarType::GROUP_VAR); });

    }

    int noFieldUdqs(const Ewoms::Schedule& sched,
                const std::size_t    simStep)
    {
        const auto& udqCfg = sched.getUDQConfig(simStep);
        std::size_t i_fudq = 0;
        for (const auto& udq_input : udqCfg.input()) {
            if (udq_input.var_type() ==  Ewoms::UDQVarType::FIELD_VAR) {
                i_fudq++;
            }
        }
        return i_fudq;
}

    Ewoms::RestartIO::InteHEAD::WellTableDim
    getWellTableDims(const int              nwgmax,
                     const int              ngmax,
                     const ::Ewoms::Runspec&  rspec,
                     const ::Ewoms::Schedule& sched,
                     const std::size_t      lookup_step)
    {
        const auto& wd = rspec.wellDimensions();

        const auto numWells = static_cast<int>(sched.numWells(lookup_step));

        const auto maxPerf =
            std::max(wd.maxConnPerWell(),
                     maxConnPerWell(sched, lookup_step));

        const auto maxWellInGroup =
            std::max(wd.maxWellsPerGroup(), nwgmax);

        const auto maxGroupInField =
            std::max(wd.maxGroupsInField(), ngmax);

        const auto nWMaxz = wd.maxWellsInField();

        return {
            numWells,
            maxPerf,
            maxWellInGroup,
            maxGroupInField,
            nWMaxz
        };
    }

    std::array<int, 4>
    getNGRPZ(const int             grpsz,
             const int             ngrp,
             const ::Ewoms::Runspec& rspec)
    {
        const auto& wd = rspec.wellDimensions();

        const auto nwgmax = std::max(grpsz, wd.maxWellsPerGroup());
        const auto ngmax  = std::max(ngrp , wd.maxGroupsInField());

        const int nigrpz = 97 + std::max(nwgmax, ngmax);
        const int nsgrpz = 112;
        const int nxgrpz = 180;
        const int nzgrpz = 5;

        return {{
            nigrpz,
            nsgrpz,
            nxgrpz,
            nzgrpz,
        }};
    }

    Ewoms::RestartIO::InteHEAD::UnitSystem
    getUnitConvention(const ::Ewoms::UnitSystem& us)
    {
        using US = ::Ewoms::RestartIO::InteHEAD::UnitSystem;

        switch (us.getType()) {
        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_METRIC:
            return US::Metric;

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_FIELD:
            return US::Field;

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_LAB:
            return US::Lab;

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_PVT_M:
            return US::PVT_M;

        case ::Ewoms::UnitSystem::UnitType::UNIT_TYPE_INPUT:
            throw std::invalid_argument {
                "Cannot Run Simulation With Non-Standard Units"
            };
        }

        return US::Metric;
    }

    Ewoms::RestartIO::InteHEAD::Phases
    getActivePhases(const ::Ewoms::Runspec& rspec)
    {
        auto phases = ::Ewoms::RestartIO::InteHEAD::Phases{};

        const auto& phasePred = rspec.phases();

        phases.oil   = phasePred.active(Ewoms::Phase::OIL);
        phases.water = phasePred.active(Ewoms::Phase::WATER);
        phases.gas   = phasePred.active(Ewoms::Phase::GAS);

        return phases;
    }

    Ewoms::RestartIO::InteHEAD::TuningPar
    getTuningPars(const ::Ewoms::Tuning& tuning,
                  const std::size_t    lookup_step)
    {
        const auto& newtmx = tuning.getNEWTMX(lookup_step);
        const auto& newtmn = tuning.getNEWTMN(lookup_step);
        const auto& litmax = tuning.getLITMAX(lookup_step);
        const auto& litmin = tuning.getLITMIN(lookup_step);
        const auto& mxwsit = tuning.getMXWSIT(lookup_step);
        const auto& mxwpit = tuning.getMXWPIT(lookup_step);

        return {
            newtmx,
            newtmn,
            litmax,
            litmin,
            mxwsit,
            mxwpit,
        };
    }

    Ewoms::RestartIO::InteHEAD::UdqParam
    getUdqParam(const ::Ewoms::Runspec& rspec, const Ewoms::Schedule& sched,
               const std::size_t simStep )
    {
        const auto& udq_par = rspec.udqParams();
        const auto& udqActive = sched.udqActive(simStep);
        const auto r_seed   = udq_par.rand_seed();
        const auto no_wudq  = noWellUdqs(sched, simStep);
        const auto no_gudq  = noGroupUdqs(sched, simStep);
        const auto no_fudq  = noFieldUdqs(sched, simStep);
        const auto no_iuads = udqActive.IUAD_size();
        const auto no_iuaps = udqActive.IUAP_size();

        return { r_seed, static_cast<int>(no_wudq), static_cast<int>(no_gudq), static_cast<int>(no_fudq),
            static_cast<int>(no_iuads), static_cast<int>(no_iuaps)};
    }

    Ewoms::RestartIO::InteHEAD::ActionParam
    getActionParam(const ::Ewoms::Runspec& rspec, const ::Ewoms::Action::Actions& acts )
    {
        const auto& no_act = acts.size();
        const auto max_lines_pr_action = acts.max_input_lines();
        const auto max_cond_per_action = rspec.actdims().max_conditions();
        const auto max_characters_per_line = rspec.actdims().max_characters();

        return { static_cast<int>(no_act), max_lines_pr_action, static_cast<int>(max_cond_per_action), static_cast<int>(max_characters_per_line)};
    }

    Ewoms::RestartIO::InteHEAD::WellSegDims
    getWellSegDims(const ::Ewoms::Runspec&  rspec,
                   const ::Ewoms::Schedule& sched,
                   const std::size_t      lookup_step)
    {
	const auto& wsd = rspec.wellSegmentDimensions();

        const auto& sched_wells = sched.getWells(lookup_step);

        const auto nsegwl =
            std::count_if(std::begin(sched_wells), std::end(sched_wells),
                          [](const Ewoms::Well& well)
            {
                return well.isMultiSegment();
            });

        const auto nswlmx = wsd.maxSegmentedWells();
        const auto nsegmx = wsd.maxSegmentsPerWell();
        const auto nlbrmx = wsd.maxLateralBranchesPerWell();
        const auto nisegz = 22;  // Number of entries per segment in ISEG.
        const auto nrsegz = 146; // Number of entries per segment in RSEG array.  (Eclipse v.2017)
        const auto nilbrz = 10;  // Number of entries per segment in ILBR array.

        return {
            static_cast<int>(nsegwl),
            nswlmx,
            nsegmx,
            nlbrmx,
            nisegz,
            nrsegz,
            nilbrz
        };
    }

    Ewoms::RestartIO::InteHEAD::RegDims
    getRegDims(const ::Ewoms::TableManager& tdims,
               const ::Ewoms::Regdims&      rdims)
    {
        const auto ntfip  = tdims.numFIPRegions();
        const auto nmfipr = rdims.getNMFIPR();
        const auto nrfreg = rdims.getNRFREG();
        const auto ntfreg = rdims.getNTFREG();
        const auto nplmix = rdims.getNPLMIX();

        return {
            static_cast<int>(ntfip),
            static_cast<int>(nmfipr),
            static_cast<int>(nrfreg),
            static_cast<int>(ntfreg),
            static_cast<int>(nplmix),
        };
    }

    Ewoms::RestartIO::InteHEAD::GuideRateNominatedPhase
    setGuideRateNominatedPhase(const ::Ewoms::Schedule& sched,
                     const std::size_t    lookup_step)
    {
            int nom_phase = 0;

            const auto& guideCFG = sched.guideRateConfig(lookup_step);
            if (guideCFG.has_model()) {
                const auto& guideRateModel = guideCFG.model();

                const auto& targPhase = guideRateModel.target();
                const auto& allow_incr = guideRateModel.allow_increase();

                const auto it_nph = nph_enumToECL.find(targPhase);
                    if (it_nph != nph_enumToECL.end()) {
                    nom_phase = it_nph->second;
                    }
                //nominated phase has negative sign for allow increment set to 'NO'
                if (!allow_incr) nom_phase *= -1;
            }

            return {nom_phase};
    }

    int getWhistctlMode(const ::Ewoms::Schedule& sched,
                     const std::size_t    lookup_step)
    {
            int mode = 0;
            const auto& w_hist_ctl_mode = sched.getGlobalWhistctlMmode(lookup_step);
            const auto it_ctl = prod_cmodeToECL.find(w_hist_ctl_mode);
                if (it_ctl != prod_cmodeToECL.end()) {
                    mode = it_ctl->second;
                }

            return mode;
    }

} // Anonymous

// #####################################################################
// Public Interface (createInteHead()) Below Separator
// ---------------------------------------------------------------------

std::vector<int>
Ewoms::RestartIO::Helpers::
createInteHead(const EclipseState& es,
               const EclipseGrid&  grid,
               const Schedule&     sched,
               const double        simTime,
               const int           num_solver_steps,
               const int           lookup_step)
{
    const auto  nwgmax = maxGroupSize(sched, lookup_step);
    const auto  ngmax  = numGroupsInField(sched, lookup_step);
    const auto& acts   = sched.actions(lookup_step);
    const auto& rspec  = es.runspec();
    const auto& tdim   = es.getTableManager();
    const auto& rdim   = tdim.getRegdims();

    const auto ih = InteHEAD{}
        .dimensions         (grid.getNXYZ())
        .numActive          (static_cast<int>(grid.getNumActive()))
        .unitConventions    (getUnitConvention(es.getDeckUnitSystem()))
        .wellTableDimensions(getWellTableDims(nwgmax, ngmax, rspec,
                                              sched, lookup_step))
        .calendarDate       (getSimulationTimePoint(sched.posixStartTime(), simTime))
        .activePhases       (getActivePhases(rspec))
             // The numbers below have been determined experimentally to work
             // across a range of reference cases, but are not guaranteed to be
             // universally valid.
        .params_NWELZ       (155, 122, 130, 3) // n{isxz}welz: number of data elements per well in {ISXZ}WELL
        .params_NCON        (25, 41, 58)       // n{isx}conz: number of data elements per completion in ICON
        .params_GRPZ        (getNGRPZ(nwgmax, ngmax, rspec))
             // ncamax: max number of analytical aquifer connections
             // n{isx}aaqz: number of data elements per aquifer in {ISX}AAQ
             // n{isa}caqz: number of data elements per aquifer connection in {ISA}CAQ
        .params_NAAQZ       (1, 18, 24, 10, 7, 2, 4)
        .stepParam          (num_solver_steps, lookup_step)
        .tuningParam        (getTuningPars(sched.getTuning(), lookup_step))
        .wellSegDimensions  (getWellSegDims(rspec, sched, lookup_step))
        .regionDimensions   (getRegDims(tdim, rdim))
        .ngroups            ({ ngmax })
        .params_NGCTRL      (GroupControl(sched,lookup_step))
        .variousParam       (201802, 100)  // Output should be compatible with Eclipse 100, 2017.02 version.
        .udqParam_1         (getUdqParam(rspec, sched, lookup_step ))
        .actionParam        (getActionParam(rspec, acts))
        .variousUDQ_ACTIONXParam()
        .nominatedPhaseGuideRate(setGuideRateNominatedPhase(sched,lookup_step))
        .whistControlMode(getWhistctlMode(sched,lookup_step))
        ;

    return ih.data();
}
