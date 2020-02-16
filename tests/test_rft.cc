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

#define BOOST_TEST_MODULE EclipseRFTWriter

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/io/erft.hh>
#include <ewoms/eclio/io/outputstream.hh>

#include <ewoms/eclio/output/data/solution.hh>
#include <ewoms/eclio/output/data/wells.hh>
#include <ewoms/eclio/output/eclipseio.hh>
#include <ewoms/eclio/output/intehead.hh>
#include <ewoms/eclio/output/writerft.hh>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/ioconfig/ioconfig.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/summaryconfig/summaryconfig.hh>

#include <ewoms/eclio/parser/parsecontext.hh>
#include <ewoms/eclio/parser/parser.hh>

#include <ewoms/eclio/parser/units/units.hh>
#include <ewoms/eclio/parser/units/unitsystem.hh>

#include <cstddef>
#include <ctime>
#include <map>
#include <iomanip>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <ewoms/common/filesystem.hh>

using namespace Ewoms;

namespace std { // hack...
    // For printing ERft::RftDate objects.  Needed by EQUAL_COLLECTIONS.
    static ostream& operator<<(ostream& os, const tuple<int,int,int>& d)
    {
        os <<        setw(4)                 << get<0>(d)
           << "-" << setw(2) << setfill('0') << get<1>(d)
           << "-" << setw(2) << setfill('0') << get<2>(d);

        return os;
    }
}

namespace {
    class RSet
    {
    public:
        explicit RSet(std::string base)
            : odir_(Ewoms::filesystem::temp_directory_path() /
                    Ewoms::unique_path("rset-%%%%"))
            , base_(std::move(base))
        {
            Ewoms::filesystem::create_directories(this->odir_);
        }

        ~RSet()
        {
            Ewoms::filesystem::remove_all(this->odir_);
        }

        std::string outputDir() const
        {
            return this->odir_.string();
        }

        operator ::Ewoms::EclIO::OutputStream::ResultSet() const
        {
            return { this->odir_.string(), this->base_ };
        }

    private:
        Ewoms::filesystem::path odir_;
        std::string             base_;
    };

    class RFTRresults
    {
    public:
        explicit RFTRresults(const ::Ewoms::EclIO::ERft&          rft,
                             const std::string&                 well,
                             const ::Ewoms::EclIO::ERft::RftDate& date);

        float depth(const int i, const int j, const int k) const
        {
            return this->depth_[this->conIx(i, j, k)];
        }

        float pressure(const int i, const int j, const int k) const
        {
            return this->press_[this->conIx(i, j, k)];
        }

        float sgas(const int i, const int j, const int k) const
        {
            return this->sgas_[this->conIx(i, j, k)];
        }

        float swat(const int i, const int j, const int k) const
        {
            return this->swat_[this->conIx(i, j, k)];
        }

    private:
        std::vector<float> depth_;
        std::vector<float> press_;
        std::vector<float> sgas_;
        std::vector<float> swat_;

        std::map<std::tuple<int, int, int>, std::size_t> xConIx_;

        std::size_t conIx(const int i, const int j, const int k) const;
    };

    RFTRresults::RFTRresults(const ::Ewoms::EclIO::ERft&          rft,
                             const std::string&                 well,
                             const ::Ewoms::EclIO::ERft::RftDate& date)
    {
        BOOST_REQUIRE(rft.hasRft(well, date));
        BOOST_REQUIRE(rft.hasArray("CONIPOS", well, date));
        BOOST_REQUIRE(rft.hasArray("CONJPOS", well, date));
        BOOST_REQUIRE(rft.hasArray("CONKPOS", well, date));

        const auto& I = rft.getRft<int>("CONIPOS", well, date);
        const auto& J = rft.getRft<int>("CONJPOS", well, date);
        const auto& K = rft.getRft<int>("CONKPOS", well, date);

        for (auto ncon = I.size(), con = 0*ncon; con < ncon; ++con) {
            this->xConIx_[std::make_tuple(I[con], J[con], K[con])] = con;
        }

        BOOST_REQUIRE(rft.hasArray("DEPTH"   , well, date));
        BOOST_REQUIRE(rft.hasArray("PRESSURE", well, date));
        BOOST_REQUIRE(rft.hasArray("SGAS"    , well, date));
        BOOST_REQUIRE(rft.hasArray("SWAT"    , well, date));

        this->depth_ = rft.getRft<float>("DEPTH"   , well, date);
        this->press_ = rft.getRft<float>("PRESSURE", well, date);
        this->sgas_  = rft.getRft<float>("SGAS"    , well, date);
        this->swat_  = rft.getRft<float>("SWAT"    , well, date);
    }

