/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ewoms/eclio/io/rst/header.hh>
#include <ewoms/eclio/io/rst/connection.hh>
#include <ewoms/eclio/io/rst/well.hh>
#include <ewoms/eclio/io/rst/state.hh>

#include <ewoms/eclio/output/vectoritems/connection.hh>
#include <ewoms/eclio/output/vectoritems/well.hh>

namespace VI = ::Ewoms::RestartIO::Helpers::VectorItems;

namespace Ewoms {
namespace RestartIO {

RstState::RstState(const std::vector<int>& intehead,
                   const std::vector<bool>& logihead,
                   const std::vector<double>& doubhead,
                   const std::vector<std::string>& zgrp,
                   const std::vector<int>& igrp,
                   const std::vector<float>& sgrp,
                   const std::vector<double>& xgrp,
                   const std::vector<std::string>& zwel,
                   const std::vector<int>& iwel,
                   const std::vector<float>& swel,
                   const std::vector<double>& xwel,
                   const std::vector<int>& icon,
                   const std::vector<float>& scon,
                   const std::vector<double>& xcon)
{
    RstHeader header(intehead, logihead, doubhead);
    this->add_groups(header, zgrp, igrp, sgrp, xgrp);

    for (int iw = 0; iw < header.num_wells; iw++) {
        std::size_t zwel_offset = iw * header.nzwelz;
        std::size_t iwel_offset = iw * header.niwelz;
        std::size_t swel_offset = iw * header.nswelz;
        std::size_t xwel_offset = iw * header.nxwelz;
        std::size_t icon_offset = iw * header.niconz * header.ncwmax;
        std::size_t scon_offset = iw * header.nsconz * header.ncwmax;
        std::size_t xcon_offset = iw * header.nxconz * header.ncwmax;

        this->wells.emplace_back(header,
                                 zwel.data() + zwel_offset,
                                 iwel.data() + iwel_offset,
                                 swel.data() + swel_offset,
                                 xwel.data() + xwel_offset,
                                 icon.data() + icon_offset,
                                 scon.data() + scon_offset,
                                 xcon.data() + xcon_offset);

        if (this->wells.back().msw_index)
            throw std::logic_error("MSW data not accounted for in this constructor");
    }
}

RstState::RstState(const std::vector<int>& intehead,
             const std::vector<bool>& logihead,
             const std::vector<double>& doubhead,
             const std::vector<std::string>& zgrp,
             const std::vector<int>& igrp,
             const std::vector<float>& sgrp,
             const std::vector<double>& xgrp,
             const std::vector<std::string>& zwel,
             const std::vector<int>& iwel,
             const std::vector<float>& swel,
             const std::vector<double>& xwel,
             const std::vector<int>& icon,
             const std::vector<float>& scon,
             const std::vector<double>& xcon,
             const std::vector<int>& iseg,
             const std::vector<double>& rseg)
{
    RstHeader header(intehead, logihead, doubhead);
    this->add_groups(header, zgrp, igrp, sgrp, xgrp);

    for (int iw = 0; iw < header.num_wells; iw++) {
        std::size_t zwel_offset = iw * header.nzwelz;
        std::size_t iwel_offset = iw * header.niwelz;
        std::size_t swel_offset = iw * header.nswelz;
        std::size_t xwel_offset = iw * header.nxwelz;
        std::size_t icon_offset = iw * header.niconz * header.ncwmax;
        std::size_t scon_offset = iw * header.nsconz * header.ncwmax;
        std::size_t xcon_offset = iw * header.nxconz * header.ncwmax;

        this->wells.emplace_back(header,
                                 zwel.data() + zwel_offset,
                                 iwel.data() + iwel_offset,
                                 swel.data() + swel_offset,
                                 xwel.data() + xwel_offset,
                                 icon.data() + icon_offset,
                                 scon.data() + scon_offset,
                                 xcon.data() + xcon_offset,
                                 iseg,
                                 rseg);
    }
}

void RstState::add_groups(const RstHeader& header,
                          const std::vector<std::string>& zgrp,
                          const std::vector<int>& igrp,
                          const std::vector<float>& sgrp,
                          const std::vector<double>& xgrp)
{
    for (int ig=0; ig < header.ngroup; ig++) {
        std::size_t zgrp_offset = ig * header.nzgrpz;
        std::size_t igrp_offset = ig * header.nigrpz;
        std::size_t sgrp_offset = ig * header.nsgrpz;
        std::size_t xgrp_offset = ig * header.nxgrpz;

        this->groups.emplace_back(zgrp.data() + zgrp_offset,
                                  igrp.data() + igrp_offset,
                                  sgrp.data() + sgrp_offset,
                                  xgrp.data() + xgrp_offset);
    }
}

RstState RstState::load(EclIO::ERst& rst_file, int report_step) {
    rst_file.loadReportStepNumber(report_step);
    const auto& intehead = rst_file.getRst<int>("INTEHEAD", report_step, 0);
    const auto& logihead = rst_file.getRst<bool>("LOGIHEAD", report_step, 0);
    const auto& doubhead = rst_file.getRst<double>("DOUBHEAD", report_step, 0);

    const auto& zgrp = rst_file.getRst<std::string>("ZGRP", report_step, 0);
    const auto& igrp = rst_file.getRst<int>("IGRP", report_step, 0);
    const auto& sgrp = rst_file.getRst<float>("SGRP", report_step, 0);
    const auto& xgrp = rst_file.getRst<double>("XGRP", report_step, 0);

    const auto& zwel = rst_file.getRst<std::string>("ZWEL", report_step, 0);
    const auto& iwel = rst_file.getRst<int>("IWEL", report_step, 0);
    const auto& swel = rst_file.getRst<float>("SWEL", report_step, 0);
    const auto& xwel = rst_file.getRst<double>("XWEL", report_step, 0);

    const auto& icon = rst_file.getRst<int>("ICON", report_step, 0);
    const auto& scon = rst_file.getRst<float>("SCON", report_step, 0);
    const auto& xcon = rst_file.getRst<double>("XCON", report_step, 0);

    if (rst_file.hasKey("ISEG")) {
        const auto& iseg = rst_file.getRst<int>("ISEG", report_step, 0);
        const auto& rseg = rst_file.getRst<double>("RSEG", report_step, 0);

        return RstState(intehead, logihead, doubhead,
                     zgrp, igrp, sgrp, xgrp,
                     zwel, iwel, swel, xwel,
                     icon, scon, xcon,
                     iseg, rseg);
    } else
        return RstState(intehead, logihead, doubhead,
                     zgrp, igrp, sgrp, xgrp,
                     zwel, iwel, swel, xwel,
                     icon, scon, xcon);
}

}
}
