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

#define BOOST_TEST_MODULE TuningTests

#include <boost/test/unit_test.hpp>

#include <ewoms/eclio/parser/deck/deck.hh>
#include <ewoms/eclio/parser/parser.hh>
#include <ewoms/eclio/parser/eclipsestate/grid/eclipsegrid.hh>
#include <ewoms/eclio/parser/eclipsestate/runspec.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/schedule.hh>
#include <ewoms/eclio/parser/eclipsestate/schedule/tuning.hh>
#include <ewoms/eclio/parser/units/units.hh>

using namespace Ewoms;

const std::string& deckStr =  R"(
START
 21 MAY 1981 /

SCHEDULE
TSTEP
 1 2 3 4 5 /

TUNING
2 300 0.3 0.30 6 0.6 0.2 2.25 2E20 /
0.2 0.002 2E-7 0.0002 11 0.02 2.0E-6 0.002 0.002 0.035 66 0.02 2/
13 2 26 2 9 9 4.0E6 4.0E6 4.0E6 1/
DATES
 1 JAN 1982 /
 1 JAN 1982 13:55:44 /
 3 JAN 1982 14:56:45.123 /
/
TSTEP
 9 10 /

TUNING
2 300 0.3 0.30 6 0.6 0.2 2.25 2E20 10.0/
/
/
WSEGITER
/
)";

static Deck createDeck(const std::string& input) {
    Ewoms::Parser parser;
    return parser.parseString(input);
}