    std::size_t RFTRresults::conIx(const int i, const int j, const int k) const
    {
        auto conIx = this->xConIx_.find(std::make_tuple(i, j, k));

        if (conIx == this->xConIx_.end()) {
            BOOST_FAIL("Invalid IJK Tuple (" << i << ", "
                       << j << ", " << k << ')');
        }

        return conIx->second;
    }

    void verifyRFTFile(const std::string& rft_filename)
    {
        using RftDate = ::Ewoms::EclIO::ERft::RftDate;

        const auto rft = ::Ewoms::EclIO::ERft{ rft_filename };

        const auto xRFT = RFTRresults {
            rft, "OP_1", RftDate{ 2008, 10, 10 }
        };

        const auto tol = 1.0e-5;

        BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 0.0   , tol);
        BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 1.0e-5, tol);
        BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 2.0e-5, tol);

        BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.0, tol);
        BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.2, tol);
        BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.4, tol);

        BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.0, tol);
        BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.1, tol);
        BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.2, tol);

        BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*0.250 + 0.250/2, tol);
        BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*0.250 + 0.250/2, tol);
        BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*0.250 + 0.250/2, tol);
    }

    data::Solution createBlackoilState(int timeStepIdx, int numCells)
    {
        std::vector< double > pressure( numCells );
        std::vector< double > swat( numCells, 0 );
        std::vector< double > sgas( numCells, 0 );

        for (int i = 0; i < numCells; ++i) {
            pressure[i] = timeStepIdx*1e5 + 1e4 + i;
        }

        data::Solution sol;
        sol.insert( "PRESSURE", UnitSystem::measure::pressure, pressure , data::TargetType::RESTART_SOLUTION );
        sol.insert( "SWAT", UnitSystem::measure::identity, swat , data::TargetType::RESTART_SOLUTION );
        sol.insert( "SGAS", UnitSystem::measure::identity, sgas, data::TargetType::RESTART_SOLUTION );

        return sol;
    }

    std::time_t timeStamp(const ::Ewoms::EclIO::ERft::RftDate& date)
    {
        auto tp = std::tm{};

        tp.tm_year = std::get<0>(date) - 1900;
        tp.tm_mon  = std::get<1>(date) -    1; // 0..11
        tp.tm_mday = std::get<2>(date);        // 1..31

        return ::Ewoms::RestartIO::makeUTCTime(tp);
    }
} // Anonymous namespace

BOOST_AUTO_TEST_SUITE(Using_EclipseIO)

