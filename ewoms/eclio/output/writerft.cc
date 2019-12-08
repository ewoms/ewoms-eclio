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

#include <ewoms/eclio/output/writerft.hh>

#include <ewoms/eclio/io/outputstream.hh>
#include <ewoms/eclio/io/paddedoutputstring.hh>

#include <ewoms/eclio/output/data/wells.hh>

#include <ewoms/eclio/output/intehead.hh>

#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/connection.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
    enum etcIx : std::vector<Ewoms::EclIO::PaddedOutputString<8>>::size_type
    {
        Time      =  0,
        Well      =  1,
        LGR       =  2,
        Depth     =  3,
        Pressure  =  4,
        DataType  =  5,
        WellType  =  6,
        LiqRate   =  7,
        GasRate   =  8,
        ResVRate  =  9,
        Velocity  = 10,
        Reserved  = 11, // Untouched
        Viscosity = 12,
        ConcPlyBr = 13,
        PlyBrRate = 14,
        PlyBrAds  = 15,
    };

    namespace RftUnits {
        namespace exceptions {
            void metric(std::vector<Ewoms::EclIO::PaddedOutputString<8>>& welletc)
            {
                welletc[etcIx::Depth]     = " METRES";
                welletc[etcIx::Velocity]  = " M/SEC";
            }

            void field(std::vector<Ewoms::EclIO::PaddedOutputString<8>>& welletc)
            {
                welletc[etcIx::Depth]     = "  FEET";
                welletc[etcIx::Velocity]  = " FT/SEC";
                welletc[etcIx::PlyBrRate] = " LB/DAY";
            }

            void lab(std::vector<Ewoms::EclIO::PaddedOutputString<8>>& welletc)
            {
                welletc[etcIx::Time]      = "   HR";
                welletc[etcIx::Pressure]  = "  ATMA";
                welletc[etcIx::Velocity]  = " CM/SEC";
                welletc[etcIx::ConcPlyBr] = " GM/SCC";
                welletc[etcIx::PlyBrRate] = " GM/HR";
                welletc[etcIx::PlyBrAds]  = "  GM/GM";
            }

            void pvt_m(std::vector<Ewoms::EclIO::PaddedOutputString<8>>& welletc)
            {
                // PVT_M is METRIC with pressures in atmospheres.
                metric(welletc);

                welletc[etcIx::Pressure] = "  ATMA";
            }

            void input(std::vector<Ewoms::EclIO::PaddedOutputString<8>>& welletc)
            {
                welletc[etcIx::Time]      = "  INPUT";
                welletc[etcIx::Depth]     = "  INPUT";
                welletc[etcIx::Pressure]  = "  INPUT";
                welletc[etcIx::LiqRate]   = "  INPUT";
                welletc[etcIx::GasRate]   = "  INPUT";
                welletc[etcIx::ResVRate]  = "  INPUT";
                welletc[etcIx::Velocity]  = "  INPUT";
                welletc[etcIx::Viscosity] = "  INPUT";
                welletc[etcIx::ConcPlyBr] = "  INPUT";
                welletc[etcIx::PlyBrRate] = "  INPUT";
                welletc[etcIx::PlyBrAds]  = "  INPUT";
            }
        } // namespace exceptions

        std::string
        centre(std::string s, const std::string::size_type width = 8)
        {
            if (s.size() >  width) { return s.substr(0, width); }
            if (s.size() == width) { return s; }

            const auto npad = width - s.size();
            const auto left = (npad + 1) / 2;  // ceil(npad / 2)

            return std::string(left, ' ') + s;
        }

        std::string combine(std::string left, std::string right)
        {
            return left + '/' + right;
        }

        void fill(const ::Ewoms::UnitSystem&                        usys,
                  std::vector<Ewoms::EclIO::PaddedOutputString<8>>& welletc)
        {
            using M = ::Ewoms::UnitSystem::measure;

            welletc[etcIx::Time]      = centre(usys.name(M::time));
            welletc[etcIx::Depth]     = centre(usys.name(M::length));
            welletc[etcIx::Pressure]  = centre(usys.name(M::pressure));
            welletc[etcIx::LiqRate]   = centre(usys.name(M::liquid_surface_rate));
            welletc[etcIx::GasRate]   = centre(usys.name(M::gas_surface_rate));
            welletc[etcIx::ResVRate]  = centre(usys.name(M::rate));
            welletc[etcIx::Velocity]  = centre(combine(usys.name(M::length),
                                                       usys.name(M::time)));
            welletc[etcIx::Viscosity] = centre(usys.name(M::viscosity));
            welletc[etcIx::ConcPlyBr] =
                centre(combine(usys.name(M::mass),
                               usys.name(M::liquid_surface_volume)));

            welletc[etcIx::PlyBrRate] = centre(usys.name(M::mass_rate));
            welletc[etcIx::PlyBrAds]  = centre(combine(usys.name(M::mass),
                                                       usys.name(M::mass)));
        }
    } // namespace RftUnits

    class WellRFT
    {
    public:
        explicit WellRFT(const std::size_t nconn = 0);

        void add(const ::Ewoms::UnitSystem&       usys,
                 const ::Ewoms::Connection&       conn,
                 const ::Ewoms::data::Connection& xcon,
                 const double                   depth);

        std::size_t nConn() const { return this->i_.size(); }

        const std::vector<int>& conI() const { return this->i_; }
        const std::vector<int>& conJ() const { return this->j_; }
        const std::vector<int>& conK() const { return this->k_; }

        const std::vector<float>& depth()    const { return this->depth_; }
        const std::vector<float>& pressure() const { return this->press_; }
        const std::vector<float>& swat()     const { return this->swat_; }
        const std::vector<float>& sgas()     const { return this->sgas_; }

        const std::vector<Ewoms::EclIO::PaddedOutputString<8>>&
        hostgrid() const
        {
            return this->host_;
        }

    private:
        std::vector<int> i_;
        std::vector<int> j_;
        std::vector<int> k_;

        std::vector<float> depth_;
        std::vector<float> press_;
        std::vector<float> swat_;
        std::vector<float> sgas_;

        std::vector<Ewoms::EclIO::PaddedOutputString<8>> host_;
    };

    WellRFT::WellRFT(const std::size_t nconn)
    {
        if (nconn == 0) { return; }

        this->i_.reserve(nconn);
        this->j_.reserve(nconn);
        this->k_.reserve(nconn);

        this->depth_.reserve(nconn);
        this->press_.reserve(nconn);
        this->swat_ .reserve(nconn);
        this->sgas_ .reserve(nconn);

        this->host_.reserve(nconn);
    }

    void WellRFT::add(const ::Ewoms::UnitSystem&       usys,
                      const ::Ewoms::Connection&       conn,
                      const ::Ewoms::data::Connection& xcon,
                      const double                   depth)
    {
        this->i_.push_back(conn.getI() + 1);
        this->j_.push_back(conn.getJ() + 1);
        this->k_.push_back(conn.getK() + 1);

        using M = ::Ewoms::UnitSystem::measure;
        auto cvrt = [&usys](const M meas, const double x) -> float
        {
            return usys.from_si(meas, x);
        };

        this->depth_.push_back(cvrt(M::length  , depth));
        this->press_.push_back(cvrt(M::pressure, xcon.cell_pressure));

        this->swat_.push_back(xcon.cell_saturation_water);
        this->sgas_.push_back(xcon.cell_saturation_gas);

        this->host_.emplace_back();
    }

    WellRFT
    createWellRFT(const int                                 reportStep,
                  const std::string&                        wname,
                  const ::Ewoms::UnitSystem&                  usys,
                  const ::Ewoms::EclipseGrid&                 grid,
                  const ::Ewoms::Schedule&                    sched,
                  const std::vector<Ewoms::data::Connection>& xcon)
    {
        auto rft = WellRFT{ xcon.size() };

        for (const auto& conn : sched.getWell(wname, reportStep).getConnections()) {
            const auto i = static_cast<std::size_t>(conn.getI());
            const auto j = static_cast<std::size_t>(conn.getJ());
            const auto k = static_cast<std::size_t>(conn.getK());

            if (! grid.cellActive(i, j, k)) {
                // Inactive cell.  Ignore.
                continue;
            }

            const auto ix = grid.getGlobalIndex(i, j, k);
            auto xconPos = std::find_if(xcon.begin(), xcon.end(),
                [ix](const ::Ewoms::data::Connection& c)
            {
                return c.index == ix;
            });

            if (xconPos == xcon.end()) {
                // RFT data not available for this connection.  Unexpected.
                continue;
            }

            rft.add(usys, conn, *xconPos, grid.getCellDepth(ix));
        }

        return rft;
    }

    std::vector<Ewoms::EclIO::PaddedOutputString<8>>
    wellETC(const std::string&       wellName,
            const std::string&       dataType,
            const ::Ewoms::UnitSystem& usys)
    {
        using UT = ::Ewoms::UnitSystem::UnitType;
        auto ret = std::vector<Ewoms::EclIO::PaddedOutputString<8>>(16);

        // Note: ret[etcIx::LGR] is well's LGR.  Default constructed
        // (i.e., blank) string is sufficient to represent no LGR.

        ret[etcIx::Well] = wellName;

          // 'P' -> PLT, 'R' -> RFT, 'S' -> Segment
        ret[etcIx::DataType] = dataType;

        // We support "standard" well type only.
        ret[etcIx::WellType] = "STANDARD";

        RftUnits::fill(usys, ret);

        switch (usys.getType()) {
        case UT::UNIT_TYPE_METRIC:
            RftUnits::exceptions::metric(ret);
            break;

        case UT::UNIT_TYPE_FIELD:
            RftUnits::exceptions::field(ret);
            break;

        case UT::UNIT_TYPE_LAB:
            RftUnits::exceptions::lab(ret);
            break;

        case UT::UNIT_TYPE_PVT_M:
            RftUnits::exceptions::pvt_m(ret);
            break;

        case UT::UNIT_TYPE_INPUT:
            RftUnits::exceptions::input(ret);
            break;
        }

        return ret;
    }

    void writeWellHeader(const double                     elapsed,
                         const std::string&               wellName,
                         const ::Ewoms::Schedule&           sched,
                         const ::Ewoms::UnitSystem&         usys,
                         ::Ewoms::EclIO::OutputStream::RFT& rftFile)
    {
        {
            const auto time =
                usys.from_si(::Ewoms::UnitSystem::measure::time, elapsed);

            rftFile.write("TIME", std::vector<float> {
                static_cast<float>(time)
            });
        }

        {
            const auto timePoint = ::Ewoms::RestartIO::
                getSimulationTimePoint(sched.getStartTime(), elapsed);

            rftFile.write("DATE", std::vector<int> {
                timePoint.day,   // 1..31
                timePoint.month, // 1..12
                timePoint.year,
            });
        }

        rftFile.write("WELLETC", wellETC(wellName, "R", usys));
    }

    void write(const WellRFT& rft, ::Ewoms::EclIO::OutputStream::RFT& rftFile)
    {
        rftFile.write("CONIPOS", rft.conI());
        rftFile.write("CONJPOS", rft.conJ());
        rftFile.write("CONKPOS", rft.conK());

        rftFile.write("HOSTGRID", rft.hostgrid());

        rftFile.write("DEPTH"   , rft.depth());
        rftFile.write("PRESSURE", rft.pressure());
        rftFile.write("SWAT"    , rft.swat());
        rftFile.write("SGAS"    , rft.sgas());
    }

    void writeWellRFT(const int                                 reportStep,
                      const double                              elapsed,
                      const std::string&                        wname,
                      const ::Ewoms::UnitSystem&                  usys,
                      const ::Ewoms::EclipseGrid&                 grid,
                      const ::Ewoms::Schedule&                    sched,
                      const std::vector<Ewoms::data::Connection>& xcon,
                      ::Ewoms::EclIO::OutputStream::RFT&          rftFile)
    {
        const auto rft =
            createWellRFT(reportStep, wname, usys, grid, sched, xcon);

        writeWellHeader(elapsed, wname, sched, usys, rftFile);

        if (rft.nConn() > std::size_t{0}) {
            write(rft, rftFile);
        }
    }
} // Anonymous namespace

void Ewoms::RftIO::write(const int                        reportStep,
                       const double                     elapsed,
                       const ::Ewoms::UnitSystem&         usys,
                       const ::Ewoms::EclipseGrid&        grid,
                       const ::Ewoms::Schedule&           schedule,
                       const ::Ewoms::data::WellRates&    wellSol,
                       ::Ewoms::EclIO::OutputStream::RFT& rftFile)
{
    const auto& rftCfg = schedule.rftConfig();
    if (! rftCfg.active(reportStep)) {
        // RFT not yet activated.  Nothing to do.
        return;
    }

    for (const auto& wname : schedule.wellNames(reportStep)) {
        if (! (rftCfg.rft(wname, reportStep) ||
               rftCfg.plt(wname, reportStep)))
        {
            // RFT output not requested for 'wname' at this time.
            continue;
        }

        // RFT output requested for 'wname' at this time.
        writeWellRFT(reportStep, elapsed, wname, usys, grid,
                     schedule, wellSol.at(wname).connections,
                     rftFile);
    }
}
