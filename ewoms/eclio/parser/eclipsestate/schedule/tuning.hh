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

#ifndef EWOMS_TUNING_H
#define EWOMS_TUNING_H

#include <ewoms/eclio/parser/units/units.hh>
#include <ewoms/eclio/parser/parserkeywords/t.hh>

namespace Ewoms {
    struct Tuning {
        using TuningKw = ParserKeywords::TUNING;

        // Record1
        double TSINIT = TuningKw::TSINIT::defaultValue * Metric::Time;
        double TSMAXZ = TuningKw::TSMAXZ::defaultValue * Metric::Time;
        double TSMINZ = TuningKw::TSMINZ::defaultValue * Metric::Time;
        double TSMCHP = TuningKw::TSMCHP::defaultValue * Metric::Time;
        double TSFMAX = TuningKw::TSFMAX::defaultValue;
        double TSFMIN = TuningKw::TSFMIN::defaultValue;
        double TFDIFF = TuningKw::TFDIFF::defaultValue;
        double TSFCNV = TuningKw::TSFCNV::defaultValue;
        double THRUPT = TuningKw::THRUPT::defaultValue;
        double TMAXWC = 0.0;
        bool TMAXWC_has_value = false;

        // Record 2
        double TRGTTE = TuningKw::TRGTTE::defaultValue;
        double TRGCNV = TuningKw::TRGCNV::defaultValue;
        double TRGMBE = TuningKw::TRGMBE::defaultValue;
        double TRGLCV = TuningKw::TRGLCV::defaultValue;
        double XXXTTE = TuningKw::XXXTTE::defaultValue;
        double XXXCNV = TuningKw::XXXCNV::defaultValue;
        double XXXMBE = TuningKw::XXXMBE::defaultValue;
        double XXXLCV = TuningKw::XXXLCV::defaultValue;
        double XXXWFL = TuningKw::XXXWFL::defaultValue;
        double TRGFIP = TuningKw::TRGFIP::defaultValue;
        double TRGSFT = 0.0;
        bool TRGSFT_has_value = false;
        double THIONX = TuningKw::THIONX::defaultValue;
        double TRWGHT = TuningKw::TRWGHT::defaultValue;

        // Record 3
        int NEWTMX = TuningKw::NEWTMX::defaultValue;
        int NEWTMN = TuningKw::NEWTMN::defaultValue;
        int LITMAX = TuningKw::LITMAX::defaultValue;
        int LITMIN = TuningKw::LITMIN::defaultValue;
        int MXWSIT = TuningKw::MXWSIT::defaultValue;
        int MXWPIT = TuningKw::MXWPIT::defaultValue;
        double DDPLIM = TuningKw::DDPLIM::defaultValue * Metric::Pressure;
        double DDSLIM = TuningKw::DDSLIM::defaultValue;
        double TRGDPR = TuningKw::TRGDPR::defaultValue * Metric::Pressure;
        double XXXDPR = 0.0 * Metric::Pressure;
        bool XXXDPR_has_value = false;

        bool operator==(const Tuning& data) const {
            return TSINIT == data.TSINIT &&
                   TSMAXZ == data.TSMAXZ &&
                   TSMINZ == data.TSMINZ &&
                   TSMCHP == data.TSMCHP &&
                   TSFMAX == data.TSFMAX &&
                   TSFMIN == data.TSFMIN &&
                   TSFCNV == data.TSFCNV &&
                   TFDIFF == data.TFDIFF &&
                   THRUPT == data.THRUPT &&
                   TMAXWC == data.TMAXWC &&
                   TMAXWC_has_value == data.TMAXWC_has_value &&
                   TRGTTE == data.TRGTTE &&
                   TRGCNV == data.TRGCNV &&
                   TRGMBE == data.TRGMBE &&
                   TRGLCV == data.TRGLCV &&
                   XXXTTE == data.XXXTTE &&
                   XXXCNV == data.XXXCNV &&
                   XXXMBE == data.XXXMBE &&
                   XXXLCV == data.XXXLCV &&
                   XXXWFL == data.XXXWFL &&
                   TRGFIP == data.TRGFIP &&
                   TRGSFT == data.TRGSFT &&
                   TRGSFT_has_value == data.TRGSFT_has_value &&
                   THIONX == data.THIONX &&
                   TRWGHT == data.TRWGHT &&
                   NEWTMX == data.NEWTMX &&
                   NEWTMN == data.NEWTMN &&
                   LITMAX == data.LITMAX &&
                   LITMIN == data.LITMIN &&
                   MXWSIT == data.MXWSIT &&
                   MXWPIT == data.MXWPIT &&
                   DDPLIM == data.DDPLIM &&
                   DDSLIM == data.DDSLIM &&
                   TRGDPR == data.TRGDPR &&
                   XXXDPR == data.XXXDPR &&
                   XXXDPR_has_value == data.XXXDPR_has_value;
        }

        bool operator !=(const Tuning& data) const {
            return !(*this == data);
        }

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(TSINIT);
            serializer(TSMAXZ);
            serializer(TSMINZ);
            serializer(TSMCHP);
            serializer(TSFMAX);
            serializer(TSFMIN);
            serializer(TFDIFF);
            serializer(TSFCNV);
            serializer(THRUPT);
            serializer(TMAXWC);
            serializer(TMAXWC_has_value);

            serializer(TRGTTE);
            serializer(TRGCNV);
            serializer(TRGMBE);
            serializer(TRGLCV);
            serializer(XXXTTE);
            serializer(XXXCNV);
            serializer(XXXMBE);
            serializer(XXXLCV);
            serializer(XXXWFL);
            serializer(TRGFIP);
            serializer(TRGSFT);
            serializer(TRGSFT_has_value);
            serializer(THIONX);
            serializer(TRWGHT);

            serializer(NEWTMX);
            serializer(NEWTMN);
            serializer(LITMAX);
            serializer(LITMIN);
            serializer(MXWSIT);
            serializer(MXWPIT);
            serializer(DDPLIM);
            serializer(DDSLIM);
            serializer(TRGDPR);
            serializer(XXXDPR);
            serializer(XXXDPR_has_value);
        }
    };

} //namespace Ewoms

#endif