BOOST_AUTO_TEST_CASE(test_RFT)
{
    const auto rset = RSet{ "TESTRFT" };

    const auto eclipse_data_filename = std::string{ "testrft.DATA" };

    const auto deck = Parser{}.parseFile(eclipse_data_filename);
    auto eclipseState = EclipseState { deck };

    eclipseState.getIOConfig().setOutputDir(rset.outputDir());

    {
        /* eclipseWriter is scoped here to ensure it is destroyed after the
         * file itself has been written, because we're going to reload it
         * immediately. first upon destruction can we guarantee it being
         * written to disk and flushed.
         */

        const auto& grid = eclipseState.getInputGrid();
        const auto numCells = grid.getCartesianSize( );

        const Schedule schedule(deck, eclipseState);
        const SummaryConfig summary_config( deck, schedule, eclipseState.getTableManager( ));

        EclipseIO eclipseWriter( eclipseState, grid, schedule, summary_config );

        const auto start_time = schedule.posixStartTime();
        const auto step_time  = timeStamp(::Ewoms::EclIO::ERft::RftDate{ 2008, 10, 10 });

        SummaryState st(std::chrono::system_clock::now());

        data::Rates r1, r2;
        r1.set( data::Rates::opt::wat, 4.11 );
        r1.set( data::Rates::opt::oil, 4.12 );
        r1.set( data::Rates::opt::gas, 4.13 );

        r2.set( data::Rates::opt::wat, 4.21 );
        r2.set( data::Rates::opt::oil, 4.22 );
        r2.set( data::Rates::opt::gas, 4.23 );

        std::vector<Ewoms::data::Connection> well1_comps(9);
        for (size_t i = 0; i < 9; ++i) {
            Ewoms::data::Connection well_comp { grid.getGlobalIndex(8,8,i) ,r1, 0.0 , 0.0, (double)i, 0.1*i,0.2*i, 1.2e3};
            well1_comps[i] = std::move(well_comp);
        }
        std::vector<Ewoms::data::Connection> well2_comps(6);
        for (size_t i = 0; i < 6; ++i) {
            Ewoms::data::Connection well_comp { grid.getGlobalIndex(3,3,i+3) ,r2, 0.0 , 0.0, (double)i, i*0.1,i*0.2, 0.15};
            well2_comps[i] = std::move(well_comp);
        }

        Ewoms::data::Solution solution = createBlackoilState(2, numCells);
        Ewoms::data::Wells wells;

        using SegRes = decltype(wells["w"].segments);
        using Ctrl = decltype(wells["w"].current_control);

        wells["OP_1"] = { std::move(r1), 1.0, 1.1, 3.1, 1, std::move(well1_comps), SegRes{}, Ctrl{} };
        wells["OP_2"] = { std::move(r2), 1.0, 1.1, 3.2, 1, std::move(well2_comps), SegRes{}, Ctrl{} };

        RestartValue restart_value(std::move(solution), std::move(wells));

        eclipseWriter.writeTimeStep( st,
                                     2,
                                     false,
                                     step_time - start_time,
                                     std::move(restart_value));
    }

    verifyRFTFile(Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT"));
}

namespace {
    void verifyRFTFile2(const std::string& rft_filename)
    {
        using RftDate = Ewoms::EclIO::ERft::RftDate;
        const auto rft = ::Ewoms::EclIO::ERft{ rft_filename };

        auto dates = std::unordered_map<
            std::string, std::vector<RftDate>
        >{};

        for (const auto& wellDate : rft.listOfRftReports()) {
            dates[wellDate.first].push_back(wellDate.second);
        }

        // Well OP_1
        {
            auto op_1 = dates.find("OP_1");
            if (op_1 == dates.end()) {
                BOOST_FAIL("Missing RFT Data for Well OP_1");
            }

            const auto expect = std::vector<Ewoms::EclIO::ERft::RftDate> {
                RftDate{ 2008, 10, 10 },
            };

            BOOST_CHECK_EQUAL_COLLECTIONS(op_1->second.begin(),
                                          op_1->second.end(),
                                          expect.begin(), expect.end());
        }

        // Well OP_2
        {
            auto op_2 = dates.find("OP_2");
            if (op_2 == dates.end()) {
                BOOST_FAIL("Missing RFT Data for Well OP_2");
            }

            const auto expect = std::vector<RftDate> {
                RftDate{ 2008, 10, 10 },
                RftDate{ 2008, 11, 10 },
            };

            BOOST_CHECK_EQUAL_COLLECTIONS(op_2->second.begin(),
                                          op_2->second.end(),
                                          expect.begin(), expect.end());
        }
    }
}

BOOST_AUTO_TEST_CASE(test_RFT2)
{
    const auto rset = RSet{ "TESTRFT" };

    const auto eclipse_data_filename = std::string{ "testrft.DATA" };

    const auto deck = Parser().parseFile( eclipse_data_filename );
    auto eclipseState = EclipseState(deck);

    eclipseState.getIOConfig().setOutputDir(rset.outputDir());

    {
        /* eclipseWriter is scoped here to ensure it is destroyed after the
         * file itself has been written, because we're going to reload it
         * immediately. first upon destruction can we guarantee it being
         * written to disk and flushed.
         */

        const auto& grid = eclipseState.getInputGrid();
        const auto numCells = grid.getCartesianSize( );

        Schedule schedule(deck, eclipseState);
        SummaryConfig summary_config( deck, schedule, eclipseState.getTableManager( ));
        SummaryState st(std::chrono::system_clock::now());

        const auto  start_time = schedule.posixStartTime();
        const auto& time_map   = schedule.getTimeMap( );

        for (int counter = 0; counter < 2; counter++) {
            EclipseIO eclipseWriter( eclipseState, grid, schedule, summary_config );
            for (size_t step = 0; step < time_map.size(); step++) {
                const auto step_time = time_map[step];

                data::Rates r1, r2;
                r1.set( data::Rates::opt::wat, 4.11 );
                r1.set( data::Rates::opt::oil, 4.12 );
                r1.set( data::Rates::opt::gas, 4.13 );

                r2.set( data::Rates::opt::wat, 4.21 );
                r2.set( data::Rates::opt::oil, 4.22 );
                r2.set( data::Rates::opt::gas, 4.23 );

                std::vector<Ewoms::data::Connection> well1_comps(9);
                for (size_t i = 0; i < 9; ++i) {
                    Ewoms::data::Connection well_comp { grid.getGlobalIndex(8,8,i) ,r1, 0.0 , 0.0, (double)i, 0.1*i,0.2*i, 3.14e5};
                    well1_comps[i] = std::move(well_comp);
                }
                std::vector<Ewoms::data::Connection> well2_comps(6);
                for (size_t i = 0; i < 6; ++i) {
                    Ewoms::data::Connection well_comp { grid.getGlobalIndex(3,3,i+3) ,r2, 0.0 , 0.0, (double)i, i*0.1,i*0.2, 355.113};
                    well2_comps[i] = std::move(well_comp);
                }

                Ewoms::data::Wells wells;
                Ewoms::data::Solution solution = createBlackoilState(2, numCells);

                using SegRes = decltype(wells["w"].segments);
                using Ctrl = decltype(wells["w"].current_control);

                wells["OP_1"] = { std::move(r1), 1.0, 1.1, 3.1, 1, std::move(well1_comps), SegRes{}, Ctrl{} };
                wells["OP_2"] = { std::move(r2), 1.0, 1.1, 3.2, 1, std::move(well2_comps), SegRes{}, Ctrl{} };

                RestartValue restart_value(std::move(solution), std::move(wells));

                eclipseWriter.writeTimeStep( st,
                                             step,
                                             false,
                                             step_time - start_time,
                                             std::move(restart_value));
            }

            verifyRFTFile2(Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT"));
        }
    }
}

BOOST_AUTO_TEST_SUITE_END() // Using_EclipseIO

// =====================================================================

BOOST_AUTO_TEST_SUITE(Using_Direct_Write)

namespace {
    struct Setup
    {
        explicit Setup(const std::string& deckfile)
            : Setup{ ::Ewoms::Parser{}.parseFile(deckfile) }
        {}

        explicit Setup(const ::Ewoms::Deck& deck)
            : es   { deck }
            , sched{ deck, es }
        {
        }

        ::Ewoms::EclipseState es;
        ::Ewoms::Schedule     sched;
    };

    std::vector<Ewoms::data::Connection>
    connRes_OP1(const ::Ewoms::EclipseGrid& grid)
    {
        auto xcon = std::vector<Ewoms::data::Connection>{};
        xcon.reserve(9);

        for (auto con = 0; con < 9; ++con) {
            xcon.emplace_back();

            auto& c = xcon.back();
            c.index = grid.getGlobalIndex(8, 8, con);

            c.cell_pressure = (120 + con*10)*::Ewoms::unit::barsa;

            c.cell_saturation_gas   = 0.15;
            c.cell_saturation_water = 0.3 + con/20.0;
        }

        return xcon;
    }

    ::Ewoms::data::Well wellSol_OP1(const ::Ewoms::EclipseGrid& grid)
    {
        auto xw = ::Ewoms::data::Well{};
        xw.connections = connRes_OP1(grid);

        return xw;
    }

    std::vector<Ewoms::data::Connection>
    connRes_OP2(const ::Ewoms::EclipseGrid& grid)
    {
        auto xcon = std::vector<Ewoms::data::Connection>{};
        xcon.reserve(6);

        for (auto con = 3; con < 9; ++con) {
            xcon.emplace_back();

            auto& c = xcon.back();
            c.index = grid.getGlobalIndex(3, 3, con);

            c.cell_pressure = (120 + con*10)*::Ewoms::unit::barsa;

            c.cell_saturation_gas   = 0.6 - con/20.0;
            c.cell_saturation_water = 0.25;
        }

        return xcon;
    }

    ::Ewoms::data::Well wellSol_OP2(const ::Ewoms::EclipseGrid& grid)
    {
        auto xw = ::Ewoms::data::Well{};
        xw.connections = connRes_OP2(grid);

        return xw;
    }

    ::Ewoms::data::WellRates wellSol(const ::Ewoms::EclipseGrid& grid)
    {
        auto xw = ::Ewoms::data::Wells{};

        xw["OP_1"] = wellSol_OP1(grid);
        xw["OP_2"] = wellSol_OP2(grid);

        return xw;
    }
}

BOOST_AUTO_TEST_CASE(Basic_Unformatted)
{
    using RftDate = ::Ewoms::EclIO::ERft::RftDate;

    const auto rset  = RSet { "TESTRFT" };
    const auto model = Setup{ "testrft.DATA" };

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ false }
        };

        const auto  reportStep = 2;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, model.es.getUnits(),
                            grid, model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 120.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 130.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 140.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }
    }

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ true }
        };

        const auto  reportStep = 3;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, model.es.getUnits(),
                            grid, model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 120.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 130.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 140.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 11, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 11, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }
    }
}

