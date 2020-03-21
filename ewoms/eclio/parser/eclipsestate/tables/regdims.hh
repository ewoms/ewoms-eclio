/*

  This file is part of the eWoms project.

  eWoms is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  eWoms is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANREGILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with eWoms.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REGDIMS_H
#define REGDIMS_H

/*
  The Regdims class is a small utility class designed to hold on to
  the values from the REGDIMS keyword.
*/

#include <ewoms/eclio/parser/parserkeywords/r.hh>

namespace Ewoms {
    class Regdims {
    public:

        Regdims() :
            m_NTFIP( ParserKeywords::REGDIMS::NTFIP::defaultValue ),
            m_NMFIPR( ParserKeywords::REGDIMS::NMFIPR::defaultValue ),
            m_NRFREG( ParserKeywords::REGDIMS::NRFREG::defaultValue ),
            m_NTFREG( ParserKeywords::REGDIMS::NTFREG::defaultValue ),
            m_NPLMIX( ParserKeywords::REGDIMS::NPLMIX::defaultValue )
        { }

        Regdims(size_t ntfip , size_t nmfipr , size_t nrfregr , size_t ntfreg , size_t nplmix) :
            m_NTFIP( ntfip ),
            m_NMFIPR( nmfipr ),
            m_NRFREG( nrfregr ),
            m_NTFREG(  ntfreg ),
            m_NPLMIX( nplmix )
        {}

        size_t getNTFIP() const {
            return m_NTFIP;
        }

        size_t getNMFIPR() const {
            return m_NMFIPR;
        }

        size_t getNRFREG() const {
            return m_NRFREG;
        }

        size_t getNTFREG() const {
            return m_NTFREG;
        }

        size_t getNPLMIX() const {
            return m_NPLMIX;
        }

        bool operator==(const Regdims& data) const {
            return this->getNTFIP() == data.getNTFIP() &&
                   this->getNMFIPR() == data.getNMFIPR() &&
                   this->getNRFREG() == data.getNRFREG() &&
                   this->getNTFREG() == data.getNTFREG() &&
                   this->getNPLMIX() == data.getNPLMIX();
        }

        template<class Serializer>
        void serializeOp(Serializer& serializer)
        {
            serializer(m_NTFIP);
            serializer(m_NMFIPR);
            serializer(m_NRFREG);
            serializer(m_NTFREG);
            serializer(m_NPLMIX);
        }

    private:
        size_t m_NTFIP;
        size_t m_NMFIPR;
        size_t m_NRFREG;
        size_t m_NTFREG;
        size_t m_NPLMIX;
    };
}

#endif