BOOST_AUTO_TEST_CASE(TuningTest) {

  auto deck = createDeck(deckStr);
  EclipseGrid grid(10,10,10);
  TableManager table ( deck );
  FieldPropsManager fp(deck, Phases{true, true, true}, grid, table);
  Runspec runspec (deck);
  Schedule schedule( deck, grid , fp, runspec);
  auto event = schedule.getEvents();

  const double diff = 1.0e-14;

  /*** TIMESTEP 4***/
  /********* Record 1 ***********/

  {
      size_t timestep = 4;
      BOOST_CHECK(!event.hasEvent(ScheduleEvents::TUNING_CHANGE, timestep));

      const auto& tuning = schedule.getTuning(4);
      double TSINIT_default = tuning.TSINIT;
      BOOST_CHECK_CLOSE(TSINIT_default, 1 * Metric::Time, diff);

      double TSMAXZ_default = tuning.TSMAXZ;
      BOOST_CHECK_CLOSE(TSMAXZ_default, 365 * Metric::Time, diff);

      double TSMINZ_default = tuning.TSMINZ;
      BOOST_CHECK_CLOSE(TSMINZ_default, 0.1 * Metric::Time, diff);

      double TSMCHP_default = tuning.TSMCHP;
      BOOST_CHECK_CLOSE(TSMCHP_default, 0.15 * Metric::Time, diff);

      double TSFMAX_default = tuning.TSFMAX;
      BOOST_CHECK_CLOSE(TSFMAX_default, 3.0, diff);

      double TSFMIN_default = tuning.TSFMIN;
      BOOST_CHECK_CLOSE(TSFMIN_default, 0.3, diff);

      double TSFCNV_default = tuning.TSFCNV;
      BOOST_CHECK_CLOSE(TSFCNV_default, 0.1, diff);

      double TFDIFF_default = tuning.TFDIFF;
      BOOST_CHECK_CLOSE(TFDIFF_default, 1.25, diff);

      double THRUPT_default = tuning.THRUPT;
      BOOST_CHECK_CLOSE(THRUPT_default, 1E20, diff);

      bool TMAXWC_has_value = tuning.TMAXWC_has_value;
      double TMAXWC_default = tuning.TMAXWC;
      BOOST_CHECK_EQUAL(false, TMAXWC_has_value);
      BOOST_CHECK_CLOSE(TMAXWC_default, 0.0 * Metric::Time, diff);

      /********* Record 2 ************/
      double TRGTTE_default = tuning.TRGTTE;
      BOOST_CHECK_CLOSE(TRGTTE_default, 0.1, diff);

      double TRGCNV_default = tuning.TRGCNV;
      BOOST_CHECK_CLOSE(TRGCNV_default, 0.001, diff);

      double TRGMBE_default = tuning.TRGMBE;
      BOOST_CHECK_CLOSE(TRGMBE_default, 0.0, diff);

      double TRGLCV_default = tuning.TRGLCV;
      BOOST_CHECK_CLOSE(TRGLCV_default, 0.0001, diff);

      double XXXTTE_default = tuning.XXXTTE;
      BOOST_CHECK_CLOSE(XXXTTE_default, 10.0, diff);

      double XXXCNV_default = tuning.XXXCNV;
      BOOST_CHECK_CLOSE(XXXCNV_default, 0.01, diff);

      double XXXMBE_default = tuning.XXXMBE;
      BOOST_CHECK_CLOSE(XXXMBE_default, 1.0E-6, diff);

      double XXXLCV_default = tuning.XXXLCV;
      BOOST_CHECK_CLOSE(XXXLCV_default, 0.001, diff);

      double XXXWFL_default = tuning.XXXWFL;
      BOOST_CHECK_CLOSE(XXXWFL_default, 0.001, diff);

      double TRGFIP_default = tuning.TRGFIP;
      BOOST_CHECK_CLOSE(TRGFIP_default, 0.025, diff);

      bool TRGSFT_has_value = tuning.TRGSFT_has_value;
      double TRGSFT_default = tuning.TRGSFT;
      BOOST_CHECK_EQUAL(false, TRGSFT_has_value);
      BOOST_CHECK_CLOSE(TRGSFT_default, 0.0, diff);

      double THIONX_default = tuning.THIONX;
      BOOST_CHECK_CLOSE(THIONX_default, 0.01, diff);

      int TRWGHT_default = tuning.TRWGHT;
      BOOST_CHECK_EQUAL(TRWGHT_default, 1);

      /********* Record 3 ************/
      int NEWTMX_default = tuning.NEWTMX;
      BOOST_CHECK_EQUAL(NEWTMX_default, 12);

      int NEWTMN_default = tuning.NEWTMN;
      BOOST_CHECK_EQUAL(NEWTMN_default, 1);

      int LITMAX_default = tuning.LITMAX;
      BOOST_CHECK_EQUAL(LITMAX_default, 25);

      int LITMIN_default = tuning.LITMIN;
      BOOST_CHECK_EQUAL(LITMIN_default, 1);

      int MXWSIT_default = tuning.MXWSIT;
      BOOST_CHECK_EQUAL(MXWSIT_default, 8);

      int MXWPIT_default = tuning.MXWPIT;
      BOOST_CHECK_EQUAL(MXWPIT_default, 8);

      double DDPLIM_default = tuning.DDPLIM;
      BOOST_CHECK_CLOSE(DDPLIM_default, 1.0E6 * Metric::Pressure, diff);

      double DDSLIM_default = tuning.DDSLIM;
      BOOST_CHECK_CLOSE(DDSLIM_default, 1.0E6, diff);

      double TRGDPR_default = tuning.TRGDPR;
      BOOST_CHECK_CLOSE(TRGDPR_default, 1.0E6 * Metric::Pressure, diff);

      bool XXXDPR_has_value = tuning.XXXDPR_has_value;
      double XXXDPR_default = tuning.XXXDPR;
      BOOST_CHECK_EQUAL(false, XXXDPR_has_value);
      BOOST_CHECK_CLOSE(XXXDPR_default, 0.0, diff);
  }

  /*** TIMESTEP 5***/
  /********* Record 1 ***********/
  {
      std::size_t timestep = 5;
      const auto& tuning = schedule.getTuning(timestep);

      BOOST_CHECK(event.hasEvent(ScheduleEvents::TUNING_CHANGE, timestep));
      double TSINIT = tuning.TSINIT;
      BOOST_CHECK_CLOSE(TSINIT, 2 * Metric::Time, diff);

      double TSMAXZ = tuning.TSMAXZ;
      BOOST_CHECK_CLOSE(TSMAXZ, 300 * Metric::Time, diff);

      double TSMINZ = tuning.TSMINZ;
      BOOST_CHECK_CLOSE(TSMINZ, 0.3 * Metric::Time, diff);

      double TSMCHP = tuning.TSMCHP;
      BOOST_CHECK_CLOSE(TSMCHP, 0.30 * Metric::Time, diff);

      double TSFMAX = tuning.TSFMAX;
      BOOST_CHECK_CLOSE(TSFMAX, 6.0, 1.0);

      double TSFMIN = tuning.TSFMIN;
      BOOST_CHECK_CLOSE(TSFMIN, 0.6, 1.0);

      double TSFCNV = tuning.TSFCNV;
      BOOST_CHECK_CLOSE(TSFCNV, 0.2, diff);

      double TFDIFF = tuning.TFDIFF;
      BOOST_CHECK_CLOSE(TFDIFF, 2.25, diff);

      double THRUPT = tuning.THRUPT;
      BOOST_CHECK_CLOSE(THRUPT, 2E20, diff);

      BOOST_CHECK_EQUAL(false, tuning.TMAXWC_has_value);
      BOOST_CHECK_CLOSE(tuning.TMAXWC, 0.0 * Metric::Time, diff);

      /********* Record 2 ***********/
      double TRGTTE = tuning.TRGTTE;
      BOOST_CHECK_CLOSE(TRGTTE, 0.2, diff);

      double TRGCNV = tuning.TRGCNV;
      BOOST_CHECK_CLOSE(TRGCNV, 0.002, diff);

      double TRGMBE = tuning.TRGMBE;
      BOOST_CHECK_CLOSE(TRGMBE, 2.0E-7, diff);

      double TRGLCV = tuning.TRGLCV;
      BOOST_CHECK_CLOSE(TRGLCV, 0.0002, diff);

      double XXXTTE = tuning.XXXTTE;
      BOOST_CHECK_CLOSE(XXXTTE, 11.0, diff);

      double XXXCNV = tuning.XXXCNV;
      BOOST_CHECK_CLOSE(XXXCNV, 0.02, diff);

      double XXXMBE = tuning.XXXMBE;
      BOOST_CHECK_CLOSE(XXXMBE, 2.0E-6, diff);

      double XXXLCV = tuning.XXXLCV;
      BOOST_CHECK_CLOSE(XXXLCV, 0.002, diff);

      double XXXWFL = tuning.XXXWFL;
      BOOST_CHECK_CLOSE(XXXWFL, 0.002, diff);

      double TRGFIP = tuning.TRGFIP;
      BOOST_CHECK_CLOSE(TRGFIP, 0.035, diff);

      BOOST_CHECK_EQUAL(true, tuning.TRGSFT_has_value);
      BOOST_CHECK_CLOSE(tuning.TRGSFT, 66.0, diff);

      double THIONX = tuning.THIONX;
      BOOST_CHECK_CLOSE(THIONX, 0.02, diff);

      int TRWGHT = tuning.TRWGHT;
      BOOST_CHECK_EQUAL(TRWGHT, 2);

      /********* Record 3 ***********/
      int NEWTMX = tuning.NEWTMX;
      BOOST_CHECK_EQUAL(NEWTMX, 13);

      int NEWTMN = tuning.NEWTMN;
      BOOST_CHECK_EQUAL(NEWTMN, 2);

      int LITMAX = tuning.LITMAX;
      BOOST_CHECK_EQUAL(LITMAX, 26);

      int LITMIN = tuning.LITMIN;
      BOOST_CHECK_EQUAL(LITMIN, 2);

      int MXWSIT = tuning.MXWSIT;
      BOOST_CHECK_EQUAL(MXWSIT, 9);

      int MXWPIT = tuning.MXWPIT;
      BOOST_CHECK_EQUAL(MXWPIT, 9);

      double DDPLIM = tuning.DDPLIM;
      BOOST_CHECK_CLOSE(DDPLIM, 4.0E6 * Metric::Pressure, diff);

      double DDSLIM = tuning.DDSLIM;
      BOOST_CHECK_CLOSE(DDSLIM, 4.0E6, diff);

      double TRGDPR = tuning.TRGDPR;
      BOOST_CHECK_CLOSE(TRGDPR, 4.0E6 * Metric::Pressure, diff);

      BOOST_CHECK_EQUAL(true, tuning.XXXDPR_has_value);
      BOOST_CHECK_CLOSE(tuning.XXXDPR, 1.0 * Metric::Pressure, diff);
  }

  /*** TIMESTEP 7 ***/
  {
      std::size_t timestep = 7;
      BOOST_CHECK(!event.hasEvent(ScheduleEvents::TUNING_CHANGE, timestep));
  }

  /*** TIMESTEP 10 ***/
  {
      /********* Record 1 ***********/
      std::size_t timestep = 10;
      const auto& tuning = schedule.getTuning(timestep);
      BOOST_CHECK(event.hasEvent(ScheduleEvents::TUNING_CHANGE, timestep));
      BOOST_CHECK_EQUAL(true, tuning.TMAXWC_has_value);
      BOOST_CHECK_CLOSE(tuning.TMAXWC, 10.0 * Metric::Time, diff);

      BOOST_CHECK_EQUAL(tuning.MXWSIT, ParserKeywords::WSEGITER::MAX_WELL_ITERATIONS::defaultValue);
  }
}