BOOST_AUTO_TEST_CASE(Basic_Formatted)
{
    using RftDate = ::Ewoms::EclIO::ERft::RftDate;

    const auto rset  = RSet { "TESTRFT" };
    const auto model = Setup{ "testrft.DATA" };

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { true  },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ false }
        };

        const auto  reportStep = 2;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, model.es.getUnits(),
                            grid, model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "FRFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 120.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 130.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 140.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }
    }

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { true },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ true }
        };

        const auto  reportStep = 3;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, model.es.getUnits(),
                            grid, model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "FRFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 120.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 130.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 140.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 11, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 150.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 160.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 170.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 180.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 190.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 200.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 11, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  BARSA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }
    }
}

BOOST_AUTO_TEST_CASE(Field_Units)
{
    using RftDate = ::Ewoms::EclIO::ERft::RftDate;

    const auto rset  = RSet { "TESTRFT" };
    const auto model = Setup{ "testrft.DATA" };
    const auto usys  = ::Ewoms::UnitSystem::newFIELD();

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ false }
        };

        const auto  reportStep = 2;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, usys, grid,
                            model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = ::Ewoms::unit::convert::to(0.25f, ::Ewoms::unit::feet);

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 5.0e-6f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 1.740452852762511e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 1.885490590492720e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 2.030528328222930e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 2.175566065953139e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 2.320603803683348e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 2.465641541413557e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 2.610679279143767e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 2.755717016873976e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 2.900754754604185e+03f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = ::Ewoms::unit::convert::to(0.25f, ::Ewoms::unit::feet);

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 5.0e-6f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 2.175566065953139e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 2.320603803683348e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 2.465641541413557e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 2.610679279143767e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 2.755717016873976e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 2.900754754604185e+03f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "  FEET");
            BOOST_CHECK_EQUAL(welletc[ 4], "  PSIA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " STB/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], "MSCF/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RB/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " FT/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " LB/STB");
            BOOST_CHECK_EQUAL(welletc[14], " LB/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  LB/LB");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "  FEET");
            BOOST_CHECK_EQUAL(welletc[ 4], "  PSIA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " STB/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], "MSCF/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RB/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " FT/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " LB/STB");
            BOOST_CHECK_EQUAL(welletc[14], " LB/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  LB/LB");
        }
    }

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ true }
        };

        const auto  reportStep = 3;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, usys, grid,
                            model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = ::Ewoms::unit::convert::to(0.25f, ::Ewoms::unit::feet);

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 5.0e-6f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 1.740452852762511e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 1.885490590492720e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 2.030528328222930e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 2.175566065953139e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 2.320603803683348e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 2.465641541413557e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 2.610679279143767e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 2.755717016873976e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 2.900754754604185e+03f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = ::Ewoms::unit::convert::to(0.25f, ::Ewoms::unit::feet);

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 5.0e-6f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 2.175566065953139e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 2.320603803683348e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 2.465641541413557e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 2.610679279143767e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 2.755717016873976e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 2.900754754604185e+03f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 11, 10 }
            };

            const auto thick = ::Ewoms::unit::convert::to(0.25f, ::Ewoms::unit::feet);

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 5.0e-6f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 5.0e-6f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 2.175566065953139e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 2.320603803683348e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 2.465641541413557e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 2.610679279143767e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 2.755717016873976e+03f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 2.900754754604185e+03f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "  FEET");
            BOOST_CHECK_EQUAL(welletc[ 4], "  PSIA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " STB/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], "MSCF/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RB/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " FT/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " LB/STB");
            BOOST_CHECK_EQUAL(welletc[14], " LB/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  LB/LB");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "  FEET");
            BOOST_CHECK_EQUAL(welletc[ 4], "  PSIA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " STB/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], "MSCF/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RB/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " FT/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " LB/STB");
            BOOST_CHECK_EQUAL(welletc[14], " LB/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  LB/LB");
        }

        {
            const auto date = RftDate{2008, 11, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "  FEET");
            BOOST_CHECK_EQUAL(welletc[ 4], "  PSIA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " STB/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], "MSCF/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RB/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " FT/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " LB/STB");
            BOOST_CHECK_EQUAL(welletc[14], " LB/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  LB/LB");
        }
    }
}

