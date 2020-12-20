#include "config.h"

#define BOOST_TEST_MODULE UDQ_Data

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/eclipsestate/eclipsestate.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/action/state.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/well/well.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/deck/deck.hh>

#include <ewoms/eclio/output/aggregatewelldata.hh>
#include <ewoms/eclio/output/writerestarthelpers.hh>

#include <ewoms/eclio/output/intehead.hh>
#include <ewoms/eclio/output/vectoritems/intehead.hh>
#include <ewoms/eclio/output/vectoritems/doubhead.hh>
#include <ewoms/eclio/output/vectoritems/well.hh>
#include <ewoms/eclio/output/doubhead.hh>
#include <ewoms/eclio/parser/python/python.hh>

#include <ewoms/eclio/parser/units/unitsystem.hh>
#include <ewoms/eclio/parser/units/units.hh>

#include <stdexcept>
#include <utility>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

    Ewoms::Deck first_sim(std::string fname) {
        return Ewoms::Parser{}.parseFile(fname);
    }
}

    Ewoms::SummaryState sum_state()
    {
        auto state = Ewoms::SummaryState{std::chrono::system_clock::now()};
        state.update("FULPR", 460.);

        return state;
    }

//int main(int argc, char* argv[])
struct SimulationCase
{
    explicit SimulationCase(const Ewoms::Deck& deck)
        : es   { deck }
        , grid { deck }
        , python { std::make_shared<Ewoms::Python>()}
        , sched{ deck, es, python }
    {}

    // Order requirement: 'es' must be declared/initialised before 'sched'.
    Ewoms::EclipseState es;
    Ewoms::EclipseGrid  grid;
    std::shared_ptr<Ewoms::Python> python;
    Ewoms::Schedule     sched;

};

BOOST_AUTO_TEST_SUITE(LiftGasOptimization)

// test lift gas optimisation data
BOOST_AUTO_TEST_CASE (liftGasOptimzation_data)
{
    const auto simCase = SimulationCase{first_sim("2_WLIFT_MODEL5_NOINC.DATA")};

    Ewoms::EclipseState es = simCase.es;
    Ewoms::SummaryState st = sum_state();
    Ewoms::Schedule     sched = simCase.sched;
    Ewoms::EclipseGrid  grid = simCase.grid;
    //const auto& ioConfig = es.getIOConfig();
    Ewoms::Action::State action_state;

    // Report Step 1: 2020-01-01
    const auto rptStep = std::size_t{2};
    const auto simStep = std::size_t{1};

    double secs_elapsed = 3.1536E07;
    const auto ih = Ewoms::RestartIO::Helpers::
        createInteHead(es, grid, sched, secs_elapsed,
                       rptStep, rptStep, simStep);

    //set dummy value for next_step_size
    const double next_step_size= 0.1;
    const auto dh = Ewoms::RestartIO::Helpers::createDoubHead(es, sched, simStep,
                                                            secs_elapsed, next_step_size);

    auto wellData = Ewoms::RestartIO::Helpers::AggregateWellData(ih);
    wellData.captureDeclaredWellData(sched, es.getUnits(), simStep, action_state, st, ih);

    // intehead data
    auto eachnc = Ewoms::RestartIO::Helpers::VectorItems::intehead::EACHNCITS;
    auto niwelz = Ewoms::RestartIO::Helpers::VectorItems::intehead::NIWELZ;
    auto nswelz = Ewoms::RestartIO::Helpers::VectorItems::intehead::NSWELZ;

    // doubhead data
    auto lomini = Ewoms::RestartIO::Helpers::VectorItems::doubhead::LOminInt;
    auto loincr = Ewoms::RestartIO::Helpers::VectorItems::doubhead::LOincrsz;
    auto lomineg = Ewoms::RestartIO::Helpers::VectorItems::doubhead::LOminEcGrad;

    // iwel data
    auto liftopt = static_cast<std::size_t>(Ewoms::RestartIO::Helpers::VectorItems::IWell::index::LiftOpt);

    // swel data
    auto lomaxrate = static_cast<std::size_t>(Ewoms::RestartIO::Helpers::VectorItems::SWell::index::LOmaxRate);
    auto lominrate = static_cast<std::size_t>(Ewoms::RestartIO::Helpers::VectorItems::SWell::index::LOminRate);
    auto loweightfac = static_cast<std::size_t>(Ewoms::RestartIO::Helpers::VectorItems::SWell::index::LOweightFac);

    {
        /*
        Check of InteHEAD and DoubHEAD data for LIFTOPT variables
        */

        BOOST_CHECK_EQUAL(ih[eachnc] ,     2);
        BOOST_CHECK_EQUAL(dh[lomini] ,   37.);
        BOOST_CHECK_EQUAL(dh[loincr] ,   12500);
        BOOST_CHECK_EQUAL(dh[lomineg] ,  5E-3);

    }

    {
        // IWEL

        const auto& iWel = wellData.getIWell();

        auto start = 0*static_cast<std::size_t>(ih[niwelz]);
        BOOST_CHECK_EQUAL(iWel[start + liftopt] ,  1);

        start = 1*static_cast<std::size_t>(ih[niwelz]);
        BOOST_CHECK_EQUAL(iWel[start + liftopt] ,  0);
    }

        {
        // SWEL

        const auto& sWel = wellData.getSWell();

        auto start = 0*static_cast<std::size_t>(ih[nswelz]);
        BOOST_CHECK_CLOSE(sWel[start + lomaxrate] ,  150000.f, 1.0e-7f);
        BOOST_CHECK_CLOSE(sWel[start + lominrate] ,     -1.0f, 1.0e-7f);
        BOOST_CHECK_CLOSE(sWel[start + loweightfac] ,   1.01f, 1.0e-7f);

        start = 1*static_cast<std::size_t>(ih[nswelz]);
        BOOST_CHECK_CLOSE(sWel[start + lomaxrate] ,  150000.f, 1.0e-7f);
        BOOST_CHECK_CLOSE(sWel[start + lominrate] ,     0.0f, 1.0e-7f);  // default value since item 2 for this well is 'NO'
        BOOST_CHECK_CLOSE(sWel[start + loweightfac] ,   1.0f, 1.0e-7f);  // default value since item 2 for this well is 'NO'

        start = 2*static_cast<std::size_t>(ih[nswelz]);
        BOOST_CHECK_CLOSE(sWel[start + lomaxrate] ,  150000.f, 1.0e-7f);
        BOOST_CHECK_CLOSE(sWel[start + lominrate] ,     3.0f, 1.0e-7f);
        BOOST_CHECK_CLOSE(sWel[start + loweightfac] ,  1.21f, 1.0e-7f);

        start = 3*static_cast<std::size_t>(ih[nswelz]);
        BOOST_CHECK_CLOSE(sWel[start + lomaxrate] ,  150000.f, 1.0e-7f);
        BOOST_CHECK_CLOSE(sWel[start + lominrate] ,       0.f, 1.0e-7f);
        BOOST_CHECK_CLOSE(sWel[start + loweightfac] ,  1.01f, 1.0e-7f);

        }

}

BOOST_AUTO_TEST_SUITE_END()