BOOST_AUTO_TEST_CASE(Lab_Units)
{
    using RftDate = ::Ewoms::EclIO::ERft::RftDate;

    const auto rset  = RSet { "TESTRFT" };
    const auto model = Setup{ "testrft.DATA" };
    const auto usys  = ::Ewoms::UnitSystem::newLAB();

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ false }
        };

        const auto  reportStep = 2;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, usys, grid,
                            model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 25.0f; // cm

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 1.184307920059215e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 1.283000246730817e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 1.381692573402418e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 25.0f; // cm

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "   HR");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "   CM");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 8], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 9], " RCC/HR");
            BOOST_CHECK_EQUAL(welletc[10], " CM/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " GM/SCC");
            BOOST_CHECK_EQUAL(welletc[14], " GM/HR");
            BOOST_CHECK_EQUAL(welletc[15], "  GM/GM");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "   HR");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "   CM");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 8], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 9], " RCC/HR");
            BOOST_CHECK_EQUAL(welletc[10], " CM/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " GM/SCC");
            BOOST_CHECK_EQUAL(welletc[14], " GM/HR");
            BOOST_CHECK_EQUAL(welletc[15], "  GM/GM");
        }
    }

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ true }
        };

        const auto  reportStep = 3;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, usys, grid,
                            model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 25.0f; // cm

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 1.184307920059215e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 1.283000246730817e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 1.381692573402418e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 25.0f; // cm

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 11, 10 }
            };

            const auto thick = 25.0f; // cm

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "   HR");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "   CM");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 8], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 9], " RCC/HR");
            BOOST_CHECK_EQUAL(welletc[10], " CM/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " GM/SCC");
            BOOST_CHECK_EQUAL(welletc[14], " GM/HR");
            BOOST_CHECK_EQUAL(welletc[15], "  GM/GM");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "   HR");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "   CM");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 8], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 9], " RCC/HR");
            BOOST_CHECK_EQUAL(welletc[10], " CM/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " GM/SCC");
            BOOST_CHECK_EQUAL(welletc[14], " GM/HR");
            BOOST_CHECK_EQUAL(welletc[15], "  GM/GM");
        }

        {
            const auto date = RftDate{2008, 11, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "   HR");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], "   CM");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 8], " SCC/HR");
            BOOST_CHECK_EQUAL(welletc[ 9], " RCC/HR");
            BOOST_CHECK_EQUAL(welletc[10], " CM/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " GM/SCC");
            BOOST_CHECK_EQUAL(welletc[14], " GM/HR");
            BOOST_CHECK_EQUAL(welletc[15], "  GM/GM");
        }
    }
}

BOOST_AUTO_TEST_CASE(PVT_M_Units)
{
    using RftDate = ::Ewoms::EclIO::ERft::RftDate;

    const auto rset  = RSet { "TESTRFT" };
    const auto model = Setup{ "testrft.DATA" };
    const auto usys  = ::Ewoms::UnitSystem::newPVT_M();

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ false }
        };

        const auto  reportStep = 2;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, usys, grid,
                            model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 1.184307920059215e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 1.283000246730817e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 1.381692573402418e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25f;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }
    }

    {
        auto rftFile = ::Ewoms::EclIO::OutputStream::RFT {
            rset, ::Ewoms::EclIO::OutputStream::Formatted  { false },
            ::Ewoms::EclIO::OutputStream::RFT::OpenExisting{ true }
        };

        const auto  reportStep = 3;
        const auto  elapsed    = model.sched.seconds(reportStep);
        const auto& grid       = model.es.getInputGrid();

        ::Ewoms::RftIO::write(reportStep, elapsed, usys, grid,
                            model.sched, wellSol(grid), rftFile);
    }

    {
        const auto rft = ::Ewoms::EclIO::ERft {
            ::Ewoms::EclIO::OutputStream::outputFileName(rset, "RFT")
        };

        {
            const auto xRFT = RFTRresults {
                rft, "OP_1", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25;

            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 1), 1*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 2), 2*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 3), 3*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(9, 9, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 1), 1.184307920059215e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 2), 1.283000246730817e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 3), 1.381692573402418e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(9, 9, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 1), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 2), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 3), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 4), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 5), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 6), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 7), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 8), 0.15f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(9, 9, 9), 0.15f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 1), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 2), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 3), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 5), 0.50f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 6), 0.55f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 7), 0.60f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 8), 0.65f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(9, 9, 9), 0.70f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 10, 10 }
            };

            const auto thick = 0.25;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto xRFT = RFTRresults {
                rft, "OP_2", RftDate{ 2008, 11, 10 }
            };

            const auto thick = 0.25;

            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 4), 4*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 5), 5*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 6), 6*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 7), 7*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 8), 8*thick + thick/2.0f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.depth(4, 4, 9), 9*thick + thick/2.0f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 4), 1.480384900074019e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 5), 1.579077226745621e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 6), 1.677769553417222e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 7), 1.776461880088823e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 8), 1.875154206760424e+02f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.pressure(4, 4, 9), 1.973846533432026e+02f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 4), 0.45f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 5), 0.40f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 6), 0.35f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 7), 0.30f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.sgas(4, 4, 9), 0.20f, 1.0e-10f);

            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 4), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 5), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 6), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 7), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 8), 0.25f, 1.0e-10f);
            BOOST_CHECK_CLOSE(xRFT.swat(4, 4, 9), 0.25f, 1.0e-10f);
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_1", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_1", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_1");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 10, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }

        {
            const auto date = RftDate{2008, 11, 10};

            BOOST_CHECK(rft.hasArray("WELLETC", "OP_2", date));

            const auto& welletc = rft.getRft<std::string>("WELLETC", "OP_2", date);

            BOOST_CHECK_EQUAL(welletc[ 0], "  DAYS");
            BOOST_CHECK_EQUAL(welletc[ 1], "OP_2");
            BOOST_CHECK_EQUAL(welletc[ 2], "");
            BOOST_CHECK_EQUAL(welletc[ 3], " METRES");
            BOOST_CHECK_EQUAL(welletc[ 4], "  ATMA");
            BOOST_CHECK_EQUAL(welletc[ 5], "R");
            BOOST_CHECK_EQUAL(welletc[ 6], "STANDARD");
            BOOST_CHECK_EQUAL(welletc[ 7], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 8], " SM3/DAY");
            BOOST_CHECK_EQUAL(welletc[ 9], " RM3/DAY");
            BOOST_CHECK_EQUAL(welletc[10], " M/SEC");
            // No check for welletc[11]
            BOOST_CHECK_EQUAL(welletc[12], "   CP");
            BOOST_CHECK_EQUAL(welletc[13], " KG/SM3");
            BOOST_CHECK_EQUAL(welletc[14], " KG/DAY");
            BOOST_CHECK_EQUAL(welletc[15], "  KG/KG");
        }
    }
}

BOOST_AUTO_TEST_SUITE_END() // Using_Direct_Write
